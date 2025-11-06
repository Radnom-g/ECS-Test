//
// Created by Radnom on 3/11/2025.
//

#include "../Entities/EntityManager.h"

#include <cassert>
#include <debugapi.h>

#include "../Components/ComponentManager.h"
#include "Components/TreeComponent.h"

namespace ECS
{
	bool EntityManager::Initialise(WorldContext *context, int _initialCapacity, int _maxCapacity)
	{
		if (isInitialised)
		{
            assert(false && "EntityManager Initialised Twice!");
			return false;
		}

		isInitialised = true;
		maxCapacity = _maxCapacity;

		assert(_initialCapacity > 0 && "Initial Capacity must be > 0.");
		assert(_maxCapacity >= _initialCapacity && "Max Capacity must be >= Initial Capacity");

		entityNames.reserve(_initialCapacity);
		entityIds.reserve(_initialCapacity);
		activeEntities.reserve(_initialCapacity);

		return true;
	}

	void EntityManager::ResetAll()
	{
		// Delete the entities we added.
		for (int i = 0; i < entityIds.size(); ++i)
		{
			DeactivateEntity(i);
		}
	}

	int EntityManager::ActivateEntity(const char* _name, int _parentId)
	{
		// This will find the next inactive Entity index in our arrays of entities.
		// If the array is full, then it will resize our arrays up to our max capacity.
		// If we exceed max capacity, this will fail and return -1.
		int entityIndex = GetNextInactiveEntityIndex();

		if (entityIndex == -1)
		{
			return -1;
		}

		// Set up a new Entity
		int newEntityId = nextId;
		nextId++;

		// Point the map to the entity index
		entityIndexMap[newEntityId] = entityIndex;

		entityNames[entityIndex] = _name;
		entityIds[entityIndex] = newEntityId;
		activeEntities[entityIndex] = true;

		char numstr[64];
		sprintf_s(numstr, "%s_%d", _name, newEntityId);
		OutputDebugString("Create Entity: ");
		OutputDebugString(numstr);
		OutputDebugString("\n");

		if (_parentId != -1)
		{
			TreeComponent* treeComp = componentManager->GetComponent<TreeComponent>();
			treeComp->AddChild(_parentId, newEntityId);
		}

		return newEntityId;
	}

	void EntityManager::DeactivateEntity(int _id)
	{
		auto mapIter = entityIndexMap.find(_id);
		if (mapIter != entityIndexMap.end())
		{
			componentManager->OnEntityDestroyed(_id);

			entityNames[mapIter->second] = "Invalid";
			entityIds[mapIter->second] = -1;
			activeEntities[mapIter->second] = false;

			entityIndexMap.erase(mapIter);
		}
	}

	bool EntityManager::IsActive(int _id)
	{
		auto mapIter = entityIndexMap.find(_id);
		if (mapIter != entityIndexMap.end())
		{
			int index = (*mapIter).second;
			return (activeEntities[index]);
		}
		// It has expired.
		return false;
	}

	int EntityManager::GetNextInactiveEntityIndex()
	{
		int capacity = (int)activeEntities.size();
		for (int i = 0; i < capacity; ++i)
		{
			int index = (nextIndex + i) % capacity;

			if (!activeEntities[index])
			{
				return index;
			}
		}

		if (capacity < maxCapacity || maxCapacity == -1)
		{
			int newCapacity = std::max( capacity * 2, 1);
			if (maxCapacity > 0)
			{
				newCapacity = std::min(newCapacity, maxCapacity);
			}
			if (newCapacity > capacity)
			{
				SetCapacity(newCapacity);
				nextIndex = capacity;
				return nextIndex;
			}
		}

		return -1;
	}

	void EntityManager::SetCapacity(int _newCapacity)
	{
		entityNames.resize(_newCapacity, "Invalid");
		entityIds.resize(_newCapacity, -1);
		activeEntities.resize(_newCapacity, false);

	}

} // ECS