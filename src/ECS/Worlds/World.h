//
// Created by Radnom on 3/11/2025.
//

#pragma once

#include "WorldSettings.h"
#include "../Entities/EntityManager.h"
#include "Components/ComponentManager.h"

namespace ECS
{
    // A World has an EntityManager and a AccessorManager.
    // Essentially a separate and independent canvas for some game logic to run on.
    // You could have a Game World and a Pause Menu state running on top of it.
    class World
    {

    public:
        virtual ~World();

        // The World will own the World Settings pointer and destroy it
        virtual bool Initialise(WorldSettings* _worldSettings);
        void Reinitialise();

        void Update(float _deltaSeconds);

        // _deltaTween between 0-1, from previous frame to next frame.
        // So that we can have a fixed update/physics tick, but smooth graphics lerping from prev. to next frame
        void Render(float _deltaTween);

        inline WorldContext* GetWorldContext() const;

    private:
        // Derived classes may want to define their own components and register them with componentManager in here.
        void CreateAndRegisterComponents();

    protected:
        // Derived classes may need to create a derived WorldContext struct in here.
        virtual void CreateWorldContext();

        // After registering all Components, they are initialised.
        virtual void InitialiseComponents();

        // Derived classes may want to define their own components and register them with componentManager in here.
        virtual void CreateAndRegisterComponentsInternal() {}
        virtual void InitialiseInternal() {}
        virtual void ReinitialiseInternal() {}
        virtual void UpdateInternal(float _deltaSeconds) {}
        virtual void RenderInternal(float _deltaTween) {}

        EntityManager entityManager = EntityManager();
        ComponentManager componentManager = ComponentManager();


        WorldContext* worldContext = nullptr;
        WorldSettings* worldSettings = nullptr;

        bool isInitialised = false;
        int defaultEntityCount = 1;
        int maxEntityCount = 1;
    };
} // ECS
