//
// Created by Radnom on 5/11/2025.
//

#include "TransformSystem.h"
#include "../Worlds/SFMLWorldContext.h"
#include "Components/ComponentManager.h"
#include "../ECS-SFML/Components/TransformComponent.h"
#include "Components/TreeComponent.h"


namespace ECS_SFML
{
    bool TransformSystem::Initialise(SFMLWorldContext *_context)
    {
        if (isInitialised)
        {
            assert(false && "TransformSystem::Initialise initialised twice.");
            return false;
        }

        transformComponent = _context->componentManager->GetComponent<TransformComponent>();
        treeComponent = _context->componentManager->GetComponent<ECS::TreeComponent>();

        isInitialised = true;
        return true;
    }

    Transform TransformSystem::GetEntityWorldTransform(int _entityId, float _frameDelta)
    {
        int transformInd = transformComponent->GetComponentIndex(_entityId);
        if (transformInd != -1)
        {
            return GetWorldTransform(transformInd, _frameDelta);
        }
        return Transform::Identity();
    }

    Transform TransformSystem::GetWorldTransform(int _transformComponentIndex, float _frameDelta)
    {
        if (_transformComponentIndex < cachedTransform.size())
        {
            return Transform::Lerp(cachedTransformPrev[_transformComponentIndex], cachedTransform[_transformComponentIndex], _frameDelta);
        }
        return transformComponent->CreateLocalTransform(_transformComponentIndex);
    }

    Transform TransformSystem::CalculateCachedTransform(int _transformCompIndex)
    {
        Transform outVal = transformComponent->CreateLocalTransform(_transformCompIndex);

        int entity = transformComponent->entityId[_transformCompIndex];
        while (treeComponent->GetParent(entity) != -1)
        {
            int transformInd = transformComponent->GetComponentIndex(entity);
            // if we don't have a transform, keep going up the tree until we find one (or reach the top).
            if (transformInd != -1)
            {
                if (transformSetThisFrame[transformInd])
                {
                    // Already cached it
                    outVal = Transform::GetAppliedTransform(cachedTransform[transformInd], outVal);
                }
                else
                {
                    // Calculate and Cache it (recursively)
                    Transform t = CalculateCachedTransform(transformInd);
                    cachedTransform[transformInd] = t;
                    transformSetThisFrame[transformInd] = true;

                    if (transformInd > cachedTransformSize)
                    {
                        cachedTransformPrev[transformInd] = cachedTransform[transformInd];
                    }

                    outVal = Transform::GetAppliedTransform(t, outVal);
                }
                break;//don't need to look higher, the recursion will take care of that
            }
        }

        cachedTransform[_transformCompIndex] = outVal;
        if (_transformCompIndex > cachedTransformSize)
        {
            cachedTransformPrev[_transformCompIndex] = cachedTransform[_transformCompIndex];
        }
        transformSetThisFrame[_transformCompIndex] = true;
        return outVal;
    }

    void TransformSystem::ProcessInternal(float _deltaTick)
    {
        cachedTransformPrev.assign(cachedTransform.begin(), cachedTransform.end());

        int prevSize = cachedTransform.size();
        int newSize = transformComponent->GetArraySize();
        if (newSize > prevSize)
        {
            cachedTransform.resize(newSize);
            cachedTransformPrev.resize(newSize);
            transformSetThisFrame.resize(newSize, false);
        }

        // This is used so we don't calculate transforms multiple times
        // because we're going through all of them, we could set it on a parent of multiple children.
        std::fill(transformSetThisFrame.begin(), transformSetThisFrame.end(), false);

        std::vector<bool>& transformCompsActive = transformComponent->active;
        for (int tCompInd = 0; tCompInd < transformCompsActive.size(); ++tCompInd)
        {
            if (transformCompsActive[tCompInd])
            {
                if (!transformSetThisFrame[tCompInd])
                {
                    CalculateCachedTransform(tCompInd);
                }
            }
        }
    }
} // ECS_SFML