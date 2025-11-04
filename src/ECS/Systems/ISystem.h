//
// Created by Radnom on 4/11/2025.
//

#pragma once

namespace ECS
{
    class ISystem
    {
    public:
        virtual ~ISystem() = default;

        //Derived classes will want to implement an Initialise function,
        // something like the below:
        // bool Initialise(EntityManager* _entityManager, ComponentManager& _componentManager);

        void Process(float _deltaTick);

        virtual const char* GetSystemName() = 0;

    protected:
        virtual void ProcessInternal(float _deltaTick) = 0;
        bool ConfirmInitialised();
        bool isInitialised = false;


    };
} // ECS