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

        int loopBreaker = 1000;
        bool allPlaced = false;

        while (!allPlaced)
        {
            allPlaced = true;
            loopBreaker--;
            if (loopBreaker <= 0)
            {
                assert(false && "ERROR!! some kind of requirements loop in Process Systems order requirement");
                break;
            }

            for (int i = 0; i < systems.size(); i++)
            {
                ISystem *system = systems[i];
                if (!system->GetDoesProcessTick())
                    continue;

                bool bContains = false;
                for (int j = 0; j < systems.size(); j++)
                {
                    if (systems[j] == system)
                    {
                        bContains = true;
                    }
                }

                if (!bContains)
                {
                    std::vector<std::string> beforeRequired;
                    system->GetProcessAfter(beforeRequired);

                    int requiredCount = beforeRequired.size();

                    for (int j = 0; j < processSystemsOrdered.size(); j++)
                    {
                        for (int k = beforeRequired.size()-1; k >= 0; k--)
                        {
                            if (processSystemsOrdered[j] == processSystemsOrdered[k])
                            {
                                requiredCount--;
                            }
                        }
                    }

                    if (requiredCount == 0)
                    {
                        processSystemsOrdered.push_back(system);
                    }
                    else
                    {
                        allPlaced = false;
                    }
                }
            }
        }

        renderSystemsOrdered.clear();

        loopBreaker = 1000;
        allPlaced = false;

        while (!allPlaced)
        {
            allPlaced = true;
            loopBreaker--;
            if (loopBreaker <= 0)
            {
                assert(false && "ERROR!! some kind of requirements loop in Render Systems order requirement");
                break;
            }

            for (int i = 0; i < systems.size(); i++)
            {
                ISystem *system = systems[i];
                if (!system->GetDoesRenderTick())
                    continue;

                bool bContains = false;
                for (int j = 0; j < systems.size(); j++)
                {
                    if (systems[j] == system)
                    {
                        bContains = true;
                    }
                }

                if (!bContains)
                {
                    std::vector<std::string> beforeRequired;
                    system->GetProcessAfter(beforeRequired);

                    int requiredCount = beforeRequired.size();

                    for (int j = 0; j < renderSystemsOrdered.size(); j++)
                    {
                        for (int k = beforeRequired.size()-1; k >= 0; k--)
                        {
                            if (renderSystemsOrdered[j] == renderSystemsOrdered[k])
                            {
                                requiredCount--;
                            }
                        }
                    }

                    if (requiredCount == 0)
                    {
                        renderSystemsOrdered.push_back(system);
                    }
                    else
                    {
                        allPlaced = false;
                    }
                }
            }
        }
    }
} // ECS