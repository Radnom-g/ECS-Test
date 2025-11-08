//
// Created by Radnom on 7/11/2025.
//

#include "MovementSystem.h"

#include "TransformSystem.h"
#include "../Components/ScreenWrapComponent.h"
#include "../ECS-SFML/Worlds/SFMLWorldContext.h"
#include "Components/ComponentManager.h"
#include "../ECS-SFML/Components/TransformComponent.h"
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
                sf::Vector2f fric = velocityComponent->friction[vCompInd];

                if (!IsEmpty(acc))
                {
                    vel += acc * _deltaTick;
                    velocityComponent->SetVelocity(vCompInd, vel);
                }
                if (!IsEmpty(fric))
                {
                    vel -= fric * _deltaTick;
                    velocityComponent->SetVelocity(vCompInd, vel);
                }
                if (!IsEmpty(vel))
                {
                    int tformIndex = transformComponent->GetComponentIndex(entity);

                    // This will also update the cache.
                    transformSystem->MoveTransform( tformIndex, vel * _deltaTick);

                    MoveEntity(entity, vel * _deltaTick, EMovementType::Collide);
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

    bool MovementSystem::MoveEntity(int _entity, sf::Vector2f _movement, EMovementType _movementType)
    {
        int tformIndex = transformComponent->GetComponentIndex(_entity);
        transformSystem->MoveTransform( tformIndex, _movement);
        return true; //TODO: Check for collisions and return if it was blocked.
    }

    bool MovementSystem::SetEntityLocation(int _entity, sf::Vector2f _newPos, EMovementType _movementType)
    {
        int tformIndex = transformComponent->GetComponentIndex(_entity);
        Transform transform = transformSystem->GetWorldTransform(tformIndex);
        transform.setPosition(_newPos);
        transformSystem->SetWorldTransform(tformIndex, transform);
        if (_movementType == EMovementType::Teleport)
        {
            transformSystem->MarkTransformAsTeleported(tformIndex);
        }
        return true; //TODO: check for collisions and return if it was blocked.
    }
} // ECS_SFML