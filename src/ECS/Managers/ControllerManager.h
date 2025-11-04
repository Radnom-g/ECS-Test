//
// Created by Radnom on 3/11/2025.
//

#pragma once

#include <map>
#include <vector>

#include "Controllers/ComponentController.h"
#include "Controllers/IComponentController.h"
namespace ECS
{
    // Each Component type has its own Controller to handle creating/destroying/processing that component Type.
    // The Controller Manager is where those Controllers can be initialised or accessed.
    class ControllerManager
    {
    public:
        ~ControllerManager() = default;

        void ProcessEndOfFrame();
        void EntityDestroyed(int _entityId);

    private:
        std::map<const char*, int> controllerIdMap;
        std::map<int, IComponentController*> controllerMap;
        std::vector< IComponentController*> controllerList;

    public:
        // Implementation:
        template<class TComponent>
        ComponentController<TComponent> *RegisterComponentController()
        {
            static_assert(std::is_base_of<AbstractComponent, TComponent>::value, "T must derive from AbstractComponent");

            // TODO: check controllers keys for duplicate name
            const char* componentName = TComponent::GetName();

            ComponentController<TComponent>* controllerT = new ComponentController<TComponent>();
            controllerList.push_back(controllerT);

            int id = (int)controllerIdMap.size()+1;
            controllerIdMap[componentName] = id;
            controllerMap[id] = controllerT;

            controllerT->Initialise(id, 1, 100);

            return controllerT;
        }

        template<class TComponent>
        ComponentController<TComponent>* GetComponentController()
        {
            static_assert(std::is_base_of<AbstractComponent, TComponent>::value, "T must derive from AbstractComponent");

            // TODO: check controllers keys that name exists
            const char* componentName = TComponent::GetName();

            int controllerId = controllerIdMap[componentName];
            if (controllerId == 0)
            {
                RegisterComponentController<TComponent>();
                controllerId = controllerIdMap[componentName];
            }
            IComponentController* icc = controllerMap[controllerId];
            return static_cast<ComponentController<TComponent>*>(icc);
        }

        template<class T>
        T* AddComponent(int _entityID)
        {
            return GetComponentController<T>()->AddComponent(_entityID);
        }

        template<class T>
        T* GetComponent(int _entityID)
        {
            return GetComponentController<T>()->GetComponent(_entityID);
        }

        template<class T>
        ComponentController<T>* GetComponentController(int _controllerId)
        {
            return controllerMap[_controllerId];
        }
    };
} // ECS
