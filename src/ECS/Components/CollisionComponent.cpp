//
// Created by Radnom on 8/11/2025.
//

#include "CollisionComponent.h"

namespace ECS
{
    ECollisionType CollisionComponent::GetOverlapResult(int _componentIndexThis, int _componentIndexOther)
    {
        ECollisionType myType = colliderResponse[_componentIndexThis];
        if (myType == ECollisionType::Ignore) { return ECollisionType::Ignore; }

        ECollisionType theirType = colliderResponse[_componentIndexOther];
        if (theirType == ECollisionType::Ignore) { return ECollisionType::Ignore; }

        // what they are.
        CollisionFlags categoryFlagsOther = categoryFlag[_componentIndexOther];

        // What we can collide with.
        CollisionFlags collideFlags = colliderFlag[_componentIndexThis];
        if (categoryFlagsOther & collideFlags)
        {
            return std::min(ECollisionType::Collision, theirType); // will be degraded to overlap
        }

        CollisionFlags overlapFlags = overlapFlag[_componentIndexThis];
        if (categoryFlagsOther & collideFlags)
        {
            return ECollisionType::Overlap;
        }

        return ECollisionType::Ignore;
    }

    bool CollisionComponent::InitialiseInternal(WorldContext *context, int _initialCapacity, int _maxCapacity)
    {
        colliderResponse.reserve(_initialCapacity);
        categoryFlag.reserve(_initialCapacity);
        colliderFlag.reserve(_initialCapacity);
        overlapFlag.reserve(_initialCapacity);

        // std::vector<ECollisionType> colliderResponse{};
        //
        // // Flags of what we 'are'
        // std::vector<uint32_t> categoryFlag{};
        //
        // // Flags of what we can hit
        // std::vector<uint32_t> colliderFlag{};
        //
        // // Flags of what we can overlap
        // std::vector<uint32_t> overlapFlag{};
        return true;
    }

    void CollisionComponent::AddComponentInternal(int _entityId, int _componentIndex)
    {
        colliderResponse[_componentIndex] = ECollisionType::Ignore;
        categoryFlag[_componentIndex] = 0;
        colliderFlag[_componentIndex] = 0;
        overlapFlag[_componentIndex] = 0;
    }

    void CollisionComponent::ClearComponentAtIndexInternal(int _componentIndex)
    {
        colliderResponse[_componentIndex] = ECollisionType::Ignore;
        categoryFlag[_componentIndex] = 0;
        colliderFlag[_componentIndex] = 0;
        overlapFlag[_componentIndex] = 0;
    }

    void CollisionComponent::SetCapacityInternal(int _newCapacity)
    {
        colliderResponse.resize(_newCapacity, ECollisionType::Ignore);
        categoryFlag.resize(_newCapacity, 0);
        colliderFlag.resize(_newCapacity, 0);
        overlapFlag.resize(_newCapacity, 0);
    }
} // ECS