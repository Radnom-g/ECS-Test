//
// Created by Radnom on 8/11/2025.
//

#pragma once
#include "Components/IComponent.h"

namespace ECS_SFML
{
    class BoundsComponent : public ECS::IComponent
    {
    public:
        friend class TransformSystem;
        friend class CollisionSystem;
        friend class MovementSystem;

        ~BoundsComponent() override = default;

    protected:
        // X and Y should come from TransformComponent.
        std::vector<float> width{};
        std::vector<float> height{};
    };
} // ECS_SFML