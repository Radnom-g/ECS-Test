//
// Created by Radnom on 3/11/2025.
//

#pragma once

namespace ECS
{
    // Interface for a controller that handles components of a particular type.
    // todo: Investigate 'Type Traits' for determining if a Controller has a Tick or Render?
    class IComponentController
    {
    public:
        virtual ~IComponentController() = default;
        virtual void EntityDestroyed(int _entityId) = 0;
        virtual void ProcessEndOfFrame() = 0;
    };
} // ECS