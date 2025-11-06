//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "../../ECS/Components/IComponent.h"
#include <SFML/System/Vector2.hpp>

#include "Transform.h"

namespace ECS_SFML
{
    class TransformSystem;
}

namespace ECS
{
    class TreeComponent;
}

namespace ECS_SFML
{
    class Transform;

    class TransformComponent: public ECS::IComponent
    {

    public:
        friend class TransformSystem;

        // Only one Transform per Entity
        [[nodiscard]] int GetDepth(int _componentIndex) const;

        [[nodiscard]] sf::Vector2f GetRelativePosition(int _componentIndex, float _frameDelta) const;
        [[nodiscard]] sf::Vector2f GetRelativeScale(int _componentIndex, float _frameDelta) const;
        [[nodiscard]] float GetRelativeRotation(int _componentIndex, float _frameDelta) const;

        void SetDepth(int _componentIndex, int _depth);
        void SetPosition(int _componentIndex, sf::Vector2f _position);
        void SetScale(int _componentIndex, sf::Vector2f _scale);
        void SetRotation(int _componentIndex, float _angle);

        // Ask for a list of active Depths, in order.
        const std::vector<int>& GetDepths();
        // Ask for a list of Entities at a specific Depth.
        // provides iterators.
        std::pair<std::multimap<int, int>::const_iterator, std::multimap<int, int>::const_iterator> GetEntitiesAtDepth(int _depth);


        [[nodiscard]] const char* GetName() const override { return "TransformComponent"; }
        [[nodiscard]] bool IsUniquePerEntity() const override { return false; }
        [[nodiscard]] bool CanInterpolate() const override { return true; }

    protected:
        bool InitialiseInternal(ECS::WorldContext* context, int _initialCapacity, int _maxCapacity) override;
        void AddComponentInternal(int _entityId, int _componentIndex) override;
        void ClearComponentAtIndexInternal(int _componentIndex) override;
        void SetCapacityInternal(int _newCapacity) override;
        void ProcessPhysicsInternal(float _delta) override;

        [[nodiscard]] Transform CreateLocalTransform(int _componentIndex) const;

        void PopulateDepthEntityMap();

    protected:
        const ECS::TreeComponent* treeComponent = nullptr;

        std::vector<sf::Vector2f> position{};
        std::vector<sf::Vector2f> scale{};
        std::vector<float> rotation{};

        // draw order depth, lower = drawn last
        std::vector<int> depth{};
        bool depthListDirty = true;
        std::vector<int> depthList{};
        bool depthMapDirty = true;
        std::multimap<int, int> depthEntityMap;
    };
} // ECS_SFML