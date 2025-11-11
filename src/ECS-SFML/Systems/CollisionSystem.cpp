//
// Created by Radnom on 8/11/2025.
//

#include "CollisionSystem.h"

#include <windows.h>
#include <debugapi.h>

#include "RenderSystem.h"
#include "TransformSystem.h"
#include "../../External/Collision.h"
#include "../../External/UGrid.h"
#include "../ECS-SFML/Worlds/SFMLWorldContext.h"
#include "Components/ComponentManager.h"
#include "Components/TreeComponent.h"
#include "../ECS-SFML/Components/TransformComponent.h"
#include "../ECS-SFML/Components/SpriteComponent.h"
#include "../Managers/ResourceManager.h"

namespace ECS_SFML
{
    CollisionSystem::~CollisionSystem()
    {
        if (uniformGrid != nullptr)
        {
            delete uniformGrid;
            uniformGrid = nullptr;
        }
    }

    bool CollisionSystem::Initialise(SFMLWorldContext *_context)
    {
        renderWindow = _context->renderWindow;

        transformComponent = _context->componentManager->GetComponent<TransformComponent>();
        collisionComponent = _context->componentManager->GetComponent<ECS::CollisionComponent>();
        treeComponent = _context->componentManager->GetComponent<ECS::TreeComponent>();
        spriteComponent = _context->componentManager->GetComponent<SpriteComponent>();
        transformSystem = _context->transformSystem;
        renderSystem = _context->renderSystem;

        resourceManager = _context->resourceManager;

        collisionComponent->RegisterOnAddComponent(
            [this](ECS::Entity _entity, int _compInd) { OnCollisionComponentAdded(_entity, _compInd); } );
        collisionComponent->RegisterOnRemoveComponent(
            [this](ECS::Entity _entity, int _compInd) { OnCollisionComponentRemoved(_entity, _compInd); } );


        float tileWidth = static_cast<float>(_context->worldSettings->tileGridWidth);
        float tileHeight = static_cast<float>(_context->worldSettings->tileGridHeight);
        float gridWidth = static_cast<float>(_context->worldSettings->worldWidth);
        float gridHeight = static_cast<float>(_context->worldSettings->worldHeight);

        // Pythagoras diagonal - max colliding distance for a circle to overlap a grid square, plus one
        float tileWidthSq = tileWidth * tileWidth;
        averageEntityRadius = 16.1f;// sqrt (tileWidthSq + tileWidthSq) + 1.0f;

        sf::Vector2f gridOffset(-tileWidth/2, -tileHeight/2);
        gridHeight += tileHeight;
        gridWidth += tileWidth;

        uniformGrid = Grid::ugrid_create(averageEntityRadius, tileWidth, tileHeight, gridOffset.x, gridOffset.y, gridWidth, gridHeight);

        isInitialised = true;


        return true;
    }

