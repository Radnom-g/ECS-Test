//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include <string>
#include <vector>

namespace ECS
{
    class ISystem
    {
    public:
        // Derived classes should define the following (in public scope)
        //static constexpr std::string SystemName = "ISystem"; < replace with the class name.

        friend class SystemManager;

        virtual ~ISystem() = default;

        //Derived classes will want to implement an Initialise function,
        // something like the below:
        // bool Initialise(EntityManager* _entityManager, ComponentManager& _componentManager);

        void Process(float _deltaSeconds);
        void Render(float _deltaTween);

        virtual const std::string& GetSystemName() = 0; // { return SystemName; }

        // Used to set up the order of processing
        virtual void GetProcessAfter(std::vector<std::string>& _outSystems) = 0;
        virtual void GetRenderAfter(std::vector<std::string>& _outSystems) = 0;

    protected:
        virtual void ProcessInternal(float _deltaTick) = 0;
        virtual void RenderInternal(float _deltaTick) = 0;

        virtual bool GetDoesProcessTick() = 0;
        virtual bool GetDoesRenderTick() = 0;

        bool isInitialised = false;


    };
} // ECS