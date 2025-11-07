//
// Created by Radnom on 7/11/2025.
//

#pragma once
#include "Systems/ISystem.h"
#include "../ECS-SFML/Components/Transform.h"

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
    class TransformComponent;
    class TransformSystem;

    class MovementSystem : public ECS::ISystem
    {
    public:
        bool Initialise(SFMLWorldContext* _context);

    protected:
        void ProcessInternal(float _deltaTick) override;
        void RenderInternal(float _deltaTween) override {}

        inline const char* GetSystemName() override { return "MovementSystem"; }
        bool GetDoesProcessTick() override { return true; }
        bool GetDoesRenderTick() override { return false; }


        sf::RenderWindow* renderWindow = nullptr; // used for world wrap
        TransformSystem* transformSystem = nullptr;

        TransformComponent* transformComponent = nullptr;

    };
} // ECS_SFML