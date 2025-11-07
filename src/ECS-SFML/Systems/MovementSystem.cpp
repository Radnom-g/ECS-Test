//
// Created by Radnom on 7/11/2025.
//

#include "MovementSystem.h"

#include "TransformSystem.h"
#include "../ECS-SFML/Worlds/SFMLWorldContext.h"
#include "Components/ComponentManager.h"
#include "../ECS-SFML/Components/TransformComponent.h"
#include "../Components/VelocityComponent.h"

namespace ECS_SFML
{
    bool MovementSystem::Initialise(SFMLWorldContext *_context)
    {
        transformComponent = _context->componentManager->GetComponent<TransformComponent>();
        velocityComponent = _context->componentManager->GetComponent<VelocityComponent>();
        transformSystem = _context->transformSystem;

        isInitialised = true;
        return true;
    }

    void MovementSystem::ProcessInternal(float _deltaTick)
    {
        for (int vCompInd = 0; vCompInd < velocityComponent->entityId.size(); ++vCompInd)
        {
            // Find out what needs ticking..
            if (velocityComponent->entityId[vCompInd] != -1)
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
                    // This will also update the cache.
                    transformSystem->MoveEntity( velocityComponent->entityId[vCompInd], vel * _deltaTick);
                }
            }
        }
    }
} // ECS_SFML