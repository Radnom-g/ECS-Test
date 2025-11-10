//
// Created by Radnom on 6/11/2025.
//

#include "SpriteComponent.h"

#include "TransformComponent.h"
#include "Components/ComponentManager.h"
#include "Components/TreeComponent.h"
#include "Worlds/WorldContext.h"

namespace ECS_SFML
{

    bool SpriteComponent::InitialiseInternal(ECS::WorldContext *context, int _initialCapacity, int _maxCapacity)
    {
        visible.reserve(_initialCapacity);
        position.reserve(_initialCapacity);
        scale.reserve(_initialCapacity);
        relativeOrigin.reserve(_initialCapacity);
        rotation.reserve(_initialCapacity);
        spriteId.reserve(_initialCapacity);
        dirty.reserve(_initialCapacity);
        return true;
    }

    void SpriteComponent::AddComponentInternal(int _entityId, int _componentIndex)
    {
        visible[_componentIndex] = true;
        position[_componentIndex] = sf::Vector2f(0, 0);
        scale[_componentIndex] = sf::Vector2f(1, 1);
        relativeOrigin[_componentIndex] = sf::Vector2f(0.5, 0.5);
        rotation[_componentIndex] = 0;
        spriteId[_componentIndex] = -1;
        dirty[_componentIndex] = true;
    }

    void SpriteComponent::ClearComponentAtIndexInternal(int _componentIndex)
    {
        visible[_componentIndex] = false;
        position[_componentIndex] = sf::Vector2f(0, 0);
        scale[_componentIndex] = sf::Vector2f(1, 1);
        relativeOrigin[_componentIndex] = sf::Vector2f(0.5, 0.5);
        rotation[_componentIndex] = 0;
        spriteId[_componentIndex] = -1;
        dirty[_componentIndex] = false;
    }

    void SpriteComponent::SetCapacityInternal(int _newCapacity)
    {
        visible.resize(_newCapacity, false);
        position.resize(_newCapacity, sf::Vector2f(0, 0));
        scale.resize(_newCapacity, sf::Vector2f(1, 1));
        relativeOrigin.resize(_newCapacity, sf::Vector2f(0.5, 0.5));
        rotation.resize(_newCapacity, 0.0f);
        spriteId.resize(_newCapacity, -1);
        dirty.resize(_newCapacity, false);
    }
}
