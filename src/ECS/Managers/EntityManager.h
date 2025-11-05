//
// Created by Radnom on 3/11/2025.
//

#pragma once

#include "Entities/Entity.h"
#include <map>
#include <vector>

namespace ECS
{
    class AccessorManager;

    template<class T>
    class ComponentAccessor;
    class TreeComponent;
    typedef ComponentAccessor<TreeComponent> TreeController;

    // The EntityManager is where we can access, create and kill Entities.
    // It keeps a list of Entities in an array.
    class EntityManager
    {

    public:
        bool Initialise(AccessorManager* _controllerManager, int _initialCapacity, int _maxCapacity);
        ~EntityManager();

        // Deactivates all the entities.
        void ResetAll();

        int ActivateEntity(const char* _name, int _parentId = -1);
        void SetEntityParent(int _childId, int _parentId);

        Entity* GetEntity(int _id);

        bool IsActive(int _id);
        void DeactivateEntity(int _id);
        void LateDeactivateEntity(int _id); //Deactivate at end of frame

        void ProcessEndOfFrame();

        int GetMaxCapacity() const { return maxCapacity; }

    private:
        // Finds an inactive entity in the list to activate
        int GetNextInactiveEntityIndex();

        // For parent entities, recursively deactivate their children.
        void DeactivateChildren(TreeComponent* _parentComponent);

        // For child entities, remove them from their parent's list.
        void RemoveFromParent(TreeComponent* _childComponent);

        // recursively checks Children in a TreeComponent to see if the entity exists.
        // Useful to make sure that we don't add a child that contains the parent (oroboros)
        bool HasEntityInChildren(TreeComponent* _treeComponent, int _entityToCheck);

        bool isInitialised = false;

        // We cannot ever make any more entities than this.
        int maxCapacity = 100;

        AccessorManager* controllerManager = nullptr;

        TreeController* treeController = nullptr;


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

        // Our entities in contiguous memory.
        std::vector<Entity> entities;

        // These guys have to be added into the original array to be contiguous,
        // but will be done at a safe time to avoid invalidating any pointers
        // that an active system is using.
        std::vector<Entity*> addedEntities;

        // these entities have been deactivated in the middle of a loop, so they couldn't be removed straight away.
        // we should deactivate them in the end of frame process.
        std::vector<int> deactivatedEntities;

    };
} // ECS
