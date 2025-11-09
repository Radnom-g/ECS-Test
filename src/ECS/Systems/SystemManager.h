//
// Created by Radnom on 9/11/2025.
//

#pragma once
#include <vector>

namespace ECS
{
    class ISystem;

    class SystemManager
    {
        public:
        virtual ~SystemManager() = default;

        void Process(float _deltaSeconds);
        void Render(float _deltaTween);

        void RegisterSystem(ISystem* _system);


        template <typename T>
        T* GetSystem()
        {
            static_assert(std::is_base_of_v<ISystem, T>, "T must derive from ISystem");
            for (ISystem* comp : systems)
            {
                T* result = dynamic_cast<T*>(comp);
                if (result)
                {
                    return result;
                }
            }
            return nullptr;
        }


    protected:

        void ReorderSystems();

        // Unordered 'super' list of systems
        std::vector<ISystem*> systems;

        // Not all systems need a process or render tick.
        // For systems that process or render, they might need to process or render in a specfic order.
        // This is set up in RegisterSystem.
        std::vector<ISystem*> processSystemsOrdered;
        std::vector<ISystem*> renderSystemsOrdered;
    };
} // ECS