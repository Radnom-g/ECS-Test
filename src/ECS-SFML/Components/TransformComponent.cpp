//
// Created by Radnom on 5/11/2025.
//
#include "TransformComponent.h"
#include "Transform.h"
#include "Components/TreeComponent.h"
#include "Components/ComponentManager.h"
#include "Worlds/WorldContext.h"

namespace ECS_SFML
{
    int TransformComponent::GetDepth(int _componentIndex) const
    {
        return depth[_componentIndex];
    }

    Transform TransformComponent::GetEntityWorldTransform(int _entityId, float _frameDelta)
    {
        if (HasComponent(_entityId))
        {
            return GetWorldTransform(_entityId, _frameDelta);
        }

        // we have to check for a Transform in parents
        int parent = treeComponent->GetParent(_entityId);
        if (parent != -1)
        {
            // keep seaching up the tree until we find our first Transform component
            while (parent != -1)
            {
                if (int parentCompIndex = GetComponentIndex(parent) != -1)
                {
                    return (GetWorldTransform(parentCompIndex, _frameDelta));
                }
                // Keep going up the chain until we find a Transform comp
                parent = treeComponent->GetParent(parent);
            }
        }

        return Transform::Identity();
    }

    Transform TransformComponent::GetWorldTransform(int _componentIndex, float _frameDelta)
    {
        Transform result = CreateLocalTransform(_componentIndex);

        if (hasCachedTransform[_componentIndex])
        {
            result = cachedTransform[_componentIndex];
        }
        else
        {
            // If we have parents, find their World Transform.
            int parent = treeComponent->GetParent(entityId[_componentIndex]);
            if (parent != -1)
            {
                // keep seaching up the tree until we find our first Transform component
                while (parent != -1)
                {
                    if (int parentCompIndex = GetComponentIndex(parent) != -1)
                    {
                        Transform parentT = GetWorldTransform(_componentIndex, _frameDelta);

                        result = Transform::GetAppliedTransform(parentT, result);
                        break;
                    }
                    // Keep going up the chain until we find a Transform comp
                    parent = treeComponent->GetParent(parent);
                }
            }
        }

        cachedTransform[_componentIndex] = result;
        hasCachedTransform[_componentIndex] = true;

        if (_frameDelta >= 1.0f)
        {
            return result;
        }

        if (!hasCachedTransformPrev[_componentIndex])
        {
            CachePrevWorldTransform(_componentIndex);
        }

        return Transform::Lerp(cachedTransformPrev[_componentIndex], result, _frameDelta);
    }

    sf::Vector2f TransformComponent::GetRelativePosition(int _componentIndex, float _frameDelta) const
    {
        const sf::Vector2f diff = position[_componentIndex] - positionPrev[_componentIndex];
        return positionPrev[_componentIndex] + (diff * _frameDelta);
    }

    sf::Vector2f TransformComponent::GetRelativeScale(int _componentIndex, float _frameDelta) const
    {
        const sf::Vector2f diff = scale[_componentIndex] - scalePrev[_componentIndex];
        return scalePrev[_componentIndex] + (diff * _frameDelta);
    }

    float TransformComponent::GetRelativeRotation(int _componentIndex, float _frameDelta) const
    {
        const float diff = rotation[_componentIndex] - rotationPrev[_componentIndex];
        return rotationPrev[_componentIndex] + (diff * _frameDelta);
    }

    void TransformComponent::SetDepth(int _componentIndex, int _depth)
    {
        if (depth[_componentIndex] != _depth)
        {
            depthListDirty = true;
            depthMapDirty = true;
            depth[_componentIndex] = _depth;
        }
    }

    void TransformComponent::SetPosition(int _componentIndex, sf::Vector2f _position)
    {
        if (position[_componentIndex] != _position)
        {
            dirty[_componentIndex] = true;
            position[_componentIndex] = _position;

            // Clear our cache
            hasCachedTransform[_componentIndex] = false;
        }
    }

    void TransformComponent::SetScale(int _componentIndex, sf::Vector2f _scale)
    {
        if (scale[_componentIndex] != _scale)
        {
            dirty[_componentIndex] = true;
            scale[_componentIndex] = _scale;

            // Clear our cache
            hasCachedTransform[_componentIndex] = false;
        }
    }

    void TransformComponent::SetRotation(int _componentIndex, float _angle)
    {

        if (rotation[_componentIndex] != _angle)
        {
            dirty[_componentIndex] = true;
            rotation[_componentIndex] = _angle;

            // Clear our cache
            hasCachedTransform[_componentIndex] = false;
        }
    }

    const std::vector<int> & TransformComponent::GetDepths()
    {
        if (depthListDirty)
        {
            if (depthMapDirty)
            {
                PopulateDepthEntityMap();
            }

            depthList.clear();

            for(  auto it = depthEntityMap.begin(), end = depthEntityMap.end(); it != end; it = depthEntityMap.upper_bound(it->first))
            {
                depthList.push_back(it->first);
            }
            depthListDirty = false;
        }

        return depthList;
    }

    std::pair<std::multimap<int, int>::const_iterator, std::multimap<int, int>::const_iterator> TransformComponent::GetEntitiesAtDepth(int _depth)
    {
        if (depthMapDirty)
        {
            PopulateDepthEntityMap();
        }

        return depthEntityMap.equal_range(_depth);
    }


