//
// Created by Radnom on 4/11/2025.
//

#include "TestGameWorld.h"

#include "../../ECS-SFML/Systems/RenderSystem.h"
#include "../../ECS-SFML/Systems/TransformSystem.h"
#include "../ECS-SFML/Components/TransformComponent.h"
#include "../ECS-SFML/Components/RenderSpriteComponent.h"
#include "Worlds/WorldContext.h"
#include "../ECS-SFML/Worlds/SFMLWorldContext.h"

#include "Worlds/World.h"

namespace ECS_Game
{
    bool TestGameWorld::Initialise(ECS::WorldSettings *_worldSettings, sf::RenderWindow *_renderWindow)
    {
        renderWindow = _renderWindow;
        Initialise(_worldSettings);
        return true;
    }

    bool TestGameWorld::Initialise(ECS::WorldSettings *_worldSettings)
    {
        return World::Initialise(_worldSettings);
    }

    void TestGameWorld::InitialiseInternal()
    {

        assert(worldContextSFML && "World Context is not SFML context");

        worldContextSFML->transformSystem = &transformSystem;
        worldContextSFML->renderSystem = &renderSystem;

        transformSystem.Initialise(worldContextSFML);
        renderSystem.Initialise(worldContextSFML);

        int spriteIndex = -1;
        resourceManager.GetOrLoadTexture("resources/qmark_block.png", spriteIndex);

        ECS_SFML::TransformComponent* transformComponent = componentManager.GetComponent<ECS_SFML::TransformComponent>();
        ECS_SFML::RenderSpriteComponent* spriteComponent = componentManager.GetComponent<ECS_SFML::RenderSpriteComponent>();

        for (int ii = 0; ii < 10; ii++)
        {
            int newEnt = entityManager.ActivateEntity("Block");

            int newT = transformComponent->AddComponent(newEnt);
            int newS = spriteComponent->AddComponent(newEnt);

            spriteComponent->SetTextureIndex(newS, spriteIndex);
            transformComponent->SetDepth(newT, std::rand() % 3);

            sf::Vector2f randPos( (std::rand() % 10) * 16, (std::rand() % 10) * 16 );

            transformComponent->SetPosition(newT, randPos);
        }
    }

    void TestGameWorld::ReinitialiseInternal()
    {
    }

    void TestGameWorld::UpdateInternal(float _deltaSeconds)
    {
        transformSystem.Process(_deltaSeconds);
    }

    void TestGameWorld::RenderInternal(float _deltaTween)
    {
        renderSystem.Render(_deltaTween);
    }

    void TestGameWorld::CreateWorldContext()
    {
        worldContext = new ECS_SFML::SFMLWorldContext();

        worldContextSFML = dynamic_cast<ECS_SFML::SFMLWorldContext*>(worldContext);

        worldContext->world = this;
        worldContext->entityManager = &entityManager;
        worldContext->componentManager = &componentManager;
        worldContext->worldSettings = worldSettings;

        worldContextSFML->resourceManager = &resourceManager;
        worldContextSFML->renderWindow = renderWindow;

    }
} // ECS_Game