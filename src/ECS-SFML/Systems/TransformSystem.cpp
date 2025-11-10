//
// Created by Radnom on 5/11/2025.
//

#include "TransformSystem.h"

#include <debugapi.h>

#include "CollisionSystem.h"
#include "../Worlds/SFMLWorldContext.h"
#include "Components/ComponentManager.h"
#include "../ECS-SFML/Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/TreeComponent.h"
#include "Entities/EntityManager.h"


namespace ECS_SFML
{
    bool TransformSystem::Initialise(SFMLWorldContext *_context)
    {
        if (isInitialised)
        {
            assert(false && "TransformSystem::Initialise initialised twice.");
            return false;
        }

        transformComponent = _context->componentManager->GetComponent<TransformComponent>();
        treeComponent = _context->componentManager->GetComponent<ECS::TreeComponent>();
        collisionComponent = _context->componentManager->GetComponent<ECS::CollisionComponent>();
        entityManager = _context->entityManager;

        isInitialised = true;

        identityTransform = Transform::Identity();

        return true;
    }

    Transform TransformSystem::GetLerpEntityWorldTransform(int _entityId, float _frameDelta) const
    {
        int transformInd = transformComponent->GetComponentIndex(_entityId);
        if (transformInd != -1)
        {
            return GetLerpWorldTransform(transformInd, _frameDelta);
        }
        return identityTransform;
    }

    Transform TransformSystem::GetLerpWorldTransform(int _transformComponentIndex, float _frameDelta) const
    {
        if (_transformComponentIndex < cachedTransform.size())
        {
            return Transform::Lerp(cachedTransformPrev[_transformComponentIndex], cachedTransform[_transformComponentIndex], _frameDelta);
        }
        return transformComponent->CreateLocalTransform(_transformComponentIndex);
    }

    sf::Vector2f TransformSystem::GetEntityWorldPosition(int _entityId) const
    {
        return GetEntityWorldTransform(_entityId).getPosition();
    }

    const Transform& TransformSystem::GetEntityWorldTransform(int _entityId) const
    {
        int transformInd = transformComponent->GetComponentIndex(_entityId);
        if (transformInd != -1)
        {
            return GetWorldTransform(transformInd);
        }
        return identityTransform;
    }

    const Transform& TransformSystem::GetWorldTransform(int _transformComponentIndex) const
    {
        if (_transformComponentIndex < cachedTransform.size())
        {
            return cachedTransform[_transformComponentIndex];
        }
        return identityTransform;
    }

    bool TransformSystem::HasCachedEntityWorldTransform(int _entityId) const
    {
        int transformInd = transformComponent->GetComponentIndex(_entityId);
        return HasCachedEntityWorldTransform(transformInd);
    }

    bool TransformSystem::HasCachedWorldTransform(int _transformComponentIndex) const
    {
        if (_transformComponentIndex < 0 || _transformComponentIndex >= transformPrevCacheSet.size())
            return false;
        return (transformPrevCacheSet[_transformComponentIndex]);
    }

    // Update an entity transform directly, and affect its children by updating the caches.
    // This is a bit expensive as we need to recalculate the local by inversing the parent transform.
    void TransformSystem::SetWorldTransform(int _transformInd, const Transform& _newTransform)
    {
        // Get the transform of our parent so that we can invert it.
        // It needs to be an identity matrix transformed by any parents so that it's a 'clean' parent transform.
        int entity = transformComponent->entityId[_transformInd];
        Transform parentWorld = Transform::Identity();
        Transform local = _newTransform;

        if (LocalToWorldTransform(entity, parentWorld))
        {
            // Figure out based on our new final transform, what our local transform is.
            // This is a little expensive but we only need to do it if we are actually a child entity.
            local = Transform::GetInverseTransform(parentWorld, local);
        }

        // Apply it to the transform component values.
        const sf::Transform& sfLocal = local.getTransform();
        transformComponent->SetPosition(_transformInd, Transform::GetPosition(sfLocal) + local.getOrigin());
        transformComponent->SetRotation(_transformInd, Transform::GetRotation(sfLocal).asDegrees());
        transformComponent->SetScale(_transformInd, Transform::GetScale(sfLocal));

        // Cache it
        cachedTransform[_transformInd] = local;

        // Update the cache on our children (if any).
        CalculateCachedTransformOfChildren(entity, local);
    }

    void TransformSystem::MoveTransform(int _transformInd, const sf::Vector2f &_movement)
    {
        if (!IsEmpty(_movement)) // check because we don't want to waste our time
        {
            // Calculate new relative position of the transform - it's relative because we're only moving this comp (and children).
            const sf::Vector2f pos = transformComponent->GetRelativePosition(_transformInd) + _movement;
            transformComponent->SetPosition(_transformInd, pos);

            // Update the cache
            SetTransformCache(_transformInd, transformComponent->CreateLocalTransform(_transformInd));

            // Update the cache on our children (if any).
            CalculateCachedTransformOfChildren(transformComponent->entityId[_transformInd], cachedTransform[_transformInd]);
        }
    }

