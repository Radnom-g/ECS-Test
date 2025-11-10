//
// Created by Radnom on 7/11/2025.
//

#include "SFMLWorld.h"

#include "SFMLWorldContext.h"
#include "../../ECS-SFML/Systems/RenderSystem.h"
#include "../../ECS-SFML/Systems/TransformSystem.h"
#include "../Components/ScreenWrapComponent.h"
#include "../ECS-SFML/Components/TransformComponent.h"
#include "../ECS-SFML/Components/SpriteComponent.h"
#include "../ECS-SFML/Components/VelocityComponent.h"

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
        World::InitialiseInternal();

        assert(worldContextSFML && "World Context is not SFML context");

        worldContextSFML->transformSystem = &transformSystem;
        worldContextSFML->renderSystem = &renderSystem;
        worldContextSFML->movementSystem = &movementSystem;
        worldContextSFML->collisionSystem = &collisionSystem;

        transformSystem.Initialise(worldContextSFML);
        renderSystem.Initialise(worldContextSFML);
        movementSystem.Initialise(worldContextSFML);
        collisionSystem.Initialise(worldContextSFML);

        systemManager.RegisterSystem(&transformSystem);
        systemManager.RegisterSystem(&renderSystem);
        systemManager.RegisterSystem(&movementSystem);
        systemManager.RegisterSystem(&collisionSystem);

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
        ECS::ComponentSettings velocitySettings = ECS_SFML::VelocityComponent::CreateSettings<ECS_SFML::VelocityComponent>(1000, 100000);
        ECS::ComponentSettings screenWrapSettings = ECS_SFML::ScreenWrapComponent::CreateSettings<ECS_SFML::ScreenWrapComponent>(1000, 100000);

        worldSettings->ComponentSettings.push_back(transformSettings);
        worldSettings->ComponentSettings.push_back(spriteSettings);
        worldSettings->ComponentSettings.push_back(velocitySettings);
        worldSettings->ComponentSettings.push_back(screenWrapSettings);
    }

    void SFMLWorld::InitialiseComponents()
    {
        World::InitialiseComponents();
    }

    void SFMLWorld::ReinitialiseInternal()
    {
        World::ReinitialiseInternal();
    }

    void SFMLWorld::UpdateInternalEarly(float _deltaSeconds)
    {
        World::UpdateInternalEarly(_deltaSeconds);
    }

    void SFMLWorld::UpdateInternalLate(float _deltaSeconds)
    {
        World::UpdateInternalLate(_deltaSeconds);
    }

    void SFMLWorld::RenderInternalEarly(float _deltaTween)
    {
        World::RenderInternalEarly(_deltaTween);
    }

    void SFMLWorld::RenderInternalLate(float _deltaTween)
    {
        World::RenderInternalLate(_deltaTween);
    }
} // ECS_SFML