    bool CollisionSystem::CheckCollisions(int _entityCheck, sf::Vector2f _at, std::vector<CollisionResult> _outCollisions)
    {
        int _entityA = _entityCheck; // just to make naming less confusing

        int collisionCompIndA = collisionComponent->GetComponentIndex(_entityA);
        int spriteCompIndA = spriteComponent->GetComponentIndex(_entityA);

        if (collisionCompIndA == -1 || spriteCompIndA == -1)
            return false; // currently can't collide without a sprite

        ECS::ECollision::ResponseType collisionTypeA = collisionComponent->GetCollisionResponseType(collisionCompIndA);

        // easy early out..
        if (collisionTypeA == ECS::ECollision::ResponseType::Ignore)
            return false;

        SmallList<int> checkList = Grid::ugrid_query(uniformGrid, _at.x, _at.y, averageEntityRadius, _entityA);

        if (checkList.size() > 0)
        {
            Transform CheckTransform = transformSystem->GetEntityWorldTransform(_entityA);
            CheckTransform.setPosition(_at);
            const sf::Transform& transformA = CheckTransform.getTransform();

            int texIdA = spriteComponent->GetTextureIndex(spriteCompIndA);
            sf::Vector2f offsetA = spriteComponent->GetRelativeOrigin(spriteCompIndA);

            Collision::CollisionSprite collisionSpriteA(resourceManager->GetTexture(texIdA), offsetA, &transformA, nullptr, resourceManager->GetTextureMask(texIdA));

            for (int i = 0; i < checkList.size(); i++)
            {
                int entityB = checkList[i];
                int collisionCompIndB = collisionComponent->GetComponentIndex(entityB);

                // We can see if we WOULD collide with them before we actually do the overlap.
                ECS::ECollision::ResponseType collResult = collisionComponent->GetOverlapResult(collisionCompIndA, collisionCompIndB);
                if (collResult == ECS::ECollision::ResponseType::Ignore)
                {
                    continue;
                }

                int spriteCompIndB = spriteComponent->GetComponentIndex(entityB);
                int texIdB = spriteComponent->GetTextureIndex(spriteCompIndB);
                const Transform& otherTransform = transformSystem->GetEntityWorldTransform(entityB);

                sf::Vector2f dist = otherTransform.getPosition() - CheckTransform.getPosition();
                if (dist.lengthSquared() < 32.0f)
                {
                    CollisionResult coll;
                    coll.entity = entityB;
                    coll.result = collResult;
                    _outCollisions.push_back(coll);
                    continue;
                    // SKIPPING the bounding box test
                }

                const sf::Transform& otherTransformA = otherTransform.getTransform();

                sf::Vector2f offsetB = spriteComponent->GetRelativeOrigin(spriteCompIndB);
                Collision::CollisionSprite collisionSpriteB(resourceManager->GetTexture(texIdB), offsetB, &otherTransformA, nullptr, resourceManager->GetTextureMask(texIdB));
                bool bCheck = Collision::boundingBoxTest(collisionSpriteA, collisionSpriteB);

                if (bCheck)
                {
                    // TODO: an actual sprite overlap

                    CollisionResult coll;
                    coll.entity = entityB;
                    coll.result = collResult;
                    _outCollisions.push_back(coll);

                    if (debugCollisions)
                    {
                        if (_outCollisions.size() == 1) // draw the first collision as debug
                        {
                            Collision::OrientedBoundingBox OBB1{ collisionSpriteA };
                            Collision::OrientedBoundingBox OBB2{ collisionSpriteB };
                            debugCollisionBoxesToDraw.push_front(OBB1);
                            debugCollisionBoxesToDraw.push_front(OBB2);
                        }
                    }
                }
            }
        }

        return _outCollisions.size() > 0;
    }

    void CollisionSystem::OnCollisionComponentAdded(ECS::Entity _entity, int _component)
    {
        entitiesPendingCacheAddition.push_back(_entity.index);
    }

    void CollisionSystem::OnCollisionComponentRemoved(ECS::Entity _entity, int component)
    {
        if (component < cachedGridPositions.size())
        {
            sf::Vector2f& cachedGridPos = cachedGridPositions[component];
            Grid::ugrid_remove(uniformGrid, _entity.index, cachedGridPos.x, cachedGridPos.y);
        }
    }

