//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "../../ECS/Components/IComponent.h"
#include <SFML/System/Vector2.hpp>

namespace ECS_SFML
{
    class TransformComponent: public ECS::IComponent<TransformComponent>
    {
    public:
        sf::Vector2f position = sf::Vector2f(0,0);
        float angle = 0;
    private:
        inline virtual void ClearInternal() override
        {
            position = sf::Vector2f(0, 0);
            angle = 0;
        }
    public:
        static const char* GetName() { return "TransformComponent"; }
    };
} // ECS_SFML