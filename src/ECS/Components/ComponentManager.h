//
// Created by Radnom on 5/11/2025.
//

#pragma once
#include "Components/IComponent.h"

namespace ECS
{
    class ComponentManager
    {
    public:
        ~ComponentManager();

        void OnEntityDestroyed(const Entity& _entity);

        void InitialiseComponents(WorldContext* _context);
        void RegisterComponent(ComponentSettings* _componentSettings, IComponent* _component);
        void ResizeEntityArray(int _newSize);

        template <typename T>
        T* GetComponent()
        {
		    static_assert(std::is_base_of_v<IComponent, T>, "T must derive from IComponent");
            for (IComponent* comp : components)
            {
                T* result = dynamic_cast<T*>(comp);
                if (result)
                {
                    return result;
                }
            }
            return nullptr;
        }

        int GetComponentCount() { return static_cast<int>(components.size()); }

    protected:
        std::vector<IComponent*> components {};

    };
} // ECS