//
// Created by Radnom on 6/11/2025.
//

#pragma once

#include <functional>
#include <vector>

#include "Components/ComponentSettings.h"

namespace ECS
{
    class IComponent;

    struct WorldSettings
    {
        virtual ~WorldSettings() {};

        int entityCapacityInitial = 1000;
        int entityCapacityMax = 100000;

        unsigned int worldWidth = 1920;
        unsigned int worldHeight = 1080;

        unsigned int tileGridWidth = 16;
        unsigned int tileGridHeight = 16;

        // Info about what Components are in this world, how to construct them, and how many component instances
        // they support.
        std::vector<ComponentSettings> ComponentSettings;

        //virtual WorldSettings* Clone() { WorldSettings* clone = new WorldSettings(*this); return clone; }
    };
} // ECS