    void CollisionSystem::ProcessInternal(float _deltaTick)
    {
        //debugBoxesToDraw.clear();

        if (cachedGridPositions.size() < collisionComponent->GetArraySize())
        {
            cachedGridPositions.resize(collisionComponent->GetArraySize());
        }

        if (debugCollisions)
        {
            DebugProcess();
        }

        // Add pending entities to the grid.
        for (int i = entitiesPendingCacheAddition.size()-1; i >= 0; i--)
        {
            int newEnt = entitiesPendingCacheAddition[i];
            if (transformSystem->HasCachedWorldTransform(newEnt))
            {
                const Transform& wTransform = transformSystem->GetEntityWorldTransform(newEnt);
                const sf::Vector2f& pos = wTransform.getPosition();
                Grid::ugrid_insert( uniformGrid, newEnt, pos.x, pos.y);
                int collComp = collisionComponent->GetComponentIndex(newEnt);


                if (debugCollisions)
                {
                    std::stringstream strDebug;
                    strDebug << "Entity #" << newEnt << " added to Grid at pos <";
                    strDebug << pos.x << ", " << pos.y << ">";
                    OutputDebugString(strDebug.str().c_str());
                }

                cachedGridPositions[collComp] = pos;
                entitiesPendingCacheAddition.pop_back();
            }
        }

        debugBoundsBoxesToDraw.clear();
        std::vector<int> collisionEntities;
        collisionComponent->GetEntitiesWithComponent(collisionEntities);
        for (int i = 0; i < collisionEntities.size(); i++)
        {
            int spriteCompInd = spriteComponent->GetComponentIndex(collisionEntities[i]);
            if (spriteCompInd == -1)
                continue; // Only using Sprites for collision at the moment.

            int texId = spriteComponent->GetTextureIndex(spriteCompInd);

            const Transform& wTransform = renderSystem->GetWorldSpriteTransform(spriteCompInd);
            const sf::Transform& transform = wTransform.getTransform();

            // Update grid position:
            if (transformSystem->HasEntityMovedThisFrame(collisionEntities[i]))
            {
                int collInd = collisionComponent->GetComponentIndex(collisionEntities[i]);
                const sf::Vector2f& oldGridPos = cachedGridPositions[collInd];
                const sf::Vector2f& newGridPos = wTransform.getPosition();
                Grid::ugrid_move(uniformGrid, collisionEntities[i], oldGridPos.x, oldGridPos.y, newGridPos.x, newGridPos.y );
                cachedGridPositions[collInd] = newGridPos;
            }


            if (debugCollisions)
            {
                sf::Vector2f offset = spriteComponent->GetRelativeOrigin(spriteCompInd);
                Collision::CollisionSprite collisionSprite(resourceManager->GetTexture(texId), offset, &transform, nullptr, resourceManager->GetTextureMask(texId));
                Collision::OrientedBoundingBox OBB1{ collisionSprite };
                debugBoundsBoxesToDraw.push_back(OBB1);

                float x,y,w,h;

                Grid::ugrid_get_rect(uniformGrid, wTransform.getPosition().x, wTransform.getPosition().y, x,y,w,h);

                Collision::OrientedBoundingBox OBB2(x,y,w,h);
                debugBoundsBoxesToDraw.push_back(OBB2);
            }
        }

        if (uniformGrid)
        {
            Grid::ugrid_optimize(uniformGrid);
        }
    }

    void CollisionSystem::RenderInternal(float _deltaTween)
    {
        if (!debugCollisions)
            return;

        if (uniformGrid)
        {
            DebugDrawGrid(uniformGrid);
        }

        for (int i = 0; i < debugBoundsBoxesToDraw.size(); i++)
        {
            sf::Color col = sf::Color(50, 180, 180, 255);
            if (i%2 == 0)
                col = sf::Color(50, 180, 50, 255);

            std::array lineBox =
            {
                sf::Vertex{debugBoundsBoxesToDraw[i].points[0], col},
                sf::Vertex{debugBoundsBoxesToDraw[i].points[1], col},

                sf::Vertex{debugBoundsBoxesToDraw[i].points[1], col},
                sf::Vertex{debugBoundsBoxesToDraw[i].points[2], col},

                sf::Vertex{debugBoundsBoxesToDraw[i].points[2], col},
                sf::Vertex{debugBoundsBoxesToDraw[i].points[3], col},

                sf::Vertex{debugBoundsBoxesToDraw[i].points[3], col},
                sf::Vertex{debugBoundsBoxesToDraw[i].points[0], col},
            };
            renderWindow->draw(lineBox.data(), lineBox.size(), sf::PrimitiveType::Lines);
        }

        for (int i = 0; i < cachedGridPositions.size(); i++)
        {
            if ( !collisionComponent->IsComponentActive(i) )
                continue;
            sf::CircleShape posCircle(4.0f, 6);
            posCircle.setPosition(cachedGridPositions[i]);
            posCircle.setOutlineThickness(2);
            posCircle.setFillColor(sf::Color::Transparent);
            posCircle.setOutlineColor(sf::Color(50, 180, 50, 255));
            renderWindow->draw(posCircle);
        }

        for (int i = 0; i < debugCollisionBoxesToDraw.size(); i++)
        {
            sf::Color col = sf::Color::Yellow;
            if (i%2 == 0)
                col = sf::Color::Red;

            std::array lineBox =
            {
                sf::Vertex{debugCollisionBoxesToDraw[i].points[0], col},
                sf::Vertex{debugCollisionBoxesToDraw[i].points[1], col},

                sf::Vertex{debugCollisionBoxesToDraw[i].points[1], col},
                sf::Vertex{debugCollisionBoxesToDraw[i].points[2], col},

                sf::Vertex{debugCollisionBoxesToDraw[i].points[2], col},
                sf::Vertex{debugCollisionBoxesToDraw[i].points[3], col},

                sf::Vertex{debugCollisionBoxesToDraw[i].points[3], col},
                sf::Vertex{debugCollisionBoxesToDraw[i].points[0], col},
            };
            renderWindow->draw(lineBox.data(), lineBox.size(), sf::PrimitiveType::Lines);
        }


        // draw a big crosshair at 256, 96
        // std::array line =
        // {
        //     sf::Vertex{sf::Vector2f(0, 96), sf::Color(255,0,255, 55)},
        //     sf::Vertex{sf::Vector2f(999, 96), sf::Color(255,0,255, 55)},
        // };
        // renderWindow->draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        // line =
        // {
        //     sf::Vertex{sf::Vector2f(256, 0), sf::Color(255,0,255, 55)},
        //     sf::Vertex{sf::Vector2f(256, 999), sf::Color(255,0,255, 55)},
        // };
        // renderWindow->draw(line.data(), line.size(), sf::PrimitiveType::Lines);
    }

