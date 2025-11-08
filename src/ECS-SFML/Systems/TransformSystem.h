//
// Created by Radnom on 5/11/2025.
//

#pragma once
#include <vector>

#include "Systems/ISystem.h"
#include "../ECS-SFML/Components/Transform.h"

namespace ECS
{
    class EntityManager;
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

        [[nodiscard]] Transform GetEntityWorldTransform(int _entityId );
        [[nodiscard]] Transform GetWorldTransform(int _transformComponentIndex );

        // to be called in process tick, these setters will go DOWN the tree and update cached transforms.
        // Intended for systems that process after TransformSystem (most of them).

        // This one is fairly expensive as we have to work backwards to calculate the local.
        // Better to use Move/Rotate/ScaleEntity/Transform if possible.
        void SetWorldTransform(int _transformInd, const Transform& _newTransform);
        void MoveTransform(int _transformInd, const sf::Vector2f& _movement);
        void RotateTransform(int _transformInd, float _rotation);
        void ScaleTransform(int _transformInd, const sf::Vector2f& _scale);
        void MoveEntity(int _entityId, const sf::Vector2f& _movement);
        void RotateEntity(int _entityId, float _rotation);
        void ScaleEntity(int _entityId, const sf::Vector2f& _scale);

        // Sets prev cache to current cache to stop the entity from lerping.
        void MarkEntityAsTeleported(int _entityId);
        bool HasEntityTeleportedThisFrame(int _entityId);
        bool HasTransformTeleportedThisFrame(int _transformComponentIndex);

    protected:
        // Get the transform's parent transform by going up the Tree to find parent Transforms.
        // This will also cache any uncached transforms on its way.
        // True if it did anything.
        bool LocalToWorldTransform(int _entityId,  Transform& _outLocalTransformToMakeWorld);

        // from an entity, affect the cached transforms of child entities.
        void CalculateCachedTransformOfChildren(int _entityId, const Transform& _parentTransform);

        void ProcessInternal(float _deltaTick) override;
        void RenderInternal(float _deltaTween) override {}

        inline const char* GetSystemName() override { return "TransformSystem"; }
        bool GetDoesProcessTick() override { return true; }
        bool GetDoesRenderTick() override { return false; }

        void SetTransformCache(int _transformIndex, const Transform& _worldTransform);

        void ResizeCache();

        ECS::EntityManager* entityManager = nullptr;
        TransformComponent* transformComponent = nullptr;
        ECS::TreeComponent* treeComponent = nullptr;

        std::vector<Transform> cachedTransform{};
        std::vector<Transform> cachedTransformPrev{};

        // this is only used in ProcessTick but might as well keep it in memory
        std::vector<bool> transformNeedsCache;
        // To make sure we don't have one frame of something snapping to position
        std::vector<bool> transformPrevCacheSet;

        // This is so that we can set the 'prev' as well if it's brand new
        int cachedTransformSize = 0;
    };
} // ECS_SFML