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

    sf::Vector2f TransformComponent::GetRelativePosition(int _componentIndex, float _frameDelta) const
    {
        return position[_componentIndex];
    }

    sf::Vector2f TransformComponent::GetRelativeScale(int _componentIndex, float _frameDelta) const
    {
        return scale[_componentIndex];
    }

    float TransformComponent::GetRelativeRotation(int _componentIndex, float _frameDelta) const
    {
        return rotation[_componentIndex];
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
        }
    }

    void TransformComponent::SetScale(int _componentIndex, sf::Vector2f _scale)
    {
        if (scale[_componentIndex] != _scale)
        {
            dirty[_componentIndex] = true;
            scale[_componentIndex] = _scale;
        }
    }

    void TransformComponent::SetRotation(int _componentIndex, float _angle)
    {

        if (rotation[_componentIndex] != _angle)
        {
            dirty[_componentIndex] = true;
            rotation[_componentIndex] = _angle;
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
        scale.reserve(_initialCapacity);
        rotation.reserve(_initialCapacity);

        return true;
    }

    void TransformComponent::AddComponentInternal(int _entityId, int _componentIndex)
    {
        depth[_componentIndex] = 0;

        position[_componentIndex] = sf::Vector2f(0, 0);
        scale[_componentIndex] = sf::Vector2f(1, 1);
        rotation[_componentIndex] = 0;

    }

    void TransformComponent::ClearComponentAtIndexInternal(int _componentIndex)
    {
        depth[_componentIndex] = 0;

        position[_componentIndex] = sf::Vector2f(0, 0);
        scale[_componentIndex] = sf::Vector2f(1, 1);
        rotation[_componentIndex] = 0;

    }

    void TransformComponent::SetCapacityInternal(int _newCapacity)
    {
        depth.resize(_newCapacity, 0);

        position.resize(_newCapacity, sf::Vector2f(0, 0));
        scale.resize(_newCapacity, sf::Vector2f(1, 1));
        rotation.resize(_newCapacity, 0.0f);

    }

    void TransformComponent::ProcessPhysicsInternal(float _delta)
    {
    }

    Transform TransformComponent::CreateLocalTransform(int _componentIndex) const
    {
        Transform transform;
        transform.setPosition(position[_componentIndex]);
        transform.setScale(scale[_componentIndex]);
        transform.setRotation(sf::degrees(rotation[_componentIndex]));
        return transform;
    }

}
