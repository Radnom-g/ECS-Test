//
// Created by Radnom on 4/11/2025.
//

#include "RenderSystem.h"

#include <SFML/Graphics.hpp>
#include "../ECS-SFML/Components/RenderSpriteComponent.h"
#include "../ECS-SFML/Components/TransformComponent.h"

#include "Managers/EntityManager.h"

namespace ECS_SFML
{
    bool RenderSystem::Initialise(ECS::EntityManager *_entityManager,
        ECS::AccessorManager *_accessorManager,
        ECS_SFML::ResourceManager* _resourceManager,
        sf::RenderWindow *_renderWindow)
    {
        if (isInitialised)
        {
            return false;
        }

        if (_renderWindow == nullptr)
        {
            assert(false && "RenderSystem::Initialise _renderWindow is null.");
            return false;
        }
        if (_entityManager == nullptr)
        {
            assert(false && "RenderSystem::Initialise _entityManager is null.");
            return false;
        }

        entityManager = _entityManager;
        renderWindow = _renderWindow;
        resourceManager = _resourceManager;

        depthAccessor = dynamic_cast<ECS::DepthAccessor*>(_accessorManager->GetComponentAccessor<ECS::DepthComponent>());
        if (depthAccessor == nullptr)
        {
            depthAccessor = new ECS::DepthAccessor();
            depthAccessor->Initialise(100, entityManager->GetMaxCapacity());
            _accessorManager->RegisterComponentAccessor(depthAccessor);
        }
        assert(depthAccessor != nullptr && "RenderSystem::Initialise depthAccessor is null.");

        renderSpriteController = _accessorManager->GetComponentAccessor<RenderSpriteComponent>();
        if (renderSpriteController == nullptr)
        {
            renderSpriteController = _accessorManager->CreateComponentAccessor<RenderSpriteComponent>();
        }
        assert(renderSpriteController != nullptr && "RenderSystem::Initialise renderSpriteController is null.");

        transformController = _accessorManager->GetComponentAccessor<TransformComponent>();
        if (transformController == nullptr)
        {
            transformController = _accessorManager->CreateComponentAccessor<TransformComponent>();
        }
        assert(transformController != nullptr && "RenderSystem::Initialise transformController is null.");

        return true;

    }

    void RenderSystem::ProcessInternal(float _deltaSeconds)
    {
    }

    void RenderSystem::RenderInternal(float _deltaTick)
    {
        //std::vector<RenderComponent*> renderComponents;

        int currentTextureId = -1;
        sf::Sprite* currentSprite = nullptr;

        std::vector<int> depths;
        depthAccessor->GetDepths(depths);
        for (int i = 0; i < depths.size(); i++)
        {
            int depth = depths[i];
            std::vector<int> entities;
            depthAccessor->GetEntitiesAtDepth(depth, entities);

            for (int j = 0; j < entities.size(); j++)
            {
                int entity = entities[j];
                std::vector<RenderSpriteComponent*> sprites;

                sf::Vector2f pos = sf::Vector2f(0,0);
                float rot = 0.0f;
                TransformComponent* transformComp = transformController->GetComponent(entity);
                if (transformComp != nullptr)
                {
                    pos = transformComp->position;
                    rot = transformComp->angle;
                }

                renderSpriteController->GetComponents(entity, sprites);

                for (auto spriteComp : sprites)
                {
                    pos += spriteComp->position;

                    if (currentTextureId != spriteComp->textureId)
                    {
                        currentTextureId = spriteComp->textureId;
                        currentSprite = resourceManager->GetSprite(currentTextureId);
                    }

                    if (currentSprite)
                    {
                        currentSprite->setPosition(pos);
                        sf::Angle angle = sf::degrees(rot);
                        currentSprite->setRotation(angle);
                        renderWindow->draw(*currentSprite);
                    }
                }
            }

        }
    }
} // ECS_SFML