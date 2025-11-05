//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "Components/IComponent.h"

#include<SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

namespace ECS_SFML
{
    class RenderSpriteComponent : public ECS::IComponent<RenderSpriteComponent>
    {
    public:
        ~RenderSpriteComponent() override = default;

        sf::Vector2f position = sf::Vector2f(0,0); //relative to transform, if we have one, otherwise from 0,0
        int textureId = -1;
        float angle = 0;

        static const char* GetName() { return "RenderSpriteComponent"; }
        static bool CanEntityHaveMultiple() { return true; }

    protected:
        inline void ClearInternal() override
        {
            textureId = -1;
            angle = 0;
            position = sf::Vector2f();
        }
    };
} // ECS_SFML