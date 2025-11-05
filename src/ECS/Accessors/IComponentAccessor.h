//
// Created by Radnom on 3/11/2025.
//

#pragma once

namespace ECS
{
    // Interface for a controller that handles components of a particular type.
    // todo: Investigate 'Type Traits' for determining if a Controller has a Tick or Render?
    class IComponentAccessor
    {
    public:
        virtual ~IComponentAccessor() = default;
        virtual void EntityDestroyed(int _entityId) = 0;
        virtual void ProcessEndOfFrame() = 0;

        virtual const char* GetComponentName() const = 0;

        bool IsInitialised() const { return isInitialised; }

        void SetControllerId(int _controllerId) { controllerId = _controllerId; }

    protected:
		bool isInitialised = false;
		int controllerId = -1;
    };
} // ECS