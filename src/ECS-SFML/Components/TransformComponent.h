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
        friend class TransformSystem; // for Translation
        friend class RenderSystem; // for Depth

        // Only one Transform per Entity
        [[nodiscard]] int GetDepth(int _componentIndex) const { return depth[_componentIndex]; }

        [[nodiscard]] const sf::Vector2f& GetRelativePosition(int _componentIndex) const { return position[_componentIndex]; }
        [[nodiscard]] const sf::Vector2f& GetRelativeScale(int _componentIndex) const { return scale[_componentIndex]; }
        [[nodiscard]] float GetRelativeRotation(int _componentIndex) const { return rotation[_componentIndex]; }
        [[nodiscard]] const sf::Vector2f& GetOrigin(int _componentIndex) const { return origin[_componentIndex]; }

        void SetDepth(int _componentIndex, int _depth);
        void SetPosition(int _componentIndex, const sf::Vector2f& _position) { position[_componentIndex] = _position; translationDirty[_componentIndex] = true; }
        void SetScale(int _componentIndex, const sf::Vector2f& _scale) { scale[_componentIndex] = _scale; translationDirty[_componentIndex] = true;}
        void SetRotation(int _componentIndex, float _angle) { rotation[_componentIndex] = _angle; translationDirty[_componentIndex] = true;}
        void SetOrigin(int _componentIndex, const sf::Vector2f& _origin) { origin[_componentIndex] = _origin; translationDirty[_componentIndex] = true;}

        [[nodiscard]] const char* GetName() const override { return "TransformComponent"; }
        [[nodiscard]] bool IsUniquePerEntity() const override { return true; }

    protected:
        bool InitialiseInternal(ECS::WorldContext* context, int _initialCapacity, int _maxCapacity) override;
        void AddComponentInternal(int _entityId, int _componentIndex) override;
        void ClearComponentAtIndexInternal(int _componentIndex) override;
        void SetCapacityInternal(int _newCapacity) override;

        [[nodiscard]] inline Transform CreateLocalTransform(int _componentIndex) const
        {
            Transform transform;
            transform.setOrigin(origin[_componentIndex]);
            transform.setPosition(position[_componentIndex]);
            transform.setScale(scale[_componentIndex]);
            transform.setRotation(sf::degrees(rotation[_componentIndex]));
            return transform;
        }

    protected:
        const ECS::TreeComponent* treeComponent = nullptr;

        std::vector<sf::Vector2f> origin{};
        std::vector<sf::Vector2f> position{};
        std::vector<sf::Vector2f> scale{};
        std::vector<float> rotation{};
        std::vector<bool> translationDirty{};

        // draw order depth, lower = drawn last
        std::vector<int> depth{};
        bool depthDirty = true;
    };
} // ECS_SFML