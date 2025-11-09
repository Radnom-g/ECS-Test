//
// Created by Radnom on 6/11/2025.
//

#include "TreeComponent.h"

namespace ECS
{
    void TreeComponent::AddChild(int _entityParent, int _entityChild)
    {
        int parCompInd = GetComponentIndex(_entityParent);
        if ( parCompInd == -1)
        {
            parCompInd = AddComponent(_entityParent);
        }

        int childCompInd = GetComponentIndex(_entityChild);
        if (childCompInd == -1)
        {
            childCompInd = AddComponent(_entityChild);
        }

        if (parentId[childCompInd] == _entityParent)
            return; // assume this is already set up.

        if (parentId[childCompInd] != -1)
        {
            // already assigned to a different parent?
            RemoveChild(parentId[childCompInd], _entityChild);
        }

        // Set parent of child.
        parentId[childCompInd] = _entityParent;

        // check that it doesn't already have this child.
       IndexList& childList = childrenIds[parCompInd];

        if (childList.find_index(_entityChild) == -1)
        {
            childList.push_back(_entityChild);
        }
        SetTreeDepth(_entityChild, treeDepth[parCompInd]);
    }

    void TreeComponent::RemoveChild(int _entityParent, int _entityChild)
    {
        const int parCompInd = GetComponentIndex(_entityParent);
        const int childCompInd = GetComponentIndex(_entityChild);

        // Clear parent of child.
        if (parentId[childCompInd] == _entityParent)
        {
            parentId[childCompInd] = -1;
            SetTreeDepth(childCompInd, 0);
        }

        // Remove it from the parent.
        IndexList& childList = childrenIds[parCompInd];
        childList.remove_matches(_entityChild);
    }

    void TreeComponent::SetTreeDepth(int _entity, int _treeDepth)
    {
        const int compInd = GetComponentIndex(_entity);

        if (compInd != -1)
        {
            treeDepth[GetComponentIndex(_entity)] = _treeDepth;

            IndexList& childList = childrenIds[compInd];
            for (auto iter : childList)
            {
                // Recursively set child depth.
                if (iter != -1)
                {
                    SetTreeDepth(iter, _treeDepth+1);
                }
            }
        }
    }

    bool TreeComponent::InitialiseInternal(WorldContext *context, int _initialCapacity, int _maxCapacity)
    {
        parentId.reserve(_initialCapacity);
        treeDepth.reserve(_initialCapacity);
        childrenIds.reserve(_initialCapacity);
        return true;
    }

    void TreeComponent::AddComponentInternal(int _entityId, int _componentIndex)
    {
        parentId[_componentIndex] = -1;
        treeDepth[_componentIndex] = 0;
        // should be set?
        childrenIds[_componentIndex].clear();
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
        childrenIds[_componentIndex].clear();
    }

    void TreeComponent::SetCapacityInternal(int _newCapacity)
    {
        parentId.resize(_newCapacity, -1);
        treeDepth.resize(_newCapacity, 0);
        childrenIds.resize(_newCapacity, IndexList());
    }

}
