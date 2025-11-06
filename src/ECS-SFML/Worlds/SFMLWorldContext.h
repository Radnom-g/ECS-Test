//
// Created by Radnom on 6/11/2025.
//

#pragma once
#include "Worlds/WorldContext.h"

namespace sf
{
    class RenderWindow;
}

namespace ECS_SFML
{
    class ResourceManager;
    class TransformSystem;
    class RenderSystem;

    struct SFMLWorldContext : public ECS::WorldContext
    {
        TransformSystem* transformSystem = nullptr;
        RenderSystem* renderSystem = nullptr;
        ResourceManager* resourceManager = nullptr;
        sf::RenderWindow* renderWindow = nullptr;
    };
} // ECS_SFML