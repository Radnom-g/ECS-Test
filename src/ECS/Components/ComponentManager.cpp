//
// Created by Radnom on 5/11/2025.
//

#include "ComponentManager.h"

#include <assert.h>
#include <cstring>

#include "Worlds/WorldContext.h"

namespace ECS
{
    ComponentManager::~ComponentManager()
    {
        for (auto iter : components)
        {
            delete iter;
        }
        components.clear();
    }

    void ComponentManager::OnEntityDestroyed(int _entityId)
    {
        for (auto iter : components)
        {
            iter->RemoveComponentsFromEntity(_entityId);
        }
    }

    void ComponentManager::InitialiseComponents(WorldContext* _context)
    {
        for (int i = 0; i < components.size(); i++)
        {
            IComponent* component = components[i];
            int initialCapacity = 0;
            int maxCapacity = 0;
            for (const auto& settingsIter : _context->worldSettings->ComponentSettings )
            {
                if (settingsIter.name == component->GetName())
                {
                    initialCapacity = settingsIter.initialCapacity;
                    maxCapacity = settingsIter.maxCapacity;
                    break;
                }
            }

            component->InitialiseComponent(_context, i, initialCapacity, maxCapacity);
        }
    }

    void ComponentManager::RegisterComponent(ComponentSettings* _componentSettings, IComponent *_component)
    {
        assert(_component && "Don't Register null component");

        for (auto iter : components)
        {
            assert (typeid(iter) == typeid(_component) && "Already registered!");
        }

        _componentSettings->name = _component->GetName();

        components.push_back(_component);
    }

    void ComponentManager::ResizeEntityArray(int _newSize)
    {
        for (auto iter : components)
        {
            iter->ResizeEntityArray(_newSize);
        }
    }
} // ECS