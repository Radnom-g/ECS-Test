//
// Created by Radnom on 7/11/2025.
//

#pragma once
#include "Components/IComponent.h"
#include "SFML/System/Vector2.hpp"

namespace ECS_SFML
{
    class VelocityComponent : public ECS::IComponent
    {
        public:
        friend class MovementSystem;

        ~VelocityComponent() override = default;

        [[nodiscard]] const sf::Vector2f& GetVelocity (int _componentIndex) const { return velocity[_componentIndex]; }
        [[nodiscard]] const sf::Vector2f& GetAcceleration (int _componentIndex) const { return acceleration[_componentIndex]; }
        [[nodiscard]] float GetFriction (int _componentIndex) const { return friction[_componentIndex]; }
        [[nodiscard]] float GetMaxSpeed (int _componentIndex) const { return maxSpeed[_componentIndex]; }

        void SetVelocity(int _componentIndex, const sf::Vector2f& _velocity) { velocity[_componentIndex] = _velocity; }
        void SetAcceleration(int _componentIndex, const sf::Vector2f& _acceleration) { acceleration[_componentIndex] = _acceleration; }
        void SetFriction(int _componentIndex, float _friction) { friction[_componentIndex] = _friction; }
        void SetMaxSpeed(int _componentIndex, float _maxSpeed) { maxSpeed[_componentIndex] = _maxSpeed; }

        [[nodiscard]] const char* GetName() const override { return "VelocityComponent"; }
        [[nodiscard]] bool IsUniquePerEntity() const override { return true; }

    protected:
        bool InitialiseInternal(ECS::WorldContext* context, int _initialCapacity, int _maxCapacity) override;
        void AddComponentInternal(int _entityId, int _componentIndex) override;
        void ClearComponentAtIndexInternal(int _componentIndex) override;
        void SetCapacityInternal(int _newCapacity) override;

    protected:

        // Component data:
        std::vector<sf::Vector2f> velocity{};
        std::vector<sf::Vector2f> acceleration{};
        std::vector<float> friction{};
        std::vector<float> maxSpeed{}; // -1 is no max speed.

    };
} // ECS_SFML