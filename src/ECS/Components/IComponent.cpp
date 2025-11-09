//
// Created by Radnom on 5/11/2025.
//

#include "IComponent.h"

#include <algorithm>
#include <cassert>

namespace ECS
{
    bool IComponent::InitialiseComponent(WorldContext *context, int _componentId, int _initialCapacity, int _maxCapacity)
    {
        if (isInitialised)
        {
            assert(false && "Component Initialised Twice!");
            return false;
        }

        componentId = _componentId;
        isInitialised = true;

        maxCapacity = _maxCapacity;
        entityId.reserve(_initialCapacity);

        return InitialiseInternal(context, _initialCapacity, _maxCapacity);
    }

    int IComponent::AddComponent(int _entityId)
    {
        if (IsUniquePerEntity())
        {
            // If we can only have one, and we already have one, return it
            if (HasComponent(_entityId))
                return GetComponentIndex(_entityId);
        }

        int componentIndex = GetNextInactiveComponent();
        if (componentIndex != -1)
        {
            entityId[componentIndex] = _entityId;
            entityComponents[_entityId].push_back(componentIndex);
            AddComponentInternal(_entityId, componentIndex);
            nextIndex++;
            entityListDirty = true;
        }

        return componentIndex;
    }

    void IComponent::RemoveComponentsFromEntity(int _entityId)
    {
        // Make a copy as RemoveComponent will also remove from the original entityComponents List.
        IndexList listCopy(entityComponents[_entityId]);

        for (int _ind = 0; _ind < listCopy.size(); _ind++)
        {
            RemoveComponent(_ind);
        }

        // we SHOULD have removed components, but just in case.
        entityComponents[_entityId].clear();
    }

    void IComponent::RemoveComponent(int _componentIndex)
    {
        if (_componentIndex == -1)
            return;

        // Call internal first, in case it relies on 'entityIdComponentIndex' or similar.
        RemoveComponentInternal(_componentIndex);

        int entity = entityId[_componentIndex];
        if (entity != -1)
        {
            entityListDirty = true;
            IndexList& list = entityComponents[entity];
            list.remove_matches(_componentIndex);
        }

        ClearComponentAtIndex(_componentIndex);
    }


    void IComponent::GetEntitiesWithComponent(std::vector<int> &_outEntityList)
    {
        if (entityListDirty)
        {
            entityList.clear();
            for (auto iter = entityId.begin(); iter != entityId.end(); ++iter)
            {
                if (*iter != -1)
                {
                    entityList.push_back((*iter));
                }
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
            if (entityComponents[*iter][0] == -1)
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
            if (entityComponents[*iter][0] != -1)
            {
                iter = _entityListFilter.erase(iter);
            }
        }
    }

    void IComponent::ClearComponentAtIndex(int _componentIndex)
    {
        ClearComponentAtIndexInternal(_componentIndex);

        int entity = entityId[_componentIndex];
        if (entity != -1)
        {
            entityComponents[entity].clear();
        }
        entityId[_componentIndex] = -1;
    }

    void IComponent::ResizeEntityArray(int _newSize)
    {
        entityComponents.resize(_newSize, IndexList());
    }

    void IComponent::SetCapacity(int _newCapacity)
    {
        entityId.resize(_newCapacity, -1);

        SetCapacityInternal(_newCapacity);
    }

    int IComponent::GetNextInactiveComponent()
    {
        int capacity = (int)entityId.size();
        for (int i = 0; i < capacity; ++i)
        {
            int index = (nextIndex + i) % capacity;

            if (entityId[index] == -1)
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
