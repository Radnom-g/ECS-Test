//
// Created by Radnom on 6/11/2025.
//

#include "RenderSpriteComponent.h"

#include "TransformComponent.h"
#include "Components/ComponentManager.h"
#include "Components/TreeComponent.h"
#include "Worlds/WorldContext.h"

namespace ECS_SFML
{
    ECS_SFML::Transform RenderSpriteComponent::GetWorldTransform(int _componentIndex, float _frameDelta)
    {
        Transform local_t;
        local_t.setPosition(GetRelativePosition(_componentIndex, _frameDelta));
        local_t.setScale(GetRelativeScale(_componentIndex, _frameDelta));
        local_t.setRotation(sf::degrees(GetRelativeRotation(_componentIndex, _frameDelta)));

        if (transformComponent->HasComponent(entityId[_componentIndex]))
        {
            Transform parent_t = transformComponent->GetWorldTransform(_componentIndex, _frameDelta);
            local_t = Transform::GetAppliedTransform(parent_t, local_t);
        }

        return local_t;
    }

    sf::Vector2f RenderSpriteComponent::GetRelativePosition(int _componentIndex, float _frameDelta) const
    {
        const sf::Vector2f diff = position[_componentIndex] - positionPrev[_componentIndex];
        return positionPrev[_componentIndex] + (diff * _frameDelta);
    }

    sf::Vector2f RenderSpriteComponent::GetRelativeScale(int _componentIndex, float _frameDelta) const
    {
        const sf::Vector2f diff = scale[_componentIndex] - scalePrev[_componentIndex];
        return scalePrev[_componentIndex] + (diff * _frameDelta);
    }

    float RenderSpriteComponent::GetRelativeRotation(int _componentIndex, float _frameDelta) const
    {
        const float diff = rotation[_componentIndex] - rotationPrev[_componentIndex];
        return rotationPrev[_componentIndex] + (diff * _frameDelta);
    }

    int RenderSpriteComponent::GetTextureIndex(int _componentIndex) const
    {
        return textureId[_componentIndex];
    }

    void RenderSpriteComponent::SetPosition(int _componentIndex, const sf::Vector2f &_position)
    {
        position[_componentIndex] = _position;
    }

    void RenderSpriteComponent::SetScale(int _componentIndex, const sf::Vector2f &_scale)
    {
        scale[_componentIndex] = _scale;
    }

    void RenderSpriteComponent::SetRotation(int _componentIndex, float _rotation)
    {
        rotation[_componentIndex] = _rotation;
    }

    void RenderSpriteComponent::SetTextureIndex(int _componentIndex, int _textureIndex)
    {
        textureId[_componentIndex] = _textureIndex;
    }

    bool RenderSpriteComponent::InitialiseInternal(ECS::WorldContext *context, int _initialCapacity, int _maxCapacity)
    {
        transformComponent = context->componentManager->GetComponent<TransformComponent>();
        position.reserve(_initialCapacity);
        positionPrev.reserve(_initialCapacity);
        scale.reserve(_initialCapacity);
        scalePrev.reserve(_initialCapacity);
        rotation.reserve(_initialCapacity);
        rotationPrev.reserve(_initialCapacity);
        textureId.reserve(_initialCapacity);
        return true;
    }

    void RenderSpriteComponent::AddComponentInternal(int _entityId, int _componentIndex)
    {
        position[_componentIndex] = sf::Vector2f(0, 0);
        positionPrev[_componentIndex] = sf::Vector2f(0, 0);
        scale[_componentIndex] = sf::Vector2f(1, 1);
        scalePrev[_componentIndex] = sf::Vector2f(0, 0);
        rotation[_componentIndex] = 0;
        rotationPrev[_componentIndex] = 0;
        textureId[_componentIndex] = -1;
    }

    void RenderSpriteComponent::ClearComponentAtIndexInternal(int _componentIndex)
    {
        position[_componentIndex] = sf::Vector2f(0, 0);
        positionPrev[_componentIndex] = sf::Vector2f(0, 0);
        scale[_componentIndex] = sf::Vector2f(1, 1);
        scalePrev[_componentIndex] = sf::Vector2f(0, 0);
        rotation[_componentIndex] = 0;
        rotationPrev[_componentIndex] = 0;
        textureId[_componentIndex] = -1;
    }

    void RenderSpriteComponent::SetCapacityInternal(int _newCapacity)
    {
        position.resize(_newCapacity, sf::Vector2f(0, 0));
        positionPrev.resize(_newCapacity, sf::Vector2f(0, 0));
        scale.resize(_newCapacity, sf::Vector2f(1, 1));
        scalePrev.resize(_newCapacity, sf::Vector2f(0, 0));
        rotation.resize(_newCapacity, 0.0f);
        rotationPrev.resize(_newCapacity, 0.0f);
        textureId.resize(_newCapacity, -1);
    }

    void RenderSpriteComponent::ProcessPhysicsInternal(float _delta)
    {
        positionPrev.assign(position.begin(), position.end());
        scalePrev.assign(scale.begin(), scale.end());
        rotationPrev.assign(rotation.begin(), rotation.end());

    }
}
