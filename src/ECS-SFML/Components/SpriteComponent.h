//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "Components/IComponent.h"

#include<SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

#include "Transform.h"

namespace ECS_SFML
{
    // This is a component that has a reference to a Texture ID (loaded by ResourceManager).
    // It also has a local offset and angle.
    class SpriteComponent : public ECS::IComponent
    {
    public:
        friend class RenderSystem;

        ~SpriteComponent() override = default;

        [[nodiscard]] Transform CreateLocalTransform(int _componentIndex) const;

        [[nodiscard]] bool IsVisible(int _componentIndex) const { return visible[_componentIndex]; }
        [[nodiscard]] sf::Vector2f GetRelativePosition(int _componentIndex) const { return position[_componentIndex]; }
        [[nodiscard]] sf::Vector2f GetRelativeScale(int _componentIndex) const { return scale[_componentIndex]; }
        [[nodiscard]] float GetRelativeRotation(int _componentIndex) const { return rotation[_componentIndex]; }
        [[nodiscard]] int GetTextureIndex(int _componentIndex) const { return spriteId[_componentIndex]; }

        // This is relative, if we have any transform and/or parents.
        void SetPosition(int _componentIndex, const sf::Vector2f& _position) { position[_componentIndex] = _position; }
        // This is relative, if we have any transform and/or parents.
        void SetScale(int _componentIndex, const sf::Vector2f& _scale) { scale[_componentIndex] = _scale; }
        // This is relative, if we have any transform and/or parents.
        void SetRotation(int _componentIndex, float _rotation) { rotation[_componentIndex] = _rotation; }

        void SetTextureIndex(int _componentIndex, int _textureIndex) { spriteId[_componentIndex] = _textureIndex; }


        [[nodiscard]] const char* GetName() const override { return "SpriteComponent"; }
        [[nodiscard]] bool IsUniquePerEntity() const override { return false; }

    protected:
        bool InitialiseInternal(ECS::WorldContext* context, int _initialCapacity, int _maxCapacity) override;
        void AddComponentInternal(int _entityId, int _componentIndex) override;
        void ClearComponentAtIndexInternal(int _componentIndex) override;
        void SetCapacityInternal(int _newCapacity) override;

    protected:

        // Component data:
        std::vector<bool> visible{};

        std::vector<sf::Vector2f> position{};
        std::vector<sf::Vector2f> scale{};
        std::vector<float> rotation{};

        // This will map to a sprite in the ResourceManager.
        std::vector<int> spriteId{};

    };
} // ECS_SFML