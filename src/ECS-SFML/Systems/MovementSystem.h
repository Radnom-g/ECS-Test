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
    class VelocityComponent;
    class ScreenWrapComponent;
    class TransformSystem;

    class MovementSystem : public ECS::ISystem
    {
    public:
        enum EMovementType
        {
            Teleport = 0,
            Collide = 1,
        };
    public:
        bool Initialise(SFMLWorldContext* _context);

        bool MoveEntity(int _entity, sf::Vector2f _movement, EMovementType _movementType);
        bool SetEntityLocation(int _entity, sf::Vector2f _newPos, EMovementType _movementType);

    protected:
        void ProcessInternal(float _deltaTick) override;
        void RenderInternal(float _deltaTween) override {}

        inline const char* GetSystemName() override { return "MovementSystem"; }
        bool GetDoesProcessTick() override { return true; }
        bool GetDoesRenderTick() override { return false; }

        TransformSystem* transformSystem = nullptr;

        TransformComponent* transformComponent = nullptr;
        VelocityComponent* velocityComponent = nullptr;
        ScreenWrapComponent* screenWrapComponent = nullptr;

        // for world wrap
        sf::Vector2f worldSize = sf::Vector2f(32.0f, 32.0f);

    };
} // ECS_SFML