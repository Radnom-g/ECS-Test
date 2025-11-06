//
// Created by Radnom on 6/11/2025.
//

#include "TreeComponent.h"

namespace ECS
{

    bool TreeComponent::IsChildOf(int _entityParent, int _entityChild) const
    {
        return (EntityIdToComponentIndex(_entityChild) == _entityParent);
    }

    void TreeComponent::GetChildren(int _entityParent, std::vector<int> &_outChildren) const
    {
        if (childrenIds.find(EntityIdToComponentIndex(_entityParent)) != childrenIds.end())
        {
            _outChildren = (childrenIds.at(EntityIdToComponentIndex(_entityParent)));
        }
    }

    int TreeComponent::GetParent(int _entityChild) const
    {
        if (!HasComponent(_entityChild))
            return -1;

        return (parentId[EntityIdToComponentIndex(_entityChild)]);
    }

    void TreeComponent::AddChild(int _entityParent, int _entityChild)
    {
        const int parCompInd = EntityIdToComponentIndex(_entityParent);
        const int childCompInd = EntityIdToComponentIndex(_entityChild);

        if (parentId[childCompInd] == _entityParent)
            return; // assume this is already set up.

        if (parentId[childCompInd] != -1)
        {
            // already assigned to a different parent?
            RemoveChild(parentId[childCompInd], _entityChild);
        }

        // Set parent of child.
        parentId[childCompInd] = _entityParent;

        if (childrenIds.find(EntityIdToComponentIndex(_entityParent)) == childrenIds.end())
        {
            // We don't have any Children in this Component.
            std::vector<int> newChildList = std::vector<int>();
            newChildList.push_back(_entityChild);
            childrenIds.emplace(EntityIdToComponentIndex(_entityParent), newChildList);
            return;
        }

        // We have Children - check that it doesn't already have this child.
        std::vector<int>& childList = childrenIds[EntityIdToComponentIndex(_entityChild)];
        for (auto i : childList)
        {
            // See if we already have it.
            if (i == _entityChild)
                return;
        }

        childList.push_back(_entityChild);

        SetTreeDepth(_entityChild, treeDepth[parCompInd]);
    }

    void TreeComponent::RemoveChild(int _entityParent, int _entityChild)
    {
        const int parCompInd = EntityIdToComponentIndex(_entityParent);
        const int childCompInd = EntityIdToComponentIndex(_entityChild);

        // Clear parent of child.
        if (parentId[childCompInd] == _entityParent)
        {
            parentId[childCompInd] = -1;
            SetTreeDepth(childCompInd, 0);
        }

        if (childrenIds.find(EntityIdToComponentIndex(_entityParent)) == childrenIds.end())
        {
            // We don't have any Children in this Component.
            return;
        }

        std::vector<int>& childList = childrenIds[EntityIdToComponentIndex(_entityChild)];
        for (auto iter = childList.begin(); iter != childList.end(); ++iter)
        {
            // Find and remove it.
            if (*iter == _entityChild)
            {
                childList.erase(iter);
                break;
            };
        }
        if (childList.empty())
        {
            childrenIds.erase(EntityIdToComponentIndex(_entityChild));
        }

    }

    void TreeComponent::SetTreeDepth(int _entity, int _treeDepth)
    {
        treeDepth[EntityIdToComponentIndex(_entity)] = _treeDepth;

        std::vector<int>& childList = childrenIds[EntityIdToComponentIndex(_entity)];
        for (auto iter = childList.begin(); iter != childList.end(); ++iter)
        {
            // Recursively set child depth.
            SetTreeDepth(*iter, _treeDepth+1);
        }
    }

    bool TreeComponent::InitialiseInternal(WorldContext *context, int _initialCapacity, int _maxCapacity)
    {
        parentId.reserve(_initialCapacity);
        treeDepth.reserve(_initialCapacity);
        childrenIds.clear();
        return true;
    }

    void TreeComponent::AddComponentInternal(int _entityId, int _componentIndex)
    {
        parentId[_componentIndex] = -1;
        treeDepth[_componentIndex] = 0;
        // No need to set childrenIds until a child is added.
    }

    void TreeComponent::ClearComponentAtIndexInternal(int _componentIndex)
    {
        // Make sure we also remove it from its Parent.
        if (parentId[_componentIndex] != -1)
        {
            RemoveChild(parentId[_componentIndex], entityId[_componentIndex]);
        }

        parentId[_componentIndex] = -1;
        treeDepth[_componentIndex] = 0;

        if (childrenIds.find(_componentIndex) != childrenIds.end())
        {
            childrenIds.erase(_componentIndex);
        }
    }

    void TreeComponent::SetCapacityInternal(int _newCapacity)
    {
        parentId.resize(_newCapacity, -1);
        treeDepth.resize(_newCapacity, 0);
    }

}
