//
// Created by Radnom on 3/11/2025.
//

#pragma once
#include "IComponent.h"
#include <vector>

namespace ECS
{
    // This is a Component that connects this Component to other Components in a hierarchy.
    // This doesn't have any access to the other components, so we need to ensure we don't do anything stupid
    // ( like add itself to its children, or create an inheritance loop) in the Controller level.
    class TreeComponent : public IComponent
    {
    public:
        // TEMP: Remove once TreeSystem is ready.
        std::vector<int>& GetTreeDepthVector() { return treeDepth; }

        [[nodiscard]] bool IsChildOf(int _entityParent, int _entityChild) const;
        bool GetChildren(int _entityParent, std::vector<int>& _outChildren) const;
        int GetParent(int _entityChild) const;

        void AddChild(int _entityParent, int _entityChild);
        void RemoveChild(int _entityParent, int _entityChild);

        [[nodiscard]] int GetTreeDepth(int _entity) const
        {
            int compInd = GetComponentIndex(_entity);
            if (compInd == -1)
            {
                return 0;
            }
            return treeDepth[compInd];
        }
        void SetTreeDepth(int _entity, int _treeDepth);

        [[nodiscard]] const char* GetName() const override { return "TreeComponent"; }
        [[nodiscard]] bool IsUniquePerEntity() const override { return true; }

    protected:
        bool InitialiseInternal(WorldContext *context, int _initialCapacity, int _maxCapacity) override;
        void AddComponentInternal(int _entityId, int _componentIndex) override;
        void ClearComponentAtIndexInternal(int _componentIndex) override;
        void SetCapacityInternal(int _newCapacity) override;

    protected:
        // component index to parent Entity ID.
        std::vector<int> parentId{};
        // how far down the tree are we. 0 = top-level parent. 1 = child, 2 = grandchild etc.
        std::vector<int> treeDepth{};
        // Maps component index to child entity IDs
        std::map<int, std::vector<int>> childrenIds;


    };
} // ECS