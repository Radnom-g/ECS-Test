//
// Created by Radnom on 5/11/2025.
//

#pragma once
#include "WorldSettings.h"

namespace ECS
{
    class World;
    class EntityManager;
    class ComponentManager;
    class SystemManager;

    struct WorldContext
    {
        virtual ~WorldContext() = default;

        World* world = nullptr;
        EntityManager* entityManager = nullptr;
        ComponentManager* componentManager = nullptr;
        WorldSettings* worldSettings = nullptr;
        SystemManager* systemManager = nullptr;
    };
} // ECS