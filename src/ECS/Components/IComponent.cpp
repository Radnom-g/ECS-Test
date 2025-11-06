//
// Created by Radnom on 5/11/2025.
//

#include "IComponent.h"

#include <algorithm>
#include <cassert>

namespace ECS
{
    bool IComponent::InitialiseComponent(WorldContext *context, int _initialCapacity, int _maxCapacity)
    {
        if (isInitialised)
        {
            assert(false && "Component Initialised Twice!");
            return false;
        }

        isInitialised = true;

        maxCapacity = _maxCapacity;
        entityId.reserve(_initialCapacity);
        active.reserve(_initialCapacity);
        dirty.reserve(_initialCapacity);

        return InitialiseInternal(context, _initialCapacity, _maxCapacity);
    }

    int IComponent::AddComponent(int _entityId)
    {
        if (IsUniquePerEntity())
        {
            if (HasComponent(_entityId))
                return entityIdComponentIndex[_entityId];
        }

        entityListDirty = true;

        int componentIndex = GetNextInactiveComponent();
        if (componentIndex != -1)
        {
            entityId[componentIndex] = _entityId;
            active[componentIndex] = true;

            if (IsUniquePerEntity())
            {
                entityIdComponentIndex.emplace(_entityId, componentIndex);
            }
            else
            {
                entityIdComponentIndexMap.emplace(_entityId, componentIndex);
            }

            AddComponentInternal(_entityId, componentIndex);

            nextIndex++;
        }

        return componentIndex;
    }

    void IComponent::RemoveComponentsFromEntity(int _entityId)
    {
        if (!HasComponent(_entityId))
            return;

        entityListDirty = true;

        // Call internal first, in case it relies on 'entityIdComponentIndex' or similar.
        RemoveComponentsFromEntityInternal(_entityId);

        if (IsUniquePerEntity())
        {
            int compIndex = EntityIdToComponentIndex(_entityId);
            ClearComponentAtIndex(compIndex);
            entityIdComponentIndex.erase(_entityId);
        }
        else
        {
            // get the range of component indices for this Entity
            auto iterPair = EntityIdToComponentRange(_entityId);
            for ( auto iter = iterPair.first; iter != iterPair.second; ++iter)
            {
                // Clear them out
                ClearComponentAtIndex(iter->second);
            }
            // erases all values for the key
            entityIdComponentIndexMap.erase(_entityId);
        }
    }

    void IComponent::RemoveComponent(int _componentIndex)
    {
        entityListDirty = true;
        // Call internal first, in case it relies on 'entityIdComponentIndex' or similar.
        RemoveComponentInternal(_componentIndex);

        if (IsUniquePerEntity())
        {
            int entity = entityId[_componentIndex];
            if (entity)
            {
                entityIdComponentIndex.erase(entity);
            }
        }
        else
        {
            int entity = entityId[_componentIndex];

            // erases matching Component Index values in the multimap for the Entity as key (should only be one match).
            auto [first, last] = entityIdComponentIndexMap.equal_range(entity);
            for (auto it = first; it != last; ++it)
            {
                if (it->second == _componentIndex)
                {
                    entityIdComponentIndexMap.erase(it);
                    break; //should only be one match.
                }
            }
        }

        ClearComponentAtIndex(_componentIndex);
    }

    void IComponent::ProcessPhysics(float _delta)
    {
        // Clear dirty flag.
        dirty.assign(dirty.size(), false);
        ProcessPhysicsInternal(_delta);
    }

    bool IComponent::HasComponent(int _entityId) const
    {
        if (IsUniquePerEntity())
        {
            auto iter = entityIdComponentIndex.find(_entityId);
            return iter != entityIdComponentIndex.end();
        }
        auto iter = entityIdComponentIndexMap.find(_entityId);
        return iter != entityIdComponentIndexMap.end();
    }

    void IComponent::GetEntitiesWithComponent(std::vector<int> &_outEntityList)
    {
        if (entityListDirty)
        {
            // We keep this map of entity ID -> component index up-to-date, so we can just return its keys.
            entityList.clear();
            for (auto iter = entityIdComponentIndexMap.begin(); iter != entityIdComponentIndexMap.end(); ++iter)
            {
                entityList.push_back((*iter).first);
            }
            entityListDirty = false;
        }
        _outEntityList = entityList;
    }

    void IComponent::FilterKeepEntitiesWithComponent(std::vector<int> &_entityListFilter)
    {
        auto iter = _entityListFilter.begin();
        while (iter != _entityListFilter.end())
        {
            int checkEntityId = *iter;
            if (entityIdComponentIndexMap.contains(checkEntityId))
            {
                ++iter;
            }
            else
            {
                iter = _entityListFilter.erase(iter);
            }
        }
    }

    void IComponent::FilterKeepEntitiesWithoutComponent(std::vector<int> &_entityListFilter)
    {
        auto iter = _entityListFilter.begin();
        while (iter != _entityListFilter.end())
        {
            int checkEntityId = *iter;
            if (entityIdComponentIndexMap.contains(checkEntityId))
            {
                iter = _entityListFilter.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    int IComponent::GetComponentIndex(int _entityId) const
    {
        auto iter = entityIdComponentIndex.find(_entityId);
        return iter != entityIdComponentIndex.end() ? iter->second : -1;
    }

    bool IComponent::GetComponentIndices(int _entityId, std::vector<int> &_componentIndexList)
    {
        _componentIndexList.clear();

        if (IsUniquePerEntity())
        {
            int compIndex = GetComponentIndex(_entityId);
            if (compIndex != -1)
            {
                _componentIndexList.push_back(compIndex);
                return true;
            }
            return false;
        }

        auto [first, last] = entityIdComponentIndexMap.equal_range(_entityId);
        for (auto it = first; it != last; ++it)
        {
            _componentIndexList.push_back(it->second);
        }

        return !_componentIndexList.empty();
    }

    void IComponent::ClearComponentAtIndex(int _componentIndex)
    {
        ClearComponentAtIndexInternal(_componentIndex);

        entityId[_componentIndex] = -1;
        active[_componentIndex] = false;
        dirty[_componentIndex] = false;
    }

    void IComponent::SetCapacity(int _newCapacity)
    {
        entityId.resize(_newCapacity, -1);
        active.resize(_newCapacity, false);
        dirty.resize(_newCapacity, false);

        SetCapacityInternal(_newCapacity);
    }

    int IComponent::GetNextInactiveComponent()
    {
        int capacity = (int)active.size();
        for (int i = 0; i < capacity; ++i)
        {
            int index = (nextIndex + i) % capacity;

            if (!active[index])
            {
                return index;
            }
        }

        if (capacity < maxCapacity || maxCapacity == -1)
        {
			int newCapacity = std::max( capacity * 2, 1);
            if (maxCapacity > 0)
            {
                newCapacity = std::min(newCapacity, maxCapacity);
            }
            if (newCapacity > capacity)
            {
                SetCapacity(newCapacity);
                nextIndex = capacity;
                return nextIndex;
            }
        }

        return -1;
    }
}
