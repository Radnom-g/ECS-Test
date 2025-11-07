//
// Created by Radnom on 7/11/2025.
//

#include "SFMLWorld.h"

#include "SFMLWorldContext.h"
#include "../../ECS-SFML/Systems/RenderSystem.h"
#include "../../ECS-SFML/Systems/TransformSystem.h"
#include "Components/ComponentManager.h"
#include "../ECS-SFML/Components/TransformComponent.h"
#include "../ECS-SFML/Components/SpriteComponent.h"

namespace ECS_SFML
{
    bool SFMLWorld::Initialise(ECS::WorldSettings *_worldSettings, sf::RenderWindow *_renderWindow)
    {
        renderWindow = _renderWindow;
        Initialise(_worldSettings);
        return true;
    }

    bool SFMLWorld::Initialise(ECS::WorldSettings *_worldSettings)
    {
        return World::Initialise(_worldSettings);
    }

    void SFMLWorld::InitialiseInternal()
    {
        assert(worldContextSFML && "World Context is not SFML context");

        worldContextSFML->transformSystem = &transformSystem;
        worldContextSFML->renderSystem = &renderSystem;

        transformSystem.Initialise(worldContextSFML);
        renderSystem.Initialise(worldContextSFML);

        int spriteIndex = -1;
        resourceManager.GetOrLoadTexture("resources/qmark_block.png", spriteIndex);

        ECS_SFML::TransformComponent* transformComponent = componentManager.GetComponent<ECS_SFML::TransformComponent>();
        ECS_SFML::SpriteComponent* spriteComponent = componentManager.GetComponent<ECS_SFML::SpriteComponent>();

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

        World::InitialiseInternal();
    }

    void SFMLWorld::CreateWorldContext()
    {
        // do NOT create this:
        //World::CreateWorldContext();

        worldContextSFML = new ECS_SFML::SFMLWorldContext();
        worldContext = worldContextSFML;

        worldContext->world = this;
        worldContext->entityManager = &entityManager;
        worldContext->componentManager = &componentManager;
        worldContext->worldSettings = worldSettings;

        worldContextSFML->resourceManager = &resourceManager;
        worldContextSFML->renderWindow = renderWindow;
    }

    void SFMLWorld::CreateAndRegisterComponentsInternal()
    {
        World::CreateAndRegisterComponentsInternal();

        ECS::ComponentSettings transformSettings = ECS_SFML::TransformComponent::CreateSettings<ECS_SFML::TransformComponent>(1000, 100000);
        ECS::ComponentSettings spriteSettings = ECS_SFML::SpriteComponent::CreateSettings<ECS_SFML::SpriteComponent>(1000, 100000);

        worldSettings->ComponentSettings.push_back(transformSettings);
        worldSettings->ComponentSettings.push_back(spriteSettings);
    }

    void SFMLWorld::InitialiseComponents()
    {
        World::InitialiseComponents();
    }

    void SFMLWorld::ReinitialiseInternal()
    {
        World::ReinitialiseInternal();
    }

    void SFMLWorld::UpdateInternal(float _deltaSeconds)
    {
        World::UpdateInternal(_deltaSeconds);
        transformSystem.Process(_deltaSeconds);
        renderSystem.Process(_deltaSeconds);
    }

    void SFMLWorld::RenderInternal(float _deltaTween)
    {
        World::RenderInternal(_deltaTween);
        renderSystem.Render(_deltaTween);
    }
} // ECS_SFML