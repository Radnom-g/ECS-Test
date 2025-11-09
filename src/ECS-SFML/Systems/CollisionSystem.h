//
// Created by Radnom on 8/11/2025.
//

#pragma once
#include "Systems/ISystem.h"
#include "SFML/System/Vector2.hpp"
#include "Components/CollisionComponent.h"
#include "SFML/Graphics/CircleShape.hpp"

namespace ECS
{
    class EntityManager;
    class TreeComponent;
}

namespace ECS_SFML
{
    struct SFMLWorldContext;
    class TransformComponent;
    class BoundsComponent;
    class TransformSystem;

    class Collision
    {
        int entity = -1;
        ECS::ECollisionType type = ECS::Ignore;

    };

    class CollisionSystem: public ECS::ISystem
    {
    public:
        static constexpr std::string SystemName = "CollisionSystem";
        enum EMovementType
        {
            Teleport = 0,
            Collide = 1,
        };
        bool Initialise(SFMLWorldContext* _context);

        // bool CheckCollisions(int _entity, sf::Vector2f _at, Collision& _outCollisions );
        //
        // sf::FloatRect CalculateAABB(int _entity);

    protected:
        void ProcessInternal(float _deltaTick) override {}
        void RenderInternal(float _deltaTween) override {}

        inline const std::string& GetSystemName() override { return SystemName; }
        bool GetDoesProcessTick() override { return false; }
        bool GetDoesRenderTick() override { return false; }

        void GetProcessAfter(std::vector<std::string> &_outSystems) override {}
        void GetRenderAfter(std::vector<std::string> &_outSystems) override {}

        TransformSystem* transformSystem = nullptr;

        ECS::CollisionComponent* collisionComponent = nullptr;
        TransformComponent* transformComponent = nullptr;
        ECS::TreeComponent* treeComponent = nullptr;
    };
} // ECS_SFML