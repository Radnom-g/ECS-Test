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
        [[nodiscard]] bool IsChildOf(int _entityParent, int _entityChild) const;
        void GetChildren(int _entityParent, std::vector<int>& _outChildren) const;
        int GetParent(int _entityChild) const;

        void AddChild(int _entityParent, int _entityChild);
        void RemoveChild(int _entityParent, int _entityChild);

        void SetTreeDepth(int _entity, int _treeDepth);

        [[nodiscard]] const char* GetName() const override { return "TreeComponent"; }
        [[nodiscard]] bool IsUniquePerEntity() const override { return true; }
        [[nodiscard]] bool CanInterpolate() const override { return false; }

    protected:
        bool InitialiseInternal(WorldContext *context, int _initialCapacity, int _maxCapacity) override;
        void AddComponentInternal(int _entityId, int _componentIndex) override;
        void ClearComponentAtIndexInternal(int _componentIndex) override;
        void SetCapacityInternal(int _newCapacity) override;
        void ProcessPhysicsInternal(float _delta) override {}; // no interpolation

    protected:
        // component index to parent Entity ID.
        std::vector<int> parentId{};
        // how far down the tree are we. 0 = top-level parent. 1 = child, 2 = grandchild etc.
        std::vector<int> treeDepth{};
        // Maps component index to child entity IDs
        std::map<int, std::vector<int>> childrenIds;


    };
} // ECS