//
// Created by Radnom on 4/11/2025.
//

#pragma once

namespace ECS
{
    template<class T>
    class ComponentAccessor;

}

namespace ECS_SFML
{
    // Useful forward declarations and handy typedefs to get around the long template format

    class RenderSpriteComponent;
    typedef ECS::ComponentAccessor<RenderSpriteComponent> RenderSpriteController;

    class TransformComponent;
    typedef ECS::ComponentAccessor<TransformComponent> TransformController;

} // ECS_SFML