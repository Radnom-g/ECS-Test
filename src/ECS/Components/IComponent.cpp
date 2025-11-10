//
// Created by Radnom on 5/11/2025.
//

#include "IComponent.h"

#include <algorithm>
#include <cassert>

#include "Worlds/WorldContext.h"

namespace ECS
{
    bool IComponent::InitialiseComponent(WorldContext *context, int _componentId, int _initialCapacity, int _maxCapacity)
    {
        if (isInitialised)
        {
            assert(false && "Component Initialised Twice!");
            return false;
        }

        entityManager = context->entityManager;

        componentId = _componentId;
        isInitialised = true;

        maxCapacity = _maxCapacity;
        entityId.reserve(_initialCapacity);

        return InitialiseInternal(context, _initialCapacity, _maxCapacity);
    }

    int IComponent::AddComponent(const Entity &_entity)
    {
        if (IsUniquePerEntity())
        {
            // If we can only have one, and we already have one, return it
            if (HasComponent(_entity.index))
                return GetComponentIndex(_entity.index);
        }

        int componentIndex = GetNextInactiveComponent();
        if (componentIndex != -1)
        {
            entityId[componentIndex] = _entity.index;
            entityComponents[_entity.index].push_back(componentIndex);
            AddComponentInternal(_entity.index, componentIndex);
            nextIndex++;
            entityListDirty = true;

            for (auto& del : componentAddedDelegates)
            {
                del(_entity, componentIndex);
            }
        }

        return componentIndex;
    }

    void IComponent::RemoveComponentsFromEntity(const Entity &_entity)
    {
        // Make a copy as RemoveComponent will also remove from the original entityComponents List.
        IndexList listCopy(entityComponents[_entity.index]);

        for (int _ind = 0; _ind < listCopy.size(); _ind++)
        {
            RemoveComponentByIndex(_entity, _ind);
        }

        // we SHOULD have removed components, but just in case.
        entityComponents[_entity.index].clear();
    }

    void IComponent::RemoveComponentByIndex(int _componentIndex)
    {
        int entityIndex = entityId[_componentIndex];
        Entity entity = entityManager->GetEntity(entityIndex);
        if (entity.IsValid())
        {
            RemoveComponentByIndex(entity, _componentIndex);
        }
    }

    void IComponent::RemoveComponentByIndex(const Entity &_entity, int _componentIndex)
    {
        if (_componentIndex == -1)
            return;

        if (!_entity.IsValid())
            return;

        // Call delegates first so they have time to get any data out that they might need
        // for their own cleanup.
        for (auto& del : componentRemovedDelegates)
        {
            del(_entity, _componentIndex);
        }

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
            else
            {
                ++iter;
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

    void IComponent::RegisterOnAddComponent(ComponentAddedDelegate _delegate)
    {
        componentAddedDelegates.push_back(_delegate);
    }

    void IComponent::RegisterOnRemoveComponent(ComponentRemovedDelegate _delegate)
    {
        componentRemovedDelegates.push_back(_delegate);
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
