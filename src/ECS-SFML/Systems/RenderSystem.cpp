//
// Created by Radnom on 4/11/2025.
//

#include "RenderSystem.h"

#include <SFML/Graphics.hpp>
#include "../ECS-SFML/Components/RenderSpriteComponent.h"
#include "../ECS-SFML/Components/TransformComponent.h"
#include "../Managers/ResourceManager.h"
#include "../Worlds/SFMLWorldContext.h"
#include "Components/ComponentManager.h"

#include "Worlds/WorldContext.h"

namespace ECS_SFML
{
    bool RenderSystem::Initialise(SFMLWorldContext* _context)
    {
        if (isInitialised)
        {
            assert(false && "RenderSystem::Initialise initialised twice.");
            return false;
        }

        renderWindow = _context->renderWindow;
        resourceManager = _context->resourceManager;
        transformComponent = _context->componentManager->GetComponent<TransformComponent>();
        renderSpriteComponent = _context->componentManager->GetComponent<RenderSpriteComponent>();

        assert(renderWindow != nullptr && "RenderSystem::Initialise _renderWindow is null.");
        assert(resourceManager != nullptr && "RenderSystem::Initialise resourceManager is null.");
        assert(transformComponent != nullptr && "RenderSystem::Initialise transformComponent is null.");
        assert(renderSpriteComponent != nullptr && "RenderSystem::Initialise renderSpriteComponent is null.");

        isInitialised = true;
        return true;
    }

    void RenderSystem::RenderInternal(float _deltaTween)
    {
        //std::vector<RenderComponent*> renderComponents;

        int currentTextureId = -1;
        sf::Sprite* currentSprite = nullptr;

        std::vector<int> depths = transformComponent->GetDepths();

        for (int i = 0; i < depths.size(); i++)
        {
            int depth = depths[i];
            auto[first,last] = transformComponent->GetEntitiesAtDepth(depth);

            for (auto iter = first; iter != last; ++iter)
            {
                int entity = iter->second;

                std::vector<int> spriteComps;
                renderSpriteComponent->GetComponentIndices(entity, spriteComps);

                for (int spriteComp : spriteComps)
                {
                    int textureId = renderSpriteComponent->GetTextureIndex(spriteComp);

                    if (currentTextureId != textureId)
                    {
                        currentTextureId = textureId;
                        currentSprite = resourceManager->GetSprite(currentTextureId);
                    }

                    if (currentSprite)
                    {
                        Transform t = renderSpriteComponent->GetWorldTransform(spriteComp, _deltaTween);
                        currentSprite->setPosition(t.getPosition());
                        currentSprite->setScale(t.getScale());
                        currentSprite->setOrigin(t.getOrigin());
                        currentSprite->setRotation(t.getRotation());
                        renderWindow->draw(*currentSprite);
                    }
                }
            }
        }
    }
} // ECS_SFML