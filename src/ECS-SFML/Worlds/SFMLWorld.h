//
// Created by Radnom on 7/11/2025.
//

#pragma once
#include "Worlds/World.h"
#include "../ECS-SFML/Systems/RenderSystem.h"
#include "../ECS-SFML/Systems/TransformSystem.h"
#include "../../ECS-SFML/Managers/ResourceManager.h"
#include "../Systems/MovementSystem.h"

namespace sf
{
    class RenderWindow;
}

namespace ECS_SFML
{
    struct SFMLWorldContext;

    class SFMLWorld : public ECS::World
    {
    public:
        bool Initialise(ECS::WorldSettings* _worldSettings, sf::RenderWindow* _renderWindow);
    protected:
        bool Initialise(ECS::WorldSettings *_worldSettings) override;
        void InitialiseInternal() override;

        void CreateWorldContext() override;
        void CreateAndRegisterComponentsInternal() override;
        void InitialiseComponents() override;
        void ReinitialiseInternal() override;
        void UpdateInternal(float _deltaSeconds) override;
        void RenderInternal(float _deltaTween) override;

    protected:
        ECS_SFML::SFMLWorldContext* worldContextSFML = nullptr;
        sf::RenderWindow* renderWindow = nullptr;

        ECS_SFML::TransformSystem transformSystem;
        ECS_SFML::RenderSystem renderSystem;
        ECS_SFML::MovementSystem movementSystem;
        ECS_SFML::ResourceManager resourceManager;
    };
} // ECS_SFML