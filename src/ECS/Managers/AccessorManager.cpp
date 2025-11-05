//
// Created by Radnom on 3/11/2025.
//

#include "AccessorManager.h"

namespace ECS
{
    AccessorManager::~AccessorManager()
    {
        // destroy the Controllers.
        for (std::vector<IComponentAccessor*>::iterator controllerIt = controllerList.begin(); controllerIt < controllerList.end(); ++controllerIt)
        {
            delete (*controllerIt);
        }
    }

    void AccessorManager::ProcessEndOfFrame()
    {
        for (std::vector<IComponentAccessor*>::iterator controllerIt = controllerList.begin(); controllerIt < controllerList.end(); ++controllerIt)
        {
            (*controllerIt)->ProcessEndOfFrame();
        }
    }

    void AccessorManager::EntityDestroyed(int _entityId)
    {
        for (std::vector<IComponentAccessor*>::iterator controllerIt = controllerList.begin(); controllerIt < controllerList.end(); ++controllerIt)
        {
            (*controllerIt)->EntityDestroyed(_entityId);
        }
    }

} // ECS