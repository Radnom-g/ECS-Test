//
// Created by Radnom on 3/11/2025.
//

#pragma once

#include <map>
#include <vector>

#include "Accessors/ComponentAccessor.h"
#include "Accessors/IComponentAccessor.h"

namespace ECS
{
    // Each Component type has its own Controller to handle creating/destroying/processing that component Type.
    // The Controller Manager is where those Controllers can be initialised or accessed.
    class AccessorManager
    {
    public:
        ~AccessorManager();

        void ProcessEndOfFrame();
        void EntityDestroyed(int _entityId);

    private:
        std::map<const char*, int> controllerIdMap;
        std::map<int, IComponentAccessor*> controllerMap;
        std::vector< IComponentAccessor*> controllerList;

    public:
        // Implementation:

        // This is for Component Controllers that aren't the default generic one.
        // Initialise them then register them.
        void RegisterComponentAccessor(IComponentAccessor* _ComponentAccessor)
        {
            assert(_ComponentAccessor && "RegisterComponentAccessor: cannot register null controller");
            assert(_ComponentAccessor->IsInitialised() && "RegisterComponentAccessor: cannot register non-initialised controller.");

            controllerList.push_back(_ComponentAccessor);
            int id = (int)controllerIdMap.size()+1;
            const char* componentName = _ComponentAccessor->GetComponentName();
            controllerIdMap[componentName] = id;
            controllerMap[id] = _ComponentAccessor;

            _ComponentAccessor->SetControllerId(id);

        }

        // This is to quickly create and register the majority of Component Controllers.
        template<class TComponent>
        ComponentAccessor<TComponent> *CreateComponentAccessor()
        {
            static_assert(std::is_base_of<AbstractComponent, TComponent>::value, "T must derive from AbstractComponent");

            // TODO: check controllers keys for duplicate name
            const char* componentName = TComponent::GetName();

            ComponentAccessor<TComponent>* controllerT = new ComponentAccessor<TComponent>();
            controllerList.push_back(controllerT);

            int id = (int)controllerIdMap.size()+1;
            controllerIdMap[componentName] = id;
            controllerMap[id] = controllerT;

            controllerT->Initialise(1, 100);
            controllerT->SetControllerId(id);

            return controllerT;
        }

        template<class TComponent>
        ComponentAccessor<TComponent>* GetComponentAccessor()
        {
            static_assert(std::is_base_of<AbstractComponent, TComponent>::value, "T must derive from AbstractComponent");

            // TODO: check controllers keys that name exists
            const char* componentName = TComponent::GetName();

            auto idIter = controllerIdMap.find(componentName);
            if (idIter != controllerIdMap.end())
            {
                auto mapIter = controllerMap.find(idIter->second);
                if (mapIter != controllerMap.end())
                {
                    IComponentAccessor* icc = mapIter->second;
                    return static_cast<ComponentAccessor<TComponent>*>(icc);
                }
            }

            return nullptr;
        }

        template<class T>
        T* AddComponent(int _entityID)
        {
            return GetComponentAccessor<T>()->AddComponent(_entityID);
        }

        template<class T>
        T* GetComponent(int _entityID)
        {
            return GetComponentAccessor<T>()->GetComponent(_entityID);
        }

        template<class T>
        ComponentAccessor<T>* GetComponentAccessor(int _controllerId)
        {
            return controllerMap[_controllerId];
        }
    };
} // ECS
