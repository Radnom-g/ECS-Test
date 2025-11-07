//
// Created by Radnom on 5/11/2025.
//

#include "TransformSystem.h"

#include <debugapi.h>

#include "../Worlds/SFMLWorldContext.h"
#include "Components/ComponentManager.h"
#include "../ECS-SFML/Components/TransformComponent.h"
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
        entityManager = _context->entityManager;

        isInitialised = true;
        return true;
    }

    Transform TransformSystem::GetEntityWorldTransform(int _entityId, float _frameDelta)
    {
        int transformInd = transformComponent->GetComponentIndex(_entityId);
        if (transformInd != -1)
        {
            return GetWorldTransform(transformInd, _frameDelta);
        }
        return Transform::Identity();
    }

    Transform TransformSystem::GetWorldTransform(int _transformComponentIndex, float _frameDelta)
    {
        if (_transformComponentIndex < cachedTransform.size())
        {
            return Transform::Lerp(cachedTransformPrev[_transformComponentIndex], cachedTransform[_transformComponentIndex], _frameDelta);
        }
        return transformComponent->CreateLocalTransform(_transformComponentIndex);
    }

    // Update an entity transform directly, and affect its children by updating the caches.
    // This is a bit expensive as we need to recalculate the local by inversing the parent transform.
    void TransformSystem::SetWorldTransform(int _transformInd, const Transform& _newTransform)
    {
        // Get the transform of our parent so that we can invert it.
        Transform parent = GetParentTransform(_transformInd);

        // Figure out based on our new final transform, what our local transform is.
        Transform newLocal = Transform::GetInverseTransform(parent, _newTransform);

        // Apply it to the transform component values.
        const sf::Transform& local = newLocal.getTransform();
        transformComponent->SetPosition(_transformInd, Transform::GetPosition(local));
        transformComponent->SetRotation(_transformInd, Transform::GetRotation(local).asDegrees());
        transformComponent->SetScale(_transformInd, Transform::GetScale(local));

        // Cache it
        cachedTransform[_transformInd] = newLocal;

        // Update the cache on our children (if any).
        CalculateCachedTransformOfChildren(transformComponent->entityId[_transformInd], newLocal);
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

    Transform TransformSystem::GetParentTransform(int _transformCompIndex)
    {
        Transform outVal = Transform::Identity();
        int entity = transformComponent->entityId[_transformCompIndex];
        while ( (entity = treeComponent->GetParent(entity)) != -1)
        {
            int transformInd = transformComponent->GetComponentIndex(entity);
            // if we don't have a transform, keep going up the tree until we find one (or reach the top).
            if (transformInd != -1)
            {
                if (!transformNeedsCache[transformInd])
                {
                    // Already cached it
                    outVal = Transform::GetAppliedTransform(cachedTransform[transformInd], outVal);
                    return outVal;
                }
                else
                {
                    // Calculate and Cache it (recursively)
                    Transform parentTransform = GetParentTransform(transformInd);
                    Transform worldTransform = Transform::GetAppliedTransform(parentTransform, transformComponent->CreateLocalTransform(transformInd));
                    SetTransformCache(transformInd, worldTransform);
                    return worldTransform;
                }
            }
        }
        return outVal;
    }

    // Note: this goes off Entity ID, not transform index!
    void TransformSystem::CalculateCachedTransformOfChildren(int _entityId, const Transform& _parentTransform)
    {
        // check for children.
        std::vector<int> _outChildren;
        if (treeComponent->GetChildren(_entityId, _outChildren))
        {
            Transform newChildTransform = _parentTransform;

            for ( int childEntity : _outChildren)
            {
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
            if (transformNeedsCache[tCompInd])
            {
                int entity = transformComponent->entityId[tCompInd];
                if (entity == -1)
                {
                    transformNeedsCache[tCompInd] = false;
                    transformPrevCacheSet[tCompInd] = false;
                    continue;
                }
                CalculateCachedTransformOfChildren(entity, identity);
            }
        }
    }

    void TransformSystem::SetTransformCache(int _transformIndex, const Transform &_worldTransform)
    {
        if ( transformComponent->GetArraySize() > cachedTransform.size())
        {
            ResizeCache();
        }

        cachedTransform[_transformIndex] = _worldTransform;

        if (!transformPrevCacheSet[_transformIndex])
        {
            cachedTransformPrev[_transformIndex] = _worldTransform;
            transformPrevCacheSet[_transformIndex] = true;
        }

        transformNeedsCache[_transformIndex] = false;
        transformComponent->translationDirty[_transformIndex] = false;

        // DEBUG: remove!
        std::stringstream strDebug;
        strDebug << "Entity #" << transformComponent->entityId[_transformIndex] << " (" << entityManager->GetName(transformComponent->entityId[_transformIndex]) << ")";
        strDebug << " transform #" << _transformIndex << " ";
        strDebug << Transform::ToString(cachedTransform[_transformIndex]);

        OutputDebugString(strDebug.str().c_str());
    }

    void TransformSystem::ResizeCache()
    {
        int newSize = transformComponent->GetArraySize();
        cachedTransformSize = newSize;
        cachedTransform.resize(newSize);
        cachedTransformPrev.resize(newSize);
        transformNeedsCache.resize(newSize, true);
        transformPrevCacheSet.resize(newSize, false);
    }
} // ECS_SFML