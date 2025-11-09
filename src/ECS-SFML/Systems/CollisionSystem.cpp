//
// Created by Radnom on 8/11/2025.
//

#include "CollisionSystem.h"
#include "../ECS-SFML/Worlds/SFMLWorldContext.h"
#include "Components/ComponentManager.h"
#include "Components/TreeComponent.h"
#include "../ECS-SFML/Components/TransformComponent.h"

namespace ECS_SFML
{
    bool CollisionSystem::Initialise(SFMLWorldContext *_context)
    {
        transformComponent = _context->componentManager->GetComponent<TransformComponent>();
        collisionComponent = _context->componentManager->GetComponent<ECS::CollisionComponent>();
        treeComponent = _context->componentManager->GetComponent<ECS::TreeComponent>();
        transformSystem = _context->transformSystem;

        isInitialised = true;


        return true;
    }

} // ECS_SFML