//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "../../ECS-SFML/Worlds/SFMLWorld.h"

namespace ECS_Game
{
    class TestGameWorld : public ECS_SFML::SFMLWorld
    {
    public:
        // Nothing yet!
        // This is where we'd put game-specific systems in place.
        // Like score systems or whatever.

        void InitialiseInternal() override;
        void UpdateInternal(float _deltaSeconds) override;

    protected:
        std::vector<ECS::Entity> entitiesToRotate;
    };
} // ECS_Game