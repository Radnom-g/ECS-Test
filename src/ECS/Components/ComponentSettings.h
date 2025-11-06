//
// Created by Radnom on 6/11/2025.
//

#pragma once
#include <string>
#include <functional>

namespace ECS
{
    class IComponent;

    struct ComponentSettings
    {
        std::string name = "None"; // gets set AFTER construction
        int initialCapacity = 1;
        int maxCapacity = 100;
        std::function<IComponent*()> Constructor;
    };
}
