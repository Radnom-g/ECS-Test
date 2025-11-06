//
// Created by Radnom on 5/11/2025.
//

#pragma once
#include <vector>

#include "Systems/ISystem.h"
#include "../ECS-SFML/Components/Transform.h"

namespace ECS
{
    class TreeComponent;
}

namespace ECS_SFML
{
    struct SFMLWorldContext;
    class TransformComponent;

    // A system for getting a Transform out of an entity's hierarchy.
    class TransformSystem : public ECS::ISystem
    {
    public:
        bool Initialise(SFMLWorldContext* _context);

        [[nodiscard]] Transform GetEntityWorldTransform(int _entityId, float _frameDelta);
        [[nodiscard]] Transform GetWorldTransform(int _transformComponentIndex, float _frameDelta);

    protected:
        Transform CalculateCachedTransform(int _transformCompIndex);

        void ProcessInternal(float _deltaTick) override;
        void RenderInternal(float _deltaTween) override {}

        inline const char* GetSystemName() override { return "TransformSystem"; }
        bool GetDoesProcessTick() override { return true; }
        bool GetDoesRenderTick() override { return false; }

        TransformComponent* transformComponent = nullptr;
        ECS::TreeComponent* treeComponent = nullptr;

        std::vector<Transform> cachedTransform{};
        std::vector<Transform> cachedTransformPrev{};

        // this is only used in ProcessTick but might as well keep it in memory
        std::vector<bool> transformSetThisFrame;

        // This is so that we can set the 'prev' as well if it's brand new
        int cachedTransformSize = 0;
    };
} // ECS_SFML