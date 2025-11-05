//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "../../ECS-SFML/Managers/ResourceManager.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "../ECS-SFML/Systems/RenderSystem.h"
#include "Worlds/World.h"

namespace ECS_Game
{
    class TestGameWorld : public ECS::World
    {
    protected:
        // Protected so that it can't be called by other classes.
        bool Initialise() override;
    public:

        bool Initialise(sf::RenderWindow& _window);
        void Reinitialise() override;
        void Update(float _deltaSeconds) override;
        void Render(float _deltaTween) override;


    protected:
        ECS_SFML::RenderSystem renderSystem;
        ECS_SFML::ResourceManager resourceManager;
    };
} // ECS_Game