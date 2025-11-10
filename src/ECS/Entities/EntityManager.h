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

    enum EEntityState
    {
        Invalid,        // Not a valid ID, out of range.
        Dead,           // Dead and can be changed to Unused at any time.
        Alive,          // Currenly active Entity.
    };

    struct Entity
    {
        Entity(int _entityIndex, int _newUniqueId)
            :index(_entityIndex), uniqueId(_newUniqueId)
        {
        }
        Entity() = default;
        Entity(const Entity& _entity)
            : index(_entity.index), uniqueId(_entity.uniqueId){}
        int index = -1;
        int uniqueId = -1;
        bool IsValid() const { return index != -1 && uniqueId != -1; }
    };

    // The EntityManager is where we can access, create and kill Entities.
    // It keeps a list of Entities in an array.
    class EntityManager
    {

    public:
        bool Initialise(WorldContext* context, int _initialCapacity, int _maxCapacity);
        virtual ~EntityManager() = default;

        // Deactivates all the entities.
        void ResetAll();

        Entity GetEntity(int _entityIndex);
        Entity ActivateEntity(const char* _name);
        Entity ActivateEntity(const char* _name, const Entity& _parent);

        bool DeactivateEntity(const Entity& _entity);

        EEntityState GetState(const Entity& _entity) const
        {
            if (_entity.index < 0 || _entity.index >= currentCapacity)
            {
                // Not a valid index.
                return EEntityState::Invalid;
            }

            // ID has been reused.
            if (uniqueId[_entity.index] != _entity.uniqueId)
                return EEntityState::Dead;

            return state[_entity.index];
        }

        const std::string& GetName(const Entity& entity)
        {
            if (entity.index < 0 || entity.index >= currentCapacity)
            {
                return name[entity.index];
            }
            return InvalidName;
        }

        int GetMaxCapacity() const { return maxCapacity; }

        // int GetComponentIndex(const Entity& entity, int _componentId)
        // {
        //     if (entity.index < 0 || entity.index >= currentCapacity)
        //         return -1;
        //     if (uniqueId[entity.index] != entity.uniqueId)
        //         return -1;
        //     return (componentIds[entity.index][_componentId]);
        // }

        inline static const Entity InvalidEntity;

    private:

        // Finds an inactive entity in the list to activate
        int GetNextInactiveEntityIndex();

        void SetCapacity(int _newCapacity);

        bool isInitialised = false;

        // Every time an Entity is created, this is increased.
        // Systems/Worlds can check if the Unique ID of an Entity is the same as the one they are expecting
        // to check if their entity has died and ID has been reused.
        int uniqueEntityCount = 0;

        // We cannot ever make any more entities than this.
        int maxCapacity = 100;

        // highest entity index
        int currentCapacity = 0;

        // Get the ComponentManager from WorldContext in Initialise so that we can tell components when an Entity
        // is destroyed.
        ComponentManager* componentManager = nullptr;

        // This is just to keep track of the last entity index we activated,
        // so we can quickly check the next one in order, rather than going from 0 each time.
        int nextIndex=0;

        // How many different Components there are.
        // This needs to be obtained by WorldContext and can't change at runtime.
        int componentCount = 1;


        /*
         *  PER ENTITY DATA:
         *  These need to be maintained to be the same size.
         */
        std::vector<std::string> name;
        std::vector<EEntityState> state;
        std::vector<int> uniqueId;

        static constexpr std::string InvalidName = std::string("Invalid");
    };
} // ECS