    void TransformSystem::RotateTransform(int _transformInd, float _rotation)
    {
        if (_rotation != 0.0f)
        {
            // Calculate new relative position of the transform - it's relative because we're only moving this comp (and children).
            const float rot = transformComponent->GetRelativeRotation(_transformInd) + _rotation;
            transformComponent->SetRotation(_transformInd, rot);

            // Update the cache
            SetTransformCache(_transformInd, transformComponent->CreateLocalTransform(_transformInd));

            // Update the cache on our children (if any).
            CalculateCachedTransformOfChildren(transformComponent->entityId[_transformInd], cachedTransform[_transformInd]);
        }
    }

    void TransformSystem::ScaleTransform(int _transformInd, const sf::Vector2f& _scale)
    {
        if (!IsEmpty(_scale)) // check because we don't want to waste our time
        {
            // Calculate new relative scale of the transform - it's relative because we're only moving this comp (and children).
            const sf::Vector2f pos = transformComponent->GetRelativeScale(_transformInd) + _scale;
            transformComponent->SetScale(_transformInd, pos);

            // Update the cache
            SetTransformCache(_transformInd, transformComponent->CreateLocalTransform(_transformInd));

            // Update the cache on our children (if any).
            CalculateCachedTransformOfChildren(transformComponent->entityId[_transformInd], cachedTransform[_transformInd]);
        }
    }

    // Try and move an Entity - either moving the transform, or looking up the tree for the first transform above it.
    void TransformSystem::MoveEntity(int _entityId, const sf::Vector2f &_movement)
    {
        int _tformId = transformComponent->GetComponentIndex(_entityId);

        if (_tformId == -1)
        {
            // go up the tree
            int _parId;
            while ( ( _parId = treeComponent->GetParent(_entityId) ) != -1)
            {
                _tformId = transformComponent->GetComponentIndex(_parId);
                // if we don't have a transform, keep going up the tree until we find one (or reach the top).
                if (_tformId != -1)
                {
                    break;
                }
            }
        }

        if (_tformId != -1)
        {
            MoveTransform(_tformId, _movement);
        }
    }

    void TransformSystem::RotateEntity(int _entityId, float _rotation)
    {
        RotateTransform(transformComponent->GetComponentIndex(_entityId), _rotation);
    }

    void TransformSystem::ScaleEntity(int _entityId, const sf::Vector2f &_scale)
    {
        ScaleTransform(transformComponent->GetComponentIndex(_entityId), _scale);
    }

    // Pass in your local transform and this will get the multiplied resulting transform based on Transforms
    // up the tree.
    bool TransformSystem::LocalToWorldTransform(int _entityId, Transform& _outLocalTransformToMakeWorld)
    {
        // Check parents recursively until we find one with a Transform, or run out.
        while ( (_entityId = treeComponent->GetParent(_entityId)) != -1)
        {
            int parentTransformInd = transformComponent->GetComponentIndex(_entityId);
            // if we don't have a transform, keep going up the tree until we find one (or reach the top).
            if (parentTransformInd != -1)
            {
                if (!transformNeedsCache[parentTransformInd])
                {
                    // Already cached it
                    _outLocalTransformToMakeWorld = Transform::GetAppliedTransform(cachedTransform[parentTransformInd], _outLocalTransformToMakeWorld);
                    return true;
                }
                else
                {
                    // Calculate and Cache it (recursively)

                    // Find the world of this Parent entity's transform by checking further up the tree until we hit a cache or the top
                    Transform _parentWorldTransform = transformComponent->CreateLocalTransform(parentTransformInd);

                    // Recursion!
                    LocalToWorldTransform(_entityId, _parentWorldTransform);

                    // Cache it!
                    SetTransformCache(parentTransformInd, _parentWorldTransform);

                    // Now we have the next Transform up the tree.
                    _outLocalTransformToMakeWorld = Transform::GetAppliedTransform(_parentWorldTransform, _outLocalTransformToMakeWorld);

                    return true;
                }
            }
        }
        return false;
    }

    // Note: this goes off Entity ID, not transform index!
    void TransformSystem::CalculateCachedTransformOfChildren(int _entityId, const Transform& _parentTransform)
    {
        // check for children.
        const IndexList& children = treeComponent->GetChildren(_entityId);
        if (children.size() != 0)
        {
            Transform newChildTransform = _parentTransform;

            for ( int childEntity : children)
            {
                if (childEntity == -1)
                    continue;

                // If this component has a Transform, update the cached transform.
                int transformInd = transformComponent->GetComponentIndex(childEntity);
                if (transformInd != -1)
                {
                    newChildTransform = transformComponent->CreateLocalTransform(transformInd);
                    newChildTransform = Transform::GetAppliedTransform(_parentTransform, newChildTransform);
                    SetTransformCache(transformInd, newChildTransform);
                }

                CalculateCachedTransformOfChildren(childEntity, newChildTransform);
            }
        }
    }

