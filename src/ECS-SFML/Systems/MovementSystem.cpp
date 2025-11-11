//
// Created by Radnom on 7/11/2025.
//

#include "MovementSystem.h"

#include <windows.h>
#include <debugapi.h>

#include "TransformSystem.h"
#include "../Components/ScreenWrapComponent.h"
#include "../ECS-SFML/Worlds/SFMLWorldContext.h"
#include "Components/ComponentManager.h"
#include "../ECS-SFML/Components/TransformComponent.h"
#include "../ECS-SFML/Systems/CollisionSystem.h"
#include "../Components/VelocityComponent.h"
#include "SFML/Graphics/RenderWindow.hpp"

namespace ECS_SFML
{
    bool MovementSystem::Initialise(SFMLWorldContext *_context)
    {
        transformComponent = _context->componentManager->GetComponent<TransformComponent>();
        velocityComponent = _context->componentManager->GetComponent<VelocityComponent>();
        screenWrapComponent = _context->componentManager->GetComponent<ScreenWrapComponent>();

        transformSystem = _context->transformSystem;
        collisionSystem = _context->collisionSystem;

        worldSize = sf::Vector2f(static_cast<float>(_context->worldSettings->worldWidth),
                                 static_cast<float>( _context->worldSettings->worldHeight));

        isInitialised = true;
        return true;
    }

    void MovementSystem::ProcessInternal(float _deltaTick)
    {
        for (int vCompInd = 0; vCompInd < velocityComponent->entityId.size(); ++vCompInd)
        {
            // Find out what needs ticking..
            int entity = velocityComponent->entityId[vCompInd];
            if (entity != -1)
            {
                sf::Vector2f acc = velocityComponent->acceleration[vCompInd];
                sf::Vector2f vel = velocityComponent->velocity[vCompInd];
                float fric = velocityComponent->friction[vCompInd];
                float maxSpeed = velocityComponent->maxSpeed[vCompInd];

                if (!IsEmpty(acc))
                {
                    vel += acc * _deltaTick;
                    velocityComponent->SetVelocity(vCompInd, vel);
                }
                if (fric > 0)
                {
                    sf::Vector2f fricVec = -vel;
                    if (!IsEmpty(fricVec))
                    {
                        fricVec = fricVec.normalized();
                        fricVec *= fric;

                        vel -= fricVec * _deltaTick;
                        velocityComponent->SetVelocity(vCompInd, vel);
                    }
                }
                if (!IsEmpty(vel))
                {
                    // Limit speed.
                    if (maxSpeed >= 0.0f)
                    {
                        float speedSq = vel.lengthSquared();
                        if (speedSq > (maxSpeed * maxSpeed))
                        {
                            if (vel != sf::Vector2f(0.0f, 0.0f))
                                vel = vel.normalized();
                            vel *= maxSpeed;
                            velocityComponent->SetVelocity(vCompInd, vel);
                        }
                    }


                    // std::stringstream strDebug;
                    // strDebug << "Entity #" << entity << " before move: <";
                    // sf::Vector2f pos = transformSystem->GetEntityWorldPosition(entity);
                    // strDebug << pos.x << ", " << pos.y << ">";
                    // OutputDebugString(strDebug.str().c_str());

                    sf::Vector2f velX = vel;
                    sf::Vector2f velY = vel;
                    velX.y = 0.0f;
                    velY.x = 0.0f;

                    bool bColX = false;
                    bool bColY = false;
                    if (abs(vel.x) > abs(vel.y))
                    {
                        bColX = !MoveEntity(entity, velX * _deltaTick, EMovementType::Collide);
                        if (abs(vel.y) > 0.0f)
                        {
                            bColY = !MoveEntity(entity, velY * _deltaTick, EMovementType::Collide);
                        }
                    }
                    else
                    {
                        bColY = !MoveEntity(entity, velX * _deltaTick, EMovementType::Collide);
                        if (abs(vel.x) > 0.0f)
                        {
                            bColX = !MoveEntity(entity, velX * _deltaTick, EMovementType::Collide);
                        }
                    }

                    if (bColX)
                    {
                        //TODO: TEMP: just reverse for now
                        int vCompInd = velocityComponent->GetComponentIndex(entity);
                        velocityComponent->velocity[vCompInd].x = -velocityComponent->velocity[vCompInd].x;
                        velocityComponent->acceleration[vCompInd].x = -velocityComponent->acceleration[vCompInd].x;
                    }
                    if (bColY)
                    {

                        //TODO: TEMP: just reverse for now
                        int vCompInd = velocityComponent->GetComponentIndex(entity);
                        velocityComponent->velocity[vCompInd].y = -velocityComponent->velocity[vCompInd].y;
                        velocityComponent->acceleration[vCompInd].y = -velocityComponent->acceleration[vCompInd].y;
                    }

                }
            }
        }

        for (int wrapCompInd = 0; wrapCompInd < screenWrapComponent->entityId.size(); ++wrapCompInd)
        {
            int entity = screenWrapComponent->entityId[wrapCompInd];
            if (entity != -1)
            {
                int tformIndex = transformComponent->GetComponentIndex(entity);
                assert(tformIndex != -1 && "Can't screen wrap something without a transform!");

                sf::Vector2f worldPos = transformSystem->GetWorldTransform( tformIndex ).getPosition();
                bool bWrapped = false;

                if (worldPos.x < 0)
                {
                    worldPos.x += worldSize.x;
                    bWrapped = true;
                }
                if (worldPos.y < 0)
                {
                    worldPos.y += worldSize.y;
                    bWrapped = true;
                }
                if (worldPos.x > worldSize.x)
                {
                    worldPos.x -= worldSize.x;
                    bWrapped = true;
                }
                if (worldPos.y > worldSize.y)
                {
                    worldPos.y -= worldSize.y;
                    bWrapped = true;
                }

                if (bWrapped)
                {
                    SetEntityLocation(entity, worldPos, EMovementType::Teleport);
                }
            }
        }
    }

