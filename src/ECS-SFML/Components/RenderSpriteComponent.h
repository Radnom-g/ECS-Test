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
    class RenderSpriteComponent : public ECS::IComponent
    {
    public:
        ~RenderSpriteComponent() override = default;

        [[nodiscard]] Transform CreateLocalTransform(int _componentIndex, float _frameDelta) const;

        [[nodiscard]] sf::Vector2f GetRelativePosition(int _componentIndex, float _frameDelta) const;

        [[nodiscard]] sf::Vector2f GetRelativeScale(int _componentIndex, float _frameDelta) const;

        [[nodiscard]] float GetRelativeRotation(int _componentIndex, float _frameDelta) const;

        [[nodiscard]] int GetTextureIndex(int _componentIndex) const;

        // This is relative, if we have any transform and/or parents.
        void SetPosition(int _componentIndex, const sf::Vector2f& _position);
        // This is relative, if we have any transform and/or parents.
        void SetScale(int _componentIndex, const sf::Vector2f& _scale);
        // This is relative, if we have any transform and/or parents.
        void SetRotation(int _componentIndex, float _rotation);

        void SetTextureIndex(int _componentIndex, int _textureIndex);


        [[nodiscard]] const char* GetName() const override { return "RenderSpriteComponent"; }
        [[nodiscard]] bool IsUniquePerEntity() const override { return false; }
        [[nodiscard]] bool CanInterpolate() const override { return true; }

    protected:
        bool InitialiseInternal(ECS::WorldContext* context, int _initialCapacity, int _maxCapacity) override;
        void AddComponentInternal(int _entityId, int _componentIndex) override;
        void ClearComponentAtIndexInternal(int _componentIndex) override;
        void SetCapacityInternal(int _newCapacity) override;
        void ProcessPhysicsInternal(float _delta) override;

    protected:

        // Component data:
        std::vector<sf::Vector2f> position{};
        std::vector<sf::Vector2f> positionPrev{};
        std::vector<sf::Vector2f> scale{};
        std::vector<sf::Vector2f> scalePrev{};
        std::vector<float> rotation{};
        std::vector<float> rotationPrev{};

        std::vector<int> textureId{};

    };
} // ECS_SFML