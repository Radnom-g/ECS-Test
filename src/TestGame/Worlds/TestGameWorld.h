//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "../../ECS-SFML/Managers/ResourceManager.h"
#include "../ECS-SFML/Systems/RenderSystem.h"
#include "../ECS-SFML/Systems/TransformSystem.h"
#include "Worlds/World.h"

namespace ECS_Game
{
    class TestGameWorld : public ECS::World
    {
    public:
        bool Initialise(ECS::WorldSettings* _worldSettings, sf::RenderWindow* _renderWindow);
    protected:
        bool Initialise(ECS::WorldSettings *_worldSettings) override;
    public:
        void InitialiseInternal() override;
        void ReinitialiseInternal() override;

        void UpdateInternal(float _deltaSeconds) override;
        void RenderInternal(float _deltaTween) override;

        void CreateWorldContext() override;


    protected:
        ECS_SFML::SFMLWorldContext* worldContextSFML = nullptr;
        sf::RenderWindow* renderWindow = nullptr;

        ECS_SFML::TransformSystem transformSystem;
        ECS_SFML::RenderSystem renderSystem;
        ECS_SFML::ResourceManager resourceManager;
    };
} // ECS_Game