//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "IComponent.h"

#include<SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

namespace ECS_SFML
{
    class RenderSpriteComponent : public IComponent<RenderSpriteComponent>
    {
    public:
        virtual ~RenderSpriteComponent() = default;

        sf::Vector2f position = sf::Vector2f(0,0); //relative to transform, if we have one, otherwise from 0,0
        sf::Sprite* sprite = nullptr;
        float angle = 0;

        static const char* GetName() { return "RenderSpriteComponent"; }

    protected:
        inline virtual void ClearInternal() override
        {
            sprite = nullptr;
            angle = 0;
            position = sf::Vector2f();
        }
    };
} // ECS_SFML