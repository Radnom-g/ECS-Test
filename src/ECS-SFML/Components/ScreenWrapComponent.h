//
// Created by Radnom on 7/11/2025.
//

#pragma once
#include "Components/IComponent.h"

namespace ECS_SFML
{
    // A 'Tag' component, no data but if attached to an entity, it will wrap around the screen when moved.
    class ScreenWrapComponent : public ECS::IComponent
    {
    public:
        friend class MovementSystem;

        ~ScreenWrapComponent() override = default;

        [[nodiscard]] const char* GetName() const override { return "ScreenWrapComponent"; }
        [[nodiscard]] bool IsUniquePerEntity() const override { return true; }

    protected:
        // No need for anything in here.
        bool InitialiseInternal(ECS::WorldContext* context, int _initialCapacity, int _maxCapacity) override { return true;};
        void AddComponentInternal(int _entityId, int _componentIndex) override {}
        void ClearComponentAtIndexInternal(int _componentIndex) override {}
        void SetCapacityInternal(int _newCapacity) override {}

        // No component data.

    };
} // ECS_SFML