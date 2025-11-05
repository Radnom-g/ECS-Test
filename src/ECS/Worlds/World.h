//
// Created by Radnom on 3/11/2025.
//

#pragma once

#include "Managers/AccessorManager.h"
#include "Managers/EntityManager.h"

namespace ECS
{
    // A World has an EntityManager and a AccessorManager.
    // Essentially a separate and independent canvas for some game logic to run on.
    // You could have a Game World and a Pause Menu state running on top of it.
    class World
    {

    public:
        virtual ~World() = default;

        virtual bool Initialise();
        virtual void Reinitialise();

        virtual void Update(float _deltaSeconds);

        // _deltaTween between 0-1, from previous frame to next frame.
        // So that we can have a fixed update/physics tick, but smooth graphics lerping from prev. to next frame
        virtual void Render(float _deltaTween);

    protected:
        EntityManager entityManager = EntityManager();
        AccessorManager accessorManager = AccessorManager();

        bool isInitialised = false;
        int defaultEntityCount = 1;
        int maxEntityCount = 1;
    };
} // ECS
