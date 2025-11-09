//
// Created by Radnom on 8/11/2025.
//

#pragma once
#include "IComponent.h"
#include <vector>

namespace ECS
{
    enum ECollisionType
    {
        Ignore,
        Overlap,
        Collision,
    };

    typedef uint32_t CollisionFlags;

    class CollisionComponent : public IComponent
    {
    public:

        void SetCollisionType( int _componentIndex, ECollisionType _collisionType) { colliderResponse[_componentIndex] = _collisionType; }
        ECollisionType GetCollisionType( int _componentIndex) { return colliderResponse[_componentIndex]; }

        void AddCategoryFlags( int _componentIndex, CollisionFlags _flags) { categoryFlag[_componentIndex] |= _flags; }
        void RemoveCategoryFlags( int _componentIndex, CollisionFlags _flags) { categoryFlag[_componentIndex] &= ~_flags; }
        void SetCategoryFlags( int _componentIndex, CollisionFlags _flags) { categoryFlag[_componentIndex] = _flags; }
        CollisionFlags GetCategoryFlags( int _componentIndex) { return categoryFlag[_componentIndex]; }

        void AddCollisionFlags( int _componentIndex, CollisionFlags _flags) { colliderFlag[_componentIndex] |= _flags; }
        void RemoveCollisionFlags( int _componentIndex, CollisionFlags _flags) { colliderFlag[_componentIndex] &= ~_flags; }
        void SetCollisionFlags( int _componentIndex, CollisionFlags _flags) { colliderFlag[_componentIndex] = _flags; }
        CollisionFlags GetCollisionFlags( int _componentIndex) { return colliderFlag[_componentIndex]; }

        void AddOverlapFlags( int _componentIndex, CollisionFlags _flags) { overlapFlag[_componentIndex] |= _flags; }
        void RemoveOverlapFlags( int _componentIndex, CollisionFlags _flags) { overlapFlag[_componentIndex] &= ~_flags; }
        void SetOverlapFlags( int _componentIndex, CollisionFlags _flags) { overlapFlag[_componentIndex] = _flags; }
        CollisionFlags GetOverlapFlags( int _componentIndex) { return overlapFlag[_componentIndex]; }

        ECollisionType GetOverlapResult( int _componentIndexThis, int _componentIndexOther);

        [[nodiscard]] const char* GetName() const override { return "CollisionComponent"; }
        [[nodiscard]] bool IsUniquePerEntity() const override { return true; }

    protected:
        bool InitialiseInternal(WorldContext *context, int _initialCapacity, int _maxCapacity) override;
        void AddComponentInternal(int _entityId, int _componentIndex) override;
        void ClearComponentAtIndexInternal(int _componentIndex) override;
        void SetCapacityInternal(int _newCapacity) override;


    protected:
        // how this component can be collided with.
        // if set to ignore, nothing will detect collisions with it.
        // if set to overlap, other components can overlap, but not collide with it.
        // if set to collision, other components can collide with it.
        std::vector<ECollisionType> colliderResponse{};

        // Flags of what we 'are'
        std::vector<uint32_t> categoryFlag{};

        // Flags of what we can hit
        std::vector<uint32_t> colliderFlag{};

        // Flags of what we can overlap
        std::vector<uint32_t> overlapFlag{};

    };
} // ECS