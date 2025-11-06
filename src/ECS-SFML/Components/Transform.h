//
// Created by Radnom on 6/11/2025.
//

#pragma once
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/System/Vector2.hpp"

namespace ECS_SFML
{
    struct Transform : public sf::Transformable
    {
        void SetToIdentity()
        {
            setPosition(sf::Vector2f(0,0));
            setRotation(sf::degrees(0));
            setScale(sf::Vector2f(1,1));
            setOrigin(sf::Vector2f(0,0));
        }

        [[nodiscard]] inline static Transform Identity()
        {
            Transform ret;
            ret.setPosition(sf::Vector2f(0,0));
            ret.setRotation(sf::degrees(0));
            ret.setScale(sf::Vector2f(1,1));
            ret.setOrigin(sf::Vector2f(0,0));
            return ret;
        }

        [[nodiscard]] inline static Transform Lerp(const Transform& _a, const Transform& _b, float t)
        {
            Transform result;
            result.setPosition(_a.getPosition() + (_b.getPosition() - _a.getPosition()) * t);
            result.setRotation(_a.getRotation() + (_b.getRotation() - _a.getRotation()) * t);
            result.setScale(_a.getScale() + (_b.getScale() - _a.getScale()) * t);
            return result;
        }

        [[nodiscard]] inline static Transform GetAppliedTransform(const Transform& _parent, const Transform& _child)
        {
            Transform ret = _child;
            ret.move(_parent.getPosition());
            ret.rotate(_parent.getRotation());
            ret.scale(_parent.getScale());
            return (ret);
        }
    };
} // ECS_SFML