    void CollisionSystem::DebugProcess()
    {


        while (debugCollisionBoxesToDraw.size() > 20)
        {
            debugCollisionBoxesToDraw.pop_back();
        }


        SmallList<int> grid_x_things;
        SmallList<int> grid_y_things;
        SmallList<int> grid_entities;
        Grid::ugrid_get_contents(uniformGrid, grid_entities, grid_x_things, grid_y_things);

        std::stringstream strDebug;
        for (int i = 0; i < grid_entities.size(); ++i)
        {
            strDebug << "Entity #" << grid_entities[i] << " @ Grid <";
            strDebug << grid_x_things[i] << ", " << grid_y_things[i] << "> --- ";
        }
        OutputDebugString(strDebug.str().c_str());

        // TODO:
        // if (collisionComponent->HasComponent(collidingEntities[i]))
        // {
        //     Grid::ugrid_insert(uniformGrid, collidingEntities[i], _worldTransform.getPosition().x, _worldTransform.getPosition().y);
        // }

        // if (collisionComponent->HasComponent(entityId))
        // {
        //     Grid::ugrid_move(uniformGrid, transformComponent->entityId[_transformIndex], lastPos.x,lastPos.y, newPos.x, newPos.y);
        // }
    }

    void CollisionSystem::GetRenderAfter(std::vector<std::string> &_outSystems)
    {
        _outSystems.push_back(std::string(renderSystem->SystemName));

    }

    void CollisionSystem::DebugDrawGrid(Grid::UGrid *_grid)
    {
        for (int xind = 0; xind < _grid->num_cols; ++xind)
        {
            float xx =  _grid->x + (xind * (1.0f / _grid->inv_cell_w));
            float y1 = _grid->y;
            float y2 = _grid->y + (_grid->h);
            std::array line =
            {
                sf::Vertex{sf::Vector2f(xx, y1), sf::Color(255,255,255, 55)},
                sf::Vertex{sf::Vector2f(xx, y2), sf::Color(255,255,255, 55)},
            };

            renderWindow->draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        }
        for (int yind = 0; yind < _grid->num_rows; ++yind)
        {
            float yy =  _grid->y + (yind * (1.0f / _grid->inv_cell_h));
            float x1 = _grid->x;
            float x2 = _grid->x + (_grid->w);

            std::array line =
            {
                sf::Vertex{sf::Vector2f(x1, yy), sf::Color(255,255,255, 55)},
                sf::Vertex{sf::Vector2f(x2, yy), sf::Color(255,255,255, 55)},
            };

            renderWindow->draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        }
    }
} // ECS_SFML