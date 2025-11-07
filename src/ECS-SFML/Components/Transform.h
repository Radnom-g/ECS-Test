//
// Created by Radnom on 6/11/2025.
//

#pragma once
#include <cmath>
#include <string>
#include <sstream>

#include "SFML/Graphics/Transformable.hpp"
#include "SFML/System/Vector2.hpp"

namespace ECS_SFML
{
    inline bool IsEmpty(const sf::Vector2f& _vec) { return _vec.x == 0 && _vec.y == 0; }

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

        static float ShortAngleDist(float _from, float _to)
        {
            float delta = std::fmod(_to - _from, sf::priv::tau);
            return std::fmod(2.0f * delta, sf::priv::tau) - delta;
        }

        static float AngleLerp(float _a, float _b, float _t)
        {
            return _a + ShortAngleDist(_a, _b) * _t;
        }

        [[nodiscard]] inline static Transform Lerp(const Transform& _a, const Transform& _b, float t)
        {
            Transform result;
            result.setOrigin(_a.getOrigin() + (_b.getOrigin() - _a.getOrigin()) * t);
            result.setPosition(_a.getPosition() + (_b.getPosition() - _a.getPosition()) * t);

            // Bad lerp!
            //result.setRotation(_a.getRotation() + (_b.getRotation() - _a.getRotation()) * t);

            // Good lerp!
            result.setRotation(sf::radians(AngleLerp(_a.getRotation().asRadians(), _b.getRotation().asRadians(), t )));
            result.setScale(_a.getScale() + (_b.getScale() - _a.getScale()) * t);
            return result;
        }

        inline static std::string ToString(const Transform& _transform)
        {
            std::stringstream strstr;
            strstr << "Origin<" << _transform.getOrigin().x << "," << _transform.getOrigin().y << ">";
            strstr << "Pos<" << _transform.getPosition().x << "," << _transform.getPosition().y << ">";
            strstr << ", Scale<" << _transform.getScale().x << "," << _transform.getScale().y << ">";
            strstr << ", Rot<" << _transform.getRotation().asDegrees() << ">";
            return strstr.str();
        }

        [[nodiscard]] inline static Transform GetAppliedTransform(const Transform& _parent, const Transform& _child)
        {
            sf::Transform world = _parent.getTransform() * _child.getTransform();
            //world.combine(_parent.getTransform(), _child.getTransform());

            Transform ret;
            ret.setOrigin(_child.getOrigin());
            ret.setScale(GetScale(world));
            ret.setRotation(GetRotation(world));
            ret.setPosition(GetPosition(world));

            return (ret);
        }

        // for world->local
        [[nodiscard]] inline static Transform GetInverseTransform(const Transform& _parent, const Transform& _child)
        {
            sf::Transform local = _parent.getInverseTransform() * _child.getTransform();

            const sf::Vector2f scale = GetScale(local);
            Transform ret;
            ret.setOrigin(_child.getOrigin());
            ret.setScale(scale);
            ret.setPosition ( GetPosition(local) );
            ret.setRotation ( GetRotation(local) );

            return (ret);
        }

        // Why doesn't sfml have easy access to this stuff?
        /*
         sf::Transform matrix is defined as a 3x3 matrix in a 4x4 structure as:
        : m_matrix{a00, a10, 0.f, a20,
                   a01, a11, 0.f, a21,
                   0.f, 0.f, 1.f, 0.f,
                   a02, a12, 0.f, a22}


        // Getting the indices:

             0   1   2   3

        0    00, 01, 02, 03,
        1    04, 05, 06, 07,
        2    08, 09, 10, 11,
        3    12, 13, 14, 15,
       */

        inline static sf::Vector2f GetPosition( const sf::Transform& _transform )
        {
            const float* m = _transform.getMatrix();
            return sf::Vector2f( m[12], m[13] );
        }

        inline static sf::Angle GetRotation( const sf::Transform& _transform )
        {
            const float* m = _transform.getMatrix();
            return sf::radians( std::atan2(m[4], m[0])  );
        }

        inline static sf::Vector2f GetScale( const sf::Transform& _transform )
        {
            const float* m = _transform.getMatrix();
            float a = m[0];
            float b = m[4];
            float c = m[1];
            float d = m[5];

            float scaleX = std::sqrt(a * a + b * b);
            float scaleY = std::sqrt(c * c + d * d);

            // mirroring:
            if (a * d - b * c < 0)
            {
                scaleY = -scaleY;
            }
            return sf::Vector2f(scaleX, scaleY);
        }

        /*

         I tested the above matrix math with tests like the following:

            ECS_SFML::Transform t;
            t.setPosition(sf::Vector2f(50.f, 0.f));
            t.setScale(sf::Vector2f(1.0f, 1.0f));
            t.setRotation(sf::degrees(90.0f));

            sf::Transform transform = t.getTransform();

            sf::Vector2f posParent = ECS_SFML::Transform::GetPosition(transform);
            sf::Vector2f scaleParent = ECS_SFML::Transform::GetScale(transform);
            sf::Angle angleParent = ECS_SFML::Transform::GetRotation(transform);

            ECS_SFML::Transform tChild;
            tChild.setPosition(sf::Vector2f(50.0f, 0.0f));
            tChild.setScale(sf::Vector2f(1.f, 1.f));
            tChild.setRotation(sf::degrees(90.0f));

            ECS_SFML::Transform tApplied = ECS_SFML::Transform::GetAppliedTransform(t, tChild);

            sf::Transform transformChildWorld= tApplied.getTransform();
            sf::Vector2f posChildWorld = ECS_SFML::Transform::GetPosition(transformChildWorld);
            sf::Vector2f scaleChildWorld = ECS_SFML::Transform::GetScale(transformChildWorld);
            sf::Angle angleChildWorld = ECS_SFML::Transform::GetRotation(transformChildWorld);


            ECS_SFML::Transform tInverted = ECS_SFML::Transform::GetInverseTransform(t, tApplied);

            sf::Transform transformChildInverted = tInverted.getTransform();
            sf::Vector2f posChildLocal= ECS_SFML::Transform::GetPosition(transformChildInverted);
            sf::Vector2f scaleChildLocal = ECS_SFML::Transform::GetScale(transformChildInverted);
            sf::Angle angleChildLocal = ECS_SFML::Transform::GetRotation(transformChildInverted);

            std::stringstream outParentT;
            outParentT << "PARENT: pos: <" << posParent.x << "," << posParent.y << ">";
            outParentT << ", scale: <" << scaleParent.x << "," << scaleParent.y << ">";
            outParentT << ", rotation: <" << angleParent.asDegrees() <<">";
            OutputDebugString(outParentT.str().c_str());


            std::stringstream outChildW;
            outChildW << "CHILD W: pos: <" << posChildWorld.x << "," << posChildWorld.y << ">";
            outChildW << ", scale: <" << scaleChildWorld.x << "," << scaleChildWorld.y << ">";
            outChildW << ", rotation: <" << angleChildWorld.asDegrees() <<">";
            OutputDebugString(outChildW.str().c_str());

            std::stringstream outChildL;
            outChildL << "CHILD W: pos: <" << posChildLocal.x << "," << posChildLocal.y << ">";
            outChildL << ", scale: <" << scaleChildLocal.x << "," << scaleChildLocal.y << ">";
            outChildL << ", rotation: <" << angleChildLocal.asDegrees() <<">";
            OutputDebugString(outChildL.str().c_str());



         **/
    };
} // ECS_SFML