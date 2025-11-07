//
// Created by Radnom on 7/11/2025.
//

#pragma once
#include "Components/IComponent.h"
#include "SFML/System/Time.hpp"

namespace ECS_SFML
{
    // A timer that ticks up from creation.
    class TimerComponent : public ECS::IComponent
    {
        friend class TimerSystem;

        ~TimerComponent() override = default;

        // Time elapsed will likely be set directly by TimerSystem.
        [[nodiscard]] sf::Time GetTimeElapsed(int _componentIndex) const { return elapsed[_componentIndex]; }
        void SetTimeElapsed(int _componentIndex, const sf::Time& newTime) { elapsed[_componentIndex] = newTime; }

        [[nodiscard]] bool IsLooping(int _componentIndex) const { return loops[_componentIndex]; }
        void SetLooping(int _componentIndex, bool _looping) { loops[_componentIndex] = _looping; }

        void ResetTimer(int _componentIndex) { elapsed[_componentIndex] = sf::Time::Zero; }
        [[nodiscard]] bool HasEnded(int _componentIndex) const { return elapsed[_componentIndex] > target[_componentIndex] && target[_componentIndex] != sf::Time::Zero; }
        [[nodiscard]] int GetLoopCount(int _componentIndex) const { return loops[_componentIndex] ? static_cast<int>(elapsed[_componentIndex] / target[_componentIndex]) : 0; }

        [[nodiscard]] const char* GetName() const override { return "TimerComponent"; }
        [[nodiscard]] bool IsUniquePerEntity() const override { return false; }

    protected:
        bool InitialiseInternal(ECS::WorldContext* context, int _initialCapacity, int _maxCapacity) override;
        void AddComponentInternal(int _entityId, int _componentIndex) override;
        void ClearComponentAtIndexInternal(int _componentIndex) override;
        void SetCapacityInternal(int _newCapacity) override;

    protected:

        // Component data:

        // ticks up since component was made active.
        std::vector<sf::Time> elapsed{};

        // the timer is considered 'finished' if elapsed > target.
        std::vector<sf::Time> target{};

        // if the timer is meant to reset. Otherwise, TimerSystem will likely remove this component.
        std::vector<bool> loops{};

    };
} // ECS_SFML