    void TransformComponent::PopulateDepthEntityMap()
    {
        depthEntityMap.clear();
        for (int i = 0; i < depth.size(); i++)
        {
            if (active[i])
            {
                depthEntityMap.emplace(depth[i], i);
            }
        }
        depthMapDirty = false;
    }


    bool TransformComponent::InitialiseInternal(ECS::WorldContext *context, int _initialCapacity, int _maxCapacity)
    {
        treeComponent = context->componentManager->GetComponent<ECS::TreeComponent>();

        depth.reserve(_initialCapacity);

        position.reserve(_initialCapacity);
        positionPrev.reserve(_initialCapacity);

        scale.reserve(_initialCapacity);
        scalePrev.reserve(_initialCapacity);

        rotation.reserve(_initialCapacity);
        rotationPrev.reserve(_initialCapacity);

        hasCachedTransform.reserve(_initialCapacity);
        cachedTransform.reserve(_initialCapacity);
        hasCachedTransformPrev.reserve(_initialCapacity);
        cachedTransformPrev.reserve(_initialCapacity);

        return true;
    }

    void TransformComponent::AddComponentInternal(int _entityId, int _componentIndex)
    {
        depth[_componentIndex] = 0;

        position[_componentIndex] = sf::Vector2f(0, 0);
        positionPrev[_componentIndex] = sf::Vector2f(0, 0);
        scale[_componentIndex] = sf::Vector2f(1, 1);
        scalePrev[_componentIndex] = sf::Vector2f(1, 1);
        rotation[_componentIndex] = 0;
        rotationPrev[_componentIndex] = 0;

        hasCachedTransform[_componentIndex] = false;
        hasCachedTransformPrev[_componentIndex] = false;
    }

    void TransformComponent::ClearComponentAtIndexInternal(int _componentIndex)
    {
        depth[_componentIndex] = 0;

        position[_componentIndex] = sf::Vector2f(0, 0);
        positionPrev[_componentIndex] = sf::Vector2f(0, 0);
        scale[_componentIndex] = sf::Vector2f(1, 1);
        scalePrev[_componentIndex] = sf::Vector2f(1, 1);
        rotation[_componentIndex] = 0;
        rotationPrev[_componentIndex] = 0;

        hasCachedTransform[_componentIndex] = false;
        hasCachedTransformPrev[_componentIndex] = false;
    }

    void TransformComponent::SetCapacityInternal(int _newCapacity)
    {
        depth.resize(_newCapacity, 0);

        position.resize(_newCapacity, sf::Vector2f(0, 0));
        positionPrev.resize(_newCapacity, sf::Vector2f(0, 0));
        scale.resize(_newCapacity, sf::Vector2f(1, 1));
        scalePrev.resize(_newCapacity, sf::Vector2f(1, 1));
        rotation.resize(_newCapacity, 0.0f);
        rotationPrev.resize(_newCapacity, 0.0f);

        hasCachedTransform.resize(_newCapacity, false);
        hasCachedTransformPrev.resize(_newCapacity, false);

        cachedTransform.resize(_newCapacity, Transform::Identity());
        cachedTransformPrev.resize(_newCapacity, Transform::Identity());

    }

    void TransformComponent::ProcessPhysicsInternal(float _delta)
    {
        // Copy the position array.
        positionPrev.assign(position.begin(), position.end());
        scalePrev.assign(scale.begin(), scale.end());
        rotationPrev.assign(rotation.begin(), rotation.end());

        // If nothing's changed and cleared the flags, we can keep our caches!
        cachedTransformPrev.assign(cachedTransform.begin(), cachedTransform.end());
        hasCachedTransformPrev.assign(hasCachedTransform.begin(), hasCachedTransform.end());
    }


    Transform TransformComponent::CachePrevWorldTransform(int _componentIndex)
    {
        if (hasCachedTransformPrev[_componentIndex] )
        {
            return cachedTransformPrev[_componentIndex];
        }

        Transform result = CreateLocalTransformPrev(_componentIndex);

        // If we have parents, find their prev World Transform.
        int parent = treeComponent->GetParent(entityId[_componentIndex]);
        if (parent != -1)
        {
            // keep seaching up the tree until we find our first Transform component
            while (parent != -1)
            {
                if (int parentCompIndex = GetComponentIndex(parent) != -1)
                {
                    Transform parentT = CachePrevWorldTransform(_componentIndex);
                    result = Transform::GetAppliedTransform(parentT, result);
                    break;
                }
                // Keep going up the chain until we find a Transform comp
                parent = treeComponent->GetParent(parent);
            }
        }
        cachedTransformPrev[_componentIndex] = result;
        hasCachedTransformPrev[_componentIndex] = true;
        return result;
    }

    Transform TransformComponent::CreateLocalTransform(int _componentIndex) const
    {
        Transform transform;
        transform.setPosition(position[_componentIndex]);
        transform.setScale(scale[_componentIndex]);
        transform.setRotation(sf::degrees(rotation[_componentIndex]));
        return transform;
    }

    Transform TransformComponent::CreateLocalTransformPrev(int _componentIndex) const
    {
        Transform transform;
        transform.setPosition(positionPrev[_componentIndex]);
        transform.setScale(scalePrev[_componentIndex]);
        transform.setRotation(sf::degrees(rotationPrev[_componentIndex]));
        return transform;
    }

}