    void MovementSystem::GetProcessAfter(std::vector<std::string> &_outSystems)
    {
        _outSystems.clear();
        _outSystems.push_back( TransformSystem::SystemName );
    }

    bool MovementSystem::MoveEntity(int _entity, sf::Vector2f _movement, EMovementType _movementType)
    {
        int tformIndex = transformComponent->GetComponentIndex(_entity);

        sf::Vector2f newPos = transformSystem->GetWorldTransform(tformIndex).getPosition() + _movement;

        std::vector<CollisionResult> results;
        if (collisionSystem->CheckCollisions(_entity, newPos,  results))
        {

            // std::stringstream strDebug;
            // strDebug << "Entity #" << _entity << " collision at <";
            // strDebug << newPos.x << ", " << newPos.y << ">";
            // strDebug << " - movement vec: <" << _movement.x << ", " << _movement.y << ">";
            // OutputDebugString(strDebug.str().c_str());

            return false;
        }
        transformSystem->MoveTransform( tformIndex, _movement);

        // std::stringstream strDebug;
        // strDebug << "Entity #" << _entity << " no collision at <";
        // strDebug << newPos.x << ", " << newPos.y << ">";
        // strDebug << " - movement vec: <" << _movement.x << ", " << _movement.y << ">";
        // OutputDebugString(strDebug.str().c_str());

        return true;
    }

    bool MovementSystem::SetEntityLocation(int _entity, sf::Vector2f _newPos, EMovementType _movementType)
    {
        int tformIndex = transformComponent->GetComponentIndex(_entity);
        Transform transform = transformSystem->GetWorldTransform(tformIndex);
        transform.setPosition(_newPos);
        transformSystem->SetWorldTransform(tformIndex, transform);
        if (_movementType == EMovementType::Teleport)
        {
            transformSystem->MarkEntityAsTeleported(_entity);
        }
        return true; //TODO: check for collisions and return if it was blocked.
    }
} // ECS_SFML