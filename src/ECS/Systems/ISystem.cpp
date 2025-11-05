//
// Created by Radnom on 4/11/2025.
//

#include "ISystem.h"

#include <cassert>

namespace ECS
{
    void ISystem::Process(float _deltaSeconds)
    {
        assert(GetDoesProcessTick() && "Don't call Update on this System.");
        ProcessInternal(_deltaSeconds);
    }

    void ISystem::Render(float _deltaTween)
    {
        assert(GetDoesRenderTick() && "Don't call Render on this System.");
        RenderInternal(_deltaTween);
    }
} // ECS