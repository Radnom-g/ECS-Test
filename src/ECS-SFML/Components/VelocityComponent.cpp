//
// Created by Radnom on 7/11/2025.
//

#include "VelocityComponent.h"

namespace ECS_SFML
{
    bool VelocityComponent::InitialiseInternal(ECS::WorldContext *context, int _initialCapacity, int _maxCapacity)
    {
        velocity.reserve(_initialCapacity);
        acceleration.reserve(_initialCapacity);
        friction.reserve(_initialCapacity);
        maxSpeed.reserve(_initialCapacity);
        return true;
    }

    void VelocityComponent::AddComponentInternal(int _entityId, int _componentIndex)
    {
        velocity[_componentIndex] = sf::Vector2f(0, 0);
        acceleration[_componentIndex] = sf::Vector2f(0, 0);
        friction[_componentIndex] = 0.0f;
        maxSpeed[_componentIndex] = -1.0f;
    }

    void VelocityComponent::ClearComponentAtIndexInternal(int _componentIndex)
    {
        velocity[_componentIndex] = sf::Vector2f(0, 0);
        acceleration[_componentIndex] = sf::Vector2f(0, 0);
        friction[_componentIndex] = 0.0f;
        maxSpeed[_componentIndex] = -1.0f;
    }

    void VelocityComponent::SetCapacityInternal(int _newCapacity)
    {
        velocity.resize(_newCapacity, sf::Vector2f(0, 0));
        acceleration.resize(_newCapacity, sf::Vector2f(0, 0));
        friction.resize(_newCapacity, 0.0f);
        maxSpeed.resize(_newCapacity, -1.0f);
    }
} // ECS_SFML