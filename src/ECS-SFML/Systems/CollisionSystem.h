//
// Created by Radnom on 8/11/2025.
//

#pragma once
#include <deque>

#include "RenderSystem.h"
#include "Systems/ISystem.h"
#include "SFML/System/Vector2.hpp"
#include "Components/CollisionComponent.h"
#include "../External/Collision.h"
#include "SFML/Graphics/CircleShape.hpp"

namespace Grid
{
    struct UGrid;
}

namespace sf
{
    class RenderWindow;
}

namespace Collision
{
    struct OrientedBoundingBox;
}

namespace ECS
{
    class EntityManager;
    class TreeComponent;
}

namespace ECS_SFML
{
    class SpriteComponent;
    class ResourceManager;
    struct SFMLWorldContext;
    class TransformComponent;
    class BoundsComponent;
    class TransformSystem;

    struct CollisionResult
    {
        int entity = -1;
        ECS::ECollision::ResponseType result = ECS::ECollision::ResponseType::Ignore;
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

        ~CollisionSystem() override;

        bool Initialise(SFMLWorldContext* _context);

        // bool CheckCollisions(int _entity, sf::Vector2f _at, Collision& _outCollisions );
        //
        // sf::FloatRect CalculateAABB(int _entity);

        bool CheckCollisions(int _entityCheck, sf::Vector2f _at, std::vector<CollisionResult> _outCollisions);

        Grid::UGrid* GetUniformGrid() { return uniformGrid; }

        void OnCollisionComponentAdded(ECS::Entity _entity, int _component);
        void OnCollisionComponentRemoved(ECS::Entity _entity, int component);

    protected:
        void ProcessInternal(float _deltaTick) override;
        void RenderInternal(float _deltaTween) override; // for debug

        void DebugProcess();

        inline const std::string& GetSystemName() override { return SystemName; }
        bool GetDoesProcessTick() override { return true; }
        bool GetDoesRenderTick() override { return true; }

        void GetProcessAfter(std::vector<std::string> &_outSystems) override {}
        void GetRenderAfter(std::vector<std::string> &_outSystems) override; //for debug

        void DebugDrawGrid(Grid::UGrid* _grid);

        sf::RenderWindow* renderWindow = nullptr; // for debugging

        TransformSystem* transformSystem = nullptr;
        RenderSystem* renderSystem = nullptr; // for debugging

        ResourceManager* resourceManager = nullptr;
        ECS::CollisionComponent* collisionComponent = nullptr;
        TransformComponent* transformComponent = nullptr;
        ECS::TreeComponent* treeComponent = nullptr;
        SpriteComponent* spriteComponent = nullptr;

        std::deque<Collision::OrientedBoundingBox> debugBoundsBoxesToDraw;
        std::deque<Collision::OrientedBoundingBox> debugCollisionBoxesToDraw;

        float averageEntityRadius = 20.0f;

        // This is just a list when a CollisionComponent is added, to make sure that we add it to the Grid.
        std::vector<int> entitiesPendingCacheAddition;

        // Per Collision Component:
        std::vector<sf::Vector2f> cachedGridPositions;

        // The grid used for early+cheap collision checks.
        Grid::UGrid* uniformGrid = nullptr;

        bool debugCollisions = false;
    };
} // ECS_SFML