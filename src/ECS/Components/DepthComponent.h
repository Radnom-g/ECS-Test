//
// Created by Radnom on 4/11/2025.
//

#pragma once
#include "IComponent.h"

namespace ECS
{
    // This component must be present to render.
    class DepthComponent : public IComponent<DepthComponent>
    {
    public:

        int depth = 0;

        void ClearInternal() override
        {
            depth = 0;
        }

        static const char* GetName() { return "DepthComponent"; }
    };
} // ECS