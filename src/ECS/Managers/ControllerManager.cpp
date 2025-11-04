//
// Created by Radnom on 3/11/2025.
//

#include "ControllerManager.h"

namespace ECS
{
    void ControllerManager::ProcessEndOfFrame()
    {
        for (std::vector<IComponentController*>::iterator controllerIt = controllerList.begin(); controllerIt < controllerList.end(); ++controllerIt)
        {
            (*controllerIt)->ProcessEndOfFrame();
        }
    }

    void ControllerManager::EntityDestroyed(int _entityId)
    {
        for (std::vector<IComponentController*>::iterator controllerIt = controllerList.begin(); controllerIt < controllerList.end(); ++controllerIt)
        {
            (*controllerIt)->EntityDestroyed(_entityId);
        }
    }

} // ECS