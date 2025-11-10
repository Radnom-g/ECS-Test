//
// Created by Radnom on 3/11/2025.
//

#include "World.h"

#include "WorldContext.h"
#include "../Entities/EntityManager.h"
#include "Components/CollisionComponent.h"
#include "Components/TreeComponent.h"

namespace ECS
{
    World::~World()
    {
        if (worldContext != nullptr)
        {
            delete worldContext;
            worldContext = nullptr;
        }
        if (worldSettings != nullptr)
        {
            delete worldSettings;
            worldSettings = nullptr;
        }
    }

    bool World::Initialise(WorldSettings* _worldSettings)
    {
        if (isInitialised)
        {
            return false;
        }
        worldSettings = _worldSettings;

        CreateWorldContext();

        CreateAndRegisterComponents();

        // Must have registered all components before initialising, as some have inter-dependencies
        InitialiseComponents();

        entityManager.Initialise(worldContext, _worldSettings->entityCapacityInitial, _worldSettings->entityCapacityMax);

        InitialiseInternal();

        isInitialised = true;
        return true;
    }

    void World::Reinitialise()
    {
        entityManager.ResetAll();

        ReinitialiseInternal();
    }

    void World::Update(float _deltaSeconds)
    {
        UpdateInternalEarly(_deltaSeconds);
        systemManager.Process(_deltaSeconds);
        UpdateInternalLate(_deltaSeconds);
    }

    void World::Render(float _deltaTween)
    {
        RenderInternalEarly(_deltaTween);
        systemManager.Render(_deltaTween);
        RenderInternalLate(_deltaTween);
    }

    WorldContext* World::GetWorldContext() const
    {
        return worldContext;
    }

    void World::CreateWorldContext()
    {
        worldContext = new WorldContext();
        worldContext->world = this;
        worldContext->entityManager = &entityManager;
        worldContext->componentManager = &componentManager;
        worldContext->worldSettings = worldSettings;
        worldContext->systemManager = &systemManager;
    }

    void World::CreateAndRegisterComponents()
    {
        // TODO: use the WorldSettings to initialise capacity values
        ECS::ComponentSettings treeSettings = ECS::TreeComponent::CreateSettings<ECS::TreeComponent>(1000, 100000);
        worldSettings->ComponentSettings.push_back(treeSettings);

        ECS::ComponentSettings collisionSettings = ECS::CollisionComponent::CreateSettings<ECS::CollisionComponent>(1000, 100000);
        worldSettings->ComponentSettings.push_back(collisionSettings);

        // Derived Worlds can add more Components here
        CreateAndRegisterComponentsInternal();

        for (ComponentSettings& settings : worldSettings->ComponentSettings)
        {
            IComponent* newComponent = settings.Constructor();
            componentManager.RegisterComponent(&settings, newComponent);
        }
    }

    void World::InitialiseComponents()
    {
        componentManager.InitialiseComponents(worldContext);
    }
} // ECS