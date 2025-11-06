//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "Systems/ISystem.h"

#include "SFML/System/Vector2.hpp"
#include "Worlds/WorldContext.h"

namespace sf
{
    class RenderWindow;
}

namespace ECS
{
    class EntityManager;
}

namespace ECS_SFML
{
    struct SFMLWorldContext;
    class RenderSpriteComponent;
    class TransformComponent;
    class ResourceManager;

    class RenderSystem : public ECS::ISystem
    {
    public:
        bool Initialise(SFMLWorldContext* _context);

    protected:
        void ProcessInternal(float _deltaTick) override {}
        void RenderInternal(float _deltaTween) override;

        inline const char* GetSystemName() override { return "RenderSystem"; }
        bool GetDoesProcessTick() override { return false; }
        bool GetDoesRenderTick() override { return true; }

        ResourceManager *resourceManager = nullptr;
        sf::RenderWindow* renderWindow = nullptr;

        TransformComponent* transformComponent = nullptr;
        RenderSpriteComponent* renderSpriteComponent = nullptr;
    };
} // ECS_SFML