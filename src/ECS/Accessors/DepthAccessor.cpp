//
// Created by Radnom on 4/11/2025.
//

#include "DepthAccessor.h"

#include <utility>

namespace ECS
{
    bool DepthAccessor::Initialise( int _initialCapacity, int _maxCapacity)
    {
        bool success = ComponentAccessor<DepthComponent>::Initialise( _initialCapacity, _maxCapacity);

        if (success)
        {
            DepthLookup.resize(components.size());
        }

        return success;
    }

    DepthComponent* DepthAccessor::AddComponent(int _entityId)
    {
        DepthComponent* newComp = ComponentAccessor<DepthComponent>::AddComponent(_entityId);
        if (newComp)
        {
            // Make sure we have enough elements in the DepthLookup array.
            int totalSize = components.size() + addedComponents.size();
            if (DepthLookup.size() != totalSize)
            {
                DepthLookup.resize(totalSize);
            }

            // Set initial Depth value in the DepthLookup.
            int compId = entityIdComponentIndexMap[_entityId];
            int componentIndex = componentIdIndexMap[compId];
            DepthLookup[compId] = newComp->depth;

            // Add it to the map.
            DepthTable.emplace(newComp->depth, componentIndex);
        }

        return newComp;
    }

    void DepthAccessor::RemoveComponent(int _entityId)
    {
        int compId = entityIdComponentIndexMap[_entityId];
        int currentDepth = DepthLookup[compId];
        int componentIndex = componentIdIndexMap[compId];
        auto [first, last] = DepthTable.equal_range(currentDepth);

        for (auto it = first; it != last; ++it)
        {
            if (it->second == componentIndex)
            {
                DepthTable.erase(it);
                break; // Stop after first match (only one should exist)
            }
        }

        DepthLookup[compId] = -1;

        ComponentAccessor<DepthComponent>::RemoveComponent(_entityId);


    }

    void DepthAccessor::GetDepths(std::vector<int> &_outDepths)
    {
        _outDepths.clear();

        for(  auto it = DepthTable.begin(), end = DepthTable.end(); it != end; it = DepthTable.upper_bound(it->first))
        {
            _outDepths.push_back(it->first);
        }
    }

    void DepthAccessor::GetEntitiesAtDepth(int _depth, std::vector<int> &_outEntities)
    {
        _outEntities.clear();
        auto [first, last] = DepthTable.equal_range(_depth);
        for (auto it = first; it != last; ++it)
        {
            _outEntities.push_back(it->second);
        }

    }
} // ECS