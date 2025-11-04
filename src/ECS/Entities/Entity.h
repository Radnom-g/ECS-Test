//
// Created by Radnom on 3/11/2025.
//

#pragma once

#include <string>

namespace ECS
{
    // An Entity is essentially a unique ID.
    // They also have an 'active' flag which marks if the Entity is 'alive' in the World, or 'dead' and ready
    // to be replaced with a new one.
    // In my case, they also have a name so that it's easier to debug.
    class Entity
    {
    public:
        Entity()
        {
            Reset();
        }

        std::string name;
        int id = -1;
        bool active = false;

        void Reset()
        {
            name = "Invalid";
            id = -1;
            active = false;
        }
    };
} // ECS
