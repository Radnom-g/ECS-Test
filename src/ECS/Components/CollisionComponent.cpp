//
// Created by Radnom on 8/11/2025.
//

#include "CollisionComponent.h"

namespace ECS
{
    ECollision::ResponseType CollisionComponent::GetOverlapResult(int _componentIndexThis, int _componentIndexOther)
    {
        if (_componentIndexThis == _componentIndexOther)
            return ECollision::ResponseType::Ignore;

        ECollision::ResponseType myType = colliderResponse[_componentIndexThis];
        if (myType == ECollision::ResponseType::Ignore) { return ECollision::ResponseType::Ignore; }

        ECollision::ResponseType theirType = colliderResponse[_componentIndexOther];
        if (theirType == ECollision::ResponseType::Ignore) { return ECollision::ResponseType::Ignore; }

        // what they are.
        const CollisionFlags& categoryFlagsOther = categoryFlag[_componentIndexOther];

        // What we can collide with.
        const CollisionFlags& collideFlags = colliderFlag[_componentIndexThis];

        if (categoryFlagsOther & collideFlags)
        {
            // Both have to be allowed to collide.
            return myType & theirType & ECollision::ResponseType::Collision;
        }

        // What we can overlap with.
        const CollisionFlags& overlapFlags = overlapFlag[_componentIndexThis];

        if (categoryFlagsOther & overlapFlags)
        {
            // Both have to be allowed to overlap.
            return myType & theirType & ECollision::ResponseType::Overlap;
        }

        return ECollision::ResponseType::Ignore;
    }

    bool CollisionComponent::InitialiseInternal(WorldContext *context, int _initialCapacity, int _maxCapacity)
    {
        colliderResponse.reserve(_initialCapacity);
        categoryFlag.reserve(_initialCapacity);
        colliderFlag.reserve(_initialCapacity);
        overlapFlag.reserve(_initialCapacity);

        // std::vector<ECollision::Type> colliderResponse{};
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
        colliderResponse[_componentIndex] = ECollision::ResponseType::Ignore;
        categoryFlag[_componentIndex] = 0;
        colliderFlag[_componentIndex] = 0;
        overlapFlag[_componentIndex] = 0;
    }

    void CollisionComponent::ClearComponentAtIndexInternal(int _componentIndex)
    {
        colliderResponse[_componentIndex] = ECollision::ResponseType::Ignore;
        categoryFlag[_componentIndex] = 0;
        colliderFlag[_componentIndex] = 0;
        overlapFlag[_componentIndex] = 0;
    }

    void CollisionComponent::SetCapacityInternal(int _newCapacity)
    {
        colliderResponse.resize(_newCapacity, ECollision::ResponseType::Ignore);
        categoryFlag.resize(_newCapacity, 0);
        colliderFlag.resize(_newCapacity, 0);
        overlapFlag.resize(_newCapacity, 0);
    }
} // ECS