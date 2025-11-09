//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include <map>
#include <vector>

#include "Systems/ISystem.h"

#include "SFML/System/Vector2.hpp"
#include "../ECS-SFML/Components/Transform.h"

namespace sf
{
    class RenderWindow;
}

namespace ECS
{
    class EntityManager;
}

namespace ECS_SFML
{
    struct SFMLWorldContext;
    class SpriteComponent;
    class TransformComponent;
    class TransformSystem;
    class ResourceManager;

    class RenderSystem : public ECS::ISystem
    {
    public:
        static constexpr std::string SystemName = "RenderSystem";
        bool Initialise(SFMLWorldContext* _context);

    protected:
        void ProcessInternal(float _deltaTick) override;
        void RenderInternal(float _deltaTween) override;

        inline const std::string& GetSystemName() override { return SystemName; }
        bool GetDoesProcessTick() override { return true; }
        bool GetDoesRenderTick() override { return true; }

        // process after Movement and Transform
        void GetProcessAfter(std::vector<std::string> &_outSystems) override;
        void GetRenderAfter(std::vector<std::string> &_outSystems) override {};

        // Ask for a list of active Depths, in order.
        const std::vector<int>& GetDepths();
        // Ask for a list of Entities at a specific Depth.
        // provides iterators.
        std::pair<std::multimap<int, int>::const_iterator, std::multimap<int, int>::const_iterator> GetEntitiesAtDepth(int _depth);

        void PopulateDepthEntityMap();


        Transform CalculateCachedSpriteTransform(int _spriteCompIndex, bool& _outHasTeleported);


        ResourceManager *resourceManager = nullptr;
        sf::RenderWindow* renderWindow = nullptr;
        TransformSystem* transformSystem = nullptr;

        SpriteComponent* spriteComponent = nullptr;
        TransformComponent* transformComponent = nullptr;

        std::vector<Transform> cachedSpriteTransform{};
        std::vector<Transform> cachedSpriteTransformPrev{};
        std::vector<bool> hasCachedSpriteTransform{};

        // caching and processing of the Transform's 'depth' value
        bool depthListDirty = true;
        std::vector<int> depthList{};
        bool depthMapDirty = true;
        std::multimap<int, int> depthEntityMap;

        // This is so that we can set the 'prev' as well if it's brand new
        int cachedTransformSize = 0;
    };
} // ECS_SFML