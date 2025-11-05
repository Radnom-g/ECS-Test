//
// Created by Radnom on 4/11/2025.
//

#include "TestGameWorld.h"

#include "../../ECS-SFML/Systems/RenderSystem.h"
#include "../ECS-SFML/Components/TransformComponent.h"
#include "../ECS-SFML/Components/RenderSpriteComponent.h"
#include "Worlds/World.h"

namespace ECS_Game
{
    bool TestGameWorld::Initialise()
    {
        maxEntityCount = 10000;
        return ECS::World::Initialise();
    }

    bool TestGameWorld::Initialise(sf::RenderWindow &_window)
    {
        bool success = Initialise();

        renderSystem.Initialise(&entityManager, &accessorManager, &resourceManager, &_window);

        ECS::DepthAccessor* depthAccessor = dynamic_cast<ECS::DepthAccessor*>(accessorManager.GetComponentAccessor<ECS::DepthComponent>());
        if (depthAccessor == nullptr)
        {
            depthAccessor = new ECS::DepthAccessor();
            depthAccessor->Initialise(100, entityManager.GetMaxCapacity());
            accessorManager.RegisterComponentAccessor(depthAccessor);
        }
        ECS_SFML::TransformController* transformController = accessorManager.GetComponentAccessor<ECS_SFML::TransformComponent>();
        if (transformController == nullptr)
        {
            transformController = accessorManager.CreateComponentAccessor<ECS_SFML::TransformComponent>();
        }
        ECS_SFML::RenderSpriteController* spriteController = accessorManager.GetComponentAccessor<ECS_SFML::RenderSpriteComponent>();
        if (spriteController == nullptr)
        {
            spriteController = accessorManager.CreateComponentAccessor<ECS_SFML::RenderSpriteComponent>();
        }

        int spriteIndex = -1;
        resourceManager.GetOrLoadTexture("resources/qmark_block.png", spriteIndex);

        for (int ii = 0; ii < 10; ii++)
        {
            int newEnt = entityManager.ActivateEntity("Block");

             accessorManager.GetComponentAccessor<ECS_SFML::TransformComponent>();

            ECS::DepthComponent* dcom = depthAccessor->AddComponent(newEnt);
            if (dcom)
            {
                dcom->depth = std::rand() % 3;
            }

            ECS_SFML::TransformComponent* tcom = transformController->AddComponent(newEnt);
            if (tcom)
            {
                tcom->position = sf::Vector2f( (std::rand() % 10) * 16, (std::rand() % 10) * 16 );
            }

            ECS_SFML::RenderSpriteComponent* rcom = spriteController->AddComponent(newEnt);
            if (rcom)
            {
                rcom->textureId = spriteIndex;
            }

        }

        return success;
    }

    void TestGameWorld::Reinitialise()
    {
        ECS::World::Reinitialise();
    }

    void TestGameWorld::Update(float _deltaSeconds)
    {
        ECS::World::Update(_deltaSeconds);
    }

    void TestGameWorld::Render(float _deltaTween)
    {
        ECS::World::Render(_deltaTween);

        renderSystem.Render(_deltaTween);
    }
} // ECS_Game