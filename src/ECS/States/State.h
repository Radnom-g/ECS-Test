//
// Created by Radnom on 3/11/2025.
//

#pragma once

#include "Managers/ControllerManager.h"
#include "Managers/EntityManager.h"

namespace ECS
{
    // A State has an EntityManager and a ControllerManager.
    // Essentially a separate and independent canvas for some game logic to run on.
    // You could have a Game State and a Pause Menu state running on top of it.
    class State
    {

    public:
        virtual ~State() = default;

        virtual bool Initialise();
        virtual void Reinitialise();

        virtual void Update(float _deltaTick);
        virtual void Render(float _deltaTick);

    private:
        EntityManager entityManager = EntityManager();
        ControllerManager controllerManager = ControllerManager();

        bool isInitialised = false;
        int defaultEntityCount = 1;
        int maxEntityCount = 1;
    };
} // ECS
