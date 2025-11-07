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
    void TransformComponent::SetDepth(int _componentIndex, int _depth)
    {
        if (depth[_componentIndex] != _depth)
        {
            depthDirty = true; // tell the RenderSystem to recalculate
            depth[_componentIndex] = _depth;
        }
    }

    bool TransformComponent::InitialiseInternal(ECS::WorldContext *context, int _initialCapacity, int _maxCapacity)
    {
        treeComponent = context->componentManager->GetComponent<ECS::TreeComponent>();

        depth.reserve(_initialCapacity);

        position.reserve(_initialCapacity);
        scale.reserve(_initialCapacity);
        rotation.reserve(_initialCapacity);
        translationDirty.reserve(_initialCapacity);

        return true;
    }

    void TransformComponent::AddComponentInternal(int _entityId, int _componentIndex)
    {
        depth[_componentIndex] = 0;

        position[_componentIndex] = sf::Vector2f(0, 0);
        scale[_componentIndex] = sf::Vector2f(1, 1);
        rotation[_componentIndex] = 0;
        translationDirty[_componentIndex] = true;
    }

    void TransformComponent::ClearComponentAtIndexInternal(int _componentIndex)
    {
        depth[_componentIndex] = 0;

        position[_componentIndex] = sf::Vector2f(0, 0);
        scale[_componentIndex] = sf::Vector2f(1, 1);
        rotation[_componentIndex] = 0;
        translationDirty[_componentIndex] = true;

    }

    void TransformComponent::SetCapacityInternal(int _newCapacity)
    {
        depth.resize(_newCapacity, 0);

        position.resize(_newCapacity, sf::Vector2f(0, 0));
        scale.resize(_newCapacity, sf::Vector2f(1, 1));
        rotation.resize(_newCapacity, 0.0f);
        translationDirty.resize(_newCapacity, true);
    }

}
