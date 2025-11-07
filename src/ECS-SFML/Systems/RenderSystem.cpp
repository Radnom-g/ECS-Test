//
// Created by Radnom on 4/11/2025.
//

#include "RenderSystem.h"

#include <SFML/Graphics.hpp>

#include "TransformSystem.h"
#include "../ECS-SFML/Components/SpriteComponent.h"
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
        transformSystem = _context->transformSystem;
        transformComponent = _context->componentManager->GetComponent<TransformComponent>();
        spriteComponent = _context->componentManager->GetComponent<SpriteComponent>();

        assert(renderWindow != nullptr && "RenderSystem::Initialise _renderWindow is null.");
        assert(resourceManager != nullptr && "RenderSystem::Initialise resourceManager is null.");
        assert(transformSystem != nullptr && "RenderSystem::Initialise transformSystem is null.");
        assert(spriteComponent != nullptr && "RenderSystem::Initialise spriteComponent is null.");

        isInitialised = true;
        return true;
    }

    void RenderSystem::ProcessInternal(float _deltaTick)
    {
        cachedSpriteTransformPrev.assign(cachedSpriteTransform.begin(), cachedSpriteTransform.end());

        int prevSize = cachedSpriteTransform.size();
        int newSize = spriteComponent->GetArraySize();
        if (newSize > prevSize)
        {
            cachedSpriteTransform.resize(newSize);
            cachedSpriteTransformPrev.resize(newSize);
            hasCachedSpriteTransform.resize(newSize, false);
        }

        std::vector<bool>& spriteCompsActive = spriteComponent->visible;
        for (int sCompInd = 0; sCompInd < spriteCompsActive.size(); ++sCompInd)
        {
            if (spriteCompsActive[sCompInd])
            {
                cachedSpriteTransform[sCompInd] = CalculateCachedSpriteTransform(sCompInd);
                if (!hasCachedSpriteTransform[sCompInd])
                {
                    cachedSpriteTransformPrev[sCompInd] = cachedSpriteTransform[sCompInd];
                }
                hasCachedSpriteTransform[sCompInd] = true;
            }
            else
            {
                hasCachedSpriteTransform[sCompInd] = false;
            }
        }
    }

    void RenderSystem::RenderInternal(float _deltaTween)
    {
        //std::vector<RenderComponent*> renderComponents;

        int currentTextureId = -1;
        sf::Sprite* currentSprite = nullptr;

        std::vector<int> depths = GetDepths();

        for (int i = 0; i < depths.size(); i++)
        {
            int depth = depths[i];
            auto[first,last] = GetEntitiesAtDepth(depth);

            for (auto iter = first; iter != last; ++iter)
            {
                int entity = iter->second;

                std::vector<int> spriteComps;
                spriteComponent->GetComponentIndices(entity, spriteComps);

                for (int spriteComp : spriteComps)
                {
                    int textureId = spriteComponent->GetTextureIndex(spriteComp);

                    if (currentTextureId != textureId)
                    {
                        currentTextureId = textureId;
                        currentSprite = resourceManager->GetSprite(currentTextureId);
                    }

                    if (currentSprite)
                    {
                        // Transform tEnt = transformSystem->GetEntityWorldTransform(entity, _deltaTween);
                        // Transform tSpr = spriteComponent->CreateLocalTransform(spriteComp, _deltaTween);
                        // tSpr = Transform::GetAppliedTransform(tEnt, tSpr);

                        // Can't render without a cached sprite transform.
                        if (hasCachedSpriteTransform.size() > spriteComp && hasCachedSpriteTransform[spriteComp])
                        {
                            Transform tSpr = Transform::Lerp(cachedSpriteTransformPrev[spriteComp], cachedSpriteTransform[spriteComp], _deltaTween);

                            // temp, not lerping
                            //Transform tSpr = cachedSpriteTransform[spriteComp];
                            // currentSprite->setPosition(tSpr.getPosition());
                            // currentSprite->setScale(tSpr.getScale());
                            // currentSprite->setOrigin(tSpr.getOrigin());
                            // currentSprite->setRotation(tSpr.getRotation());

                            // TODO: Don't do this every time!
                            tSpr.setOrigin(sf::Vector2f(static_cast<float>(currentSprite->getTexture().getSize().x) * 0.5f, static_cast<float>( currentSprite->getTexture().getSize().y) * 0.5f ));

                            sf::RenderStates newStates(tSpr.getTransform());
                            renderWindow->draw(*currentSprite, newStates);
                        }
                    }
                }
            }
        }
    }

    const std::vector<int> & RenderSystem::GetDepths()
    {
        if (depthListDirty)
        {
            if (depthMapDirty)
            {
                PopulateDepthEntityMap();
            }

            depthList.clear();

            for(  auto it = depthEntityMap.begin(), end = depthEntityMap.end(); it != end; it = depthEntityMap.upper_bound(it->first))
            {
                depthList.push_back(it->first);
            }
            depthListDirty = false;
        }

        return depthList;
    }

    std::pair<std::multimap<int, int>::const_iterator, std::multimap<int, int>::const_iterator> RenderSystem::
    GetEntitiesAtDepth(int _depth)
    {
        if (depthMapDirty)
        {
            PopulateDepthEntityMap();
        }

        return depthEntityMap.equal_range(_depth);
    }

    void RenderSystem::PopulateDepthEntityMap()
    {
        depthEntityMap.clear();
        for (int i = 0; i < transformComponent->depth.size(); i++)
        {
            if (transformComponent->entityId[i] != -1)
            {
                depthEntityMap.emplace(transformComponent->depth[i], i);
            }
        }
        depthMapDirty = false;
    }

    Transform RenderSystem::CalculateCachedSpriteTransform(int _spriteCompIndex)
    {
        int entity = spriteComponent->entityId[_spriteCompIndex];

        Transform tEnt = transformSystem->GetEntityWorldTransform(entity, 1);
        Transform tSpr = spriteComponent->CreateLocalTransform(_spriteCompIndex);
        tSpr = Transform::GetAppliedTransform(tEnt, tSpr);
        return tSpr;
    }
} // ECS_SFML