    void TransformSystem::ProcessInternal(float _deltaTick)
    {
        cachedTransformPrev.assign(cachedTransform.begin(), cachedTransform.end());

        if ( transformComponent->GetArraySize() > cachedTransform.size())
        {
            ResizeCache();
        }

        // This is used so we don't calculate transforms multiple times
        // because we're going through all of them, we could set it on a parent of multiple children.
        // First, only set it on transform components that have changed and been marked dirty.
        transformNeedsCache.assign(transformComponent->translationDirty.begin(), transformComponent->translationDirty.end());

        const Transform identity = Transform::Identity();

        // Go down from dirty transforms to ensure children are updated.
        // TODO: We should have a way to get this sorted by tree depth to avoid potentially updating children multiple times.
        for (int tCompInd = 0; tCompInd < transformComponent->entityId.size(); ++tCompInd)
        {
            translationMovedThisTick[tCompInd] = false;

            if (transformNeedsCache[tCompInd])
            {
                int entity = transformComponent->entityId[tCompInd];
                if (entity == -1)
                {
                    transformNeedsCache[tCompInd] = false;
                    transformPrevCacheSet[tCompInd] = false;
                    continue;
                }

                // Update the cache

                // Find the world of this Parent entity's transform by checking further up the tree until we hit a cache or the top
                Transform _worldTransform = transformComponent->CreateLocalTransform(tCompInd);

                // Recursion!
                LocalToWorldTransform(transformComponent->entityId[tCompInd], _worldTransform);

                // Cache it!
                SetTransformCache(tCompInd, _worldTransform);

                // send it down the chain!
                CalculateCachedTransformOfChildren(entity, identity);
            }
        }

    }

    void TransformSystem::GetProcessAfter(std::vector<std::string> &_outSystems)
    {
        _outSystems.clear();
        _outSystems.push_back( CollisionSystem::SystemName );
    }

    void TransformSystem::MarkEntityAsTeleported(int _entityId)
    {
        int _tComp = transformComponent->GetComponentIndex(_entityId);
        if (_tComp != -1)
        {
            cachedTransformPrev[_tComp] = cachedTransform[_tComp];
            transformPrevCacheSet[_tComp] = true;
        }

        // Have to tell children too, recursively.
        const IndexList& children = treeComponent->GetChildren(_entityId);
        if (children.size() != 0)
        {
            for ( int childEntity : children)
            {
                if (childEntity == -1)
                    continue;

                MarkEntityAsTeleported(childEntity);
            }
        }
    }

    bool TransformSystem::HasEntityTeleportedThisFrame(int _entityId)
    {
        int _tComp = transformComponent->GetComponentIndex(_entityId);
        if (_tComp != -1)
        {
            return HasTransformTeleportedThisFrame(_tComp);
        }
        return false;
    }

    bool TransformSystem::HasTransformTeleportedThisFrame(int _transformComponentIndex)
    {
        return cachedTransformPrev[_transformComponentIndex].getPosition() == cachedTransform[_transformComponentIndex].getPosition();
    }

    void TransformSystem::SetTransformCache(int _transformIndex, const Transform &_worldTransform)
    {
        if ( transformComponent->GetArraySize() > cachedTransform.size())
        {
            ResizeCache();
        }

        int entityId = transformComponent->entityId[_transformIndex];

        if (!transformPrevCacheSet[_transformIndex])
        {
            cachedTransformPrev[_transformIndex] = _worldTransform;
            transformPrevCacheSet[_transformIndex] = true;

        }
        else
        {
            const sf::Vector2f& lastPos = cachedTransform[_transformIndex].getPosition();
            const sf::Vector2f& newPos = _worldTransform.getPosition();

        }

        cachedTransform[_transformIndex] = _worldTransform;

        transformNeedsCache[_transformIndex] = false;
        transformComponent->translationDirty[_transformIndex] = false;
        translationMovedThisTick[_transformIndex] = true;

        // DEBUG: logging when something moves.
        // std::stringstream strDebug;
        // strDebug << "Entity #" << transformComponent->entityId[_transformIndex] << " (" << entityManager->GetName(transformComponent->entityId[_transformIndex]) << ")";
        // strDebug << " transform #" << _transformIndex << " ";
        // strDebug << Transform::ToString(cachedTransform[_transformIndex]);
        //
        // OutputDebugString(strDebug.str().c_str());
    }

    void TransformSystem::ResizeCache()
    {
        int newSize = transformComponent->GetArraySize();
        cachedTransformSize = newSize;
        cachedTransform.resize(newSize);
        cachedTransformPrev.resize(newSize);
        transformNeedsCache.resize(newSize, true);
        transformPrevCacheSet.resize(newSize, false);
        translationMovedThisTick.resize(newSize, false);
    }
} // ECS_SFML