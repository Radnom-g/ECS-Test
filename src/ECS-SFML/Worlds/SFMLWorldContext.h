//
// Created by Radnom on 6/11/2025.
//

#pragma once
#include "Worlds/WorldContext.h"

namespace ECS_SFML
{
    class ResourceManager;

    struct SFMLWorldContext : public ECS::WorldContext
    {
        ResourceManager* resourceManager = nullptr;
        sf::RenderWindow* renderWindow = nullptr;
    };
} // ECS_SFML