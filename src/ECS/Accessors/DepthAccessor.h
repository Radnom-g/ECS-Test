//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "ComponentAccessor.h"
#include "Components/DepthComponent.h"

namespace ECS
{
    class DepthAccessor : public ComponentAccessor<DepthComponent>
    {
    public:
        bool Initialise(int _initialCapacity, int _maxCapacity) override;
        DepthComponent *AddComponent(int _entityId) override;
        void RemoveComponent(int _entityId) override;


        // First ask for a list of active Depths, in order.
        void GetDepths(std::vector<int>& _outDepths);
        // Then you can ask for the entities at this depth to render them.
        void GetEntitiesAtDepth(int _depth, std::vector<int>& _outEntities);

    protected:
        // length of this vector matches the size of our Depth Components array
        // and tells us what the current Depth is set to.
        std::vector<int> DepthLookup;

        // map of depth to the contents of that depth (as entity ID)
        std::multimap<int, int> DepthTable;

    };
} // ECS