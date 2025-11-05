//
// Created by Radnom on 3/11/2025.
//

#include "World.h"
#include "Managers/EntityManager.h"

namespace ECS
{

    bool World::Initialise()
    {
        if (isInitialised)
        {
            return false;
        }

        entityManager.Initialise(&accessorManager, defaultEntityCount, maxEntityCount);

        isInitialised = true;
        return true;
    }

    void World::Reinitialise()
    {
    }

    void World::Update(float _deltaSeconds)
    {

        // finally:
        accessorManager.ProcessEndOfFrame();
    }

    void World::Render(float _deltaTween)
    {
    }
} // ECS