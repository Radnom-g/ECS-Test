//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "Systems/ISystem.h"

#include "Managers/AccessorManager.h"
#include "SFML/System/Vector2.hpp"
#include "../Accessors/AccessorTypes.h"
#include "../Managers/ResourceManager.h"
#include "Accessors/DepthAccessor.h"

namespace sf
{
    class RenderWindow;
}

namespace  ECS
{
    class EntityManager;
}

namespace ECS_SFML
{
    class RenderSystem : public ECS::ISystem
    {
    public:
        bool Initialise(ECS::EntityManager* _entityManager, ECS::AccessorManager* _accessorManager,
            ECS_SFML::ResourceManager* _resourceManager, sf::RenderWindow* _renderWindow);

    protected:
        void ProcessInternal(float _deltaSeconds) override;
        void RenderInternal(float _deltaTick) override;

        inline virtual const char* GetSystemName() override { return "RenderSystem"; }
        bool GetDoesProcessTick() override { return false; }
        bool GetDoesRenderTick() override { return true; }

        ECS::EntityManager* entityManager = nullptr;
        sf::RenderWindow* renderWindow = nullptr;
        ResourceManager* resourceManager = nullptr;
	    RenderSpriteController* renderSpriteController = nullptr;
        ECS::DepthAccessor* depthAccessor = nullptr;
        TransformController* transformController = nullptr;
    };
} // ECS_SFML