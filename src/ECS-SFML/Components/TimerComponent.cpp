//
// Created by Radnom on 7/11/2025.
//

#include "TimerComponent.h"

namespace ECS_SFML
{
    bool TimerComponent::InitialiseInternal(ECS::WorldContext *context, int _initialCapacity, int _maxCapacity)
    {
        elapsed.reserve(_initialCapacity);
        target.reserve(_initialCapacity);
        loops.reserve(_initialCapacity);
        return true;
    }

    void TimerComponent::AddComponentInternal(int _entityId, int _componentIndex)
    {
        elapsed[_componentIndex] = sf::Time::Zero;
        target[_componentIndex] = sf::Time::Zero;
        loops[_componentIndex] = false;
    }

    void TimerComponent::ClearComponentAtIndexInternal(int _componentIndex)
    {
        elapsed[_componentIndex] = sf::Time::Zero;
        target[_componentIndex] = sf::Time::Zero;
        loops[_componentIndex] = false;
    }

    void TimerComponent::SetCapacityInternal(int _newCapacity)
    {
        elapsed.resize(_newCapacity, sf::Time::Zero);
        target.resize(_newCapacity, sf::Time::Zero);
        loops.resize(_newCapacity, false);
    }
} // ECS_SFML