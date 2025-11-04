//
// Created by Radnom on 3/11/2025.
//

#include "State.h"
#include "Managers/EntityManager.h"

namespace ECS
{

    bool State::Initialise()
    {
        if (isInitialised)
        {
            return false;
        }

        entityManager.Initialise(&controllerManager, defaultEntityCount, maxEntityCount);

        isInitialised = true;
        return true;
    }

    void State::Reinitialise()
    {
    }

    void State::Update(float _deltaTick)
    {
    }

    void State::Render(float _deltaTick)
    {
    }
} // ECS