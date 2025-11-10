//
// Created by Radnom on 9/11/2025.
//

#include "SystemManager.h"

#include <typeinfo>
#include <cassert>

#include "ISystem.h"

namespace ECS
{
    void SystemManager::Process(float _deltaSeconds)
    {
        for (int i = 0; i < processSystemsOrdered.size(); i++)
        {
            ISystem *system = processSystemsOrdered[i];
            system->Process(_deltaSeconds);
        }
    }

    void SystemManager::Render(float _deltaTween)
    {
        for (int i = 0; i < renderSystemsOrdered.size(); i++)
        {
            ISystem *system = renderSystemsOrdered[i];
            system->Render(_deltaTween);
        }
    }

    void SystemManager::RegisterSystem(ISystem *_system)
    {
        assert(_system && "Don't Register null component");

        for (auto iter : systems)
        {
            assert (typeid(iter) == typeid(_system) && "Already registered!");
        }

        systems.push_back(_system);

        if (_system->GetDoesProcessTick() || _system->GetDoesRenderTick())
        {
            ReorderSystems();
        }
    }

    void SystemManager::ReorderSystems()
    {
        processSystemsOrdered.clear();

        // Add processing systems
        for (int i = 0; i < systems.size(); i++)
        {
            ISystem *system = systems[i];
            if (!system->GetDoesProcessTick())
                continue;
            processSystemsOrdered.push_back(system);
        }


        bool listSorted = false;
        int breaker = 500;
        while (!listSorted)
        {
            breaker--;
            assert(breaker >= 0 && "ERROR! got stuck in a loop reordering systems");

            listSorted = true;
            for (int i = 0; i < processSystemsOrdered.size(); i++)
            {
                ISystem *system = processSystemsOrdered[i];

                std::vector<std::string> beforeRequired;
                system->GetProcessAfter(beforeRequired);

                if (beforeRequired.size() == 0)
                {
                    continue;
                }

                for (int j = i+1; j < processSystemsOrdered.size(); j++)
                {
                    ISystem *comp = processSystemsOrdered[j];

                    for (int k = 0; k < beforeRequired.size(); ++k)
                    {
                        if (comp->GetSystemName() == beforeRequired[k])
                        {
                            listSorted = false;
                            break;
                        }
                    }
                    if (!listSorted)
                        break;
                }

                if (!listSorted)
                {
                    // move it to the back
                    processSystemsOrdered.erase(processSystemsOrdered.begin()+i);
                    processSystemsOrdered.push_back(system);
                    break;
                }
            }
        }


        renderSystemsOrdered.clear();
        // Add processing systems
        for (int i = 0; i < systems.size(); i++)
        {
            ISystem *system = systems[i];
            if (!system->GetDoesRenderTick())
                continue;
            renderSystemsOrdered.push_back(system);
        }

        listSorted = false;
        breaker = 500;
        while (!listSorted)
        {
            breaker--;
            assert(breaker >= 0 && "ERROR! got stuck in a loop reordering rendering systems");

            listSorted = true;
            for (int i = 0; i < renderSystemsOrdered.size(); i++)
            {
                ISystem *system = renderSystemsOrdered[i];

                std::vector<std::string> beforeRequired;
                system->GetRenderAfter(beforeRequired);

                if (beforeRequired.size() == 0)
                {
                    continue;
                }

                for (int j = i+1; j < renderSystemsOrdered.size(); j++)
                {
                    ISystem *comp = renderSystemsOrdered[j];

                    for (int k = 0; k < beforeRequired.size(); ++k)
                    {
                        if (comp->GetSystemName() == beforeRequired[k])
                        {
                            listSorted = false;
                            break;
                        }
                    }
                    if (!listSorted)
                        break;
                }

                if (!listSorted)
                {
                    // move it to the back
                    renderSystemsOrdered.erase(renderSystemsOrdered.begin()+i);
                    renderSystemsOrdered.push_back(system);
                    break;
                }
            }
        }
    }
} // ECS