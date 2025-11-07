//
// Created by Radnom on 3/11/2025.
//

#pragma once

#include <map>
#include <string>
#include <vector>

namespace ECS
{
    class ComponentManager;
}

namespace ECS
{
    struct WorldContext;

    // The EntityManager is where we can access, create and kill Entities.
    // It keeps a list of Entities in an array.
    class EntityManager
    {

    public:
        bool Initialise(WorldContext* context, int _initialCapacity, int _maxCapacity);
        ~EntityManager() = default;

        // Deactivates all the entities.
        void ResetAll();

        int ActivateEntity(const char* _name, int _parentId = -1);
        void DeactivateEntity(int _id);

        inline bool IsActive(int _id);
        const std::string& GetName(int _id);

        int GetMaxCapacity() const { return maxCapacity; }

    private:
        // Finds an inactive entity in the list to activate
        int GetNextInactiveEntityIndex();

        void SetCapacity(int _newCapacity);

        bool isInitialised = false;

        // We cannot ever make any more entities than this.
        int maxCapacity = 100;

        // Get the ComponentManager from WorldContext in Initialise so that we can tell components when an Entity
        // is destroyed.
        ComponentManager* componentManager = nullptr;

        // An entity's index will remain constant, but the entity's ID will change
        // so that we can track when an entity is destroyed.

        // This is just to keep track of the last entity index we activated,
        // so we can quickly check the next one in order, rather than going from 0 each time.
        int nextIndex=0;

        // The ID of the next entity to be activated. This increases for each entity so it should be
        // unique.
        int nextId = 0;

        // This map converts ID to INDEX.
        std::map<int, int> entityIndexMap;

        std::vector<std::string> entityNames;
        std::vector<int> entityIds;
        std::vector<bool> activeEntities;

    };
} // ECS
