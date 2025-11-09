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
    class MovementSystem;
    class CollisionSystem;

    struct SFMLWorldContext : public ECS::WorldContext
    {
        TransformSystem* transformSystem = nullptr;
        CollisionSystem* collisionSystem = nullptr;
        RenderSystem* renderSystem = nullptr;
        MovementSystem* movementSystem = nullptr;
        ResourceManager* resourceManager = nullptr;
        sf::RenderWindow* renderWindow = nullptr;
    };
} // ECS_SFML