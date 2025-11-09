//
// Created by Radnom on 3/11/2025.
//

#include "../Entities/EntityManager.h"

#include <cassert>
#include <debugapi.h>

#include "../Components/ComponentManager.h"
#include "Components/TreeComponent.h"
#include "Worlds/WorldContext.h"

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

		componentManager = context->componentManager;
		componentCount = componentManager->GetComponentCount();

		assert(_initialCapacity > 0 && "Initial Capacity must be > 0.");
		assert(_maxCapacity >= _initialCapacity && "Max Capacity must be >= Initial Capacity");

		SetCapacity(_initialCapacity);


		return true;
	}

	void EntityManager::ResetAll()
	{
		// Delete the entities we added.
		for (int i = 0; i < state.size(); ++i)
		{
			if (state[i] != EEntityState::Alive)
			{
				DeactivateEntity(i);
			}
			else
			{
				state[i] = EEntityState::Dead;
			}
		}
	}

	Entity EntityManager::ActivateEntity(const char* _name, int _parentId)
	{
		// This will find the next inactive Entity index in our arrays of entities.
		// If the array is full, then it will resize our arrays up to our max capacity.
		// If we exceed max capacity, this will fail and return -1.
		int entityIndex = GetNextInactiveEntityIndex();

		if (entityIndex == -1)
		{
			return Entity(-1, -1);
		}

		// Set up a new Entity

		name[entityIndex] = _name;
		state[entityIndex] = EEntityState::Alive;

		int newUniqueId = uniqueEntityCount;
		uniqueEntityCount++;
		uniqueId[entityIndex] = newUniqueId;

		char numstr[64];
		sprintf_s(numstr, "Create Entity[%d]: %s:%d", entityIndex, _name, newUniqueId);
		OutputDebugString(numstr);

		if (_parentId != -1)
		{
			TreeComponent* treeComp = componentManager->GetComponent<TreeComponent>();
			treeComp->AddChild(_parentId, entityIndex);
		}

		return Entity(entityIndex, newUniqueId);
	}

	bool EntityManager::DeactivateEntity(const Entity& entity)
	{
		if (entity.index < 0 || entity.index >= currentCapacity)
		{
			// Not a valid index.
			return false;
		}

		if (uniqueId[entity.index] != entity.uniqueId)
		{
			// entity doesn't exist any more.
			return false;
		}

		if (state[entity.index] != EEntityState::Alive)
		{
			// already dead.
			return false;
		}

		char numstr[64];
		sprintf_s(numstr, "Destroy Entity[%d]: %s:%d", entity.index, name[entity.index].c_str(), entity.uniqueId);
		OutputDebugString(numstr);

		return DeactivateEntity(entity.index);
	}

	bool EntityManager::DeactivateEntity(int _entityIndex)
	{
		bool bWasAlive = state[_entityIndex] == EEntityState::Alive;

		componentManager->OnEntityDestroyed(_entityIndex);

		name[_entityIndex] = "Invalid";
		state[_entityIndex] = EEntityState::Dead;
		uniqueId[_entityIndex] = -1;

		return bWasAlive;
	}

	int EntityManager::GetNextInactiveEntityIndex()
	{
		for (int i = 0; i < currentCapacity; ++i)
		{
			int index = (nextIndex + i) % currentCapacity;

			if (state[index] == EEntityState::Dead)
			{
				return index;
			}
		}

		if (currentCapacity < maxCapacity || maxCapacity == -1)
		{
			int beforeCapacity = currentCapacity;
			int newCapacity = std::max( currentCapacity * 2, 1);
			SetCapacity(newCapacity);

			if (newCapacity > beforeCapacity)
			{
				nextIndex = beforeCapacity;
				return nextIndex;
			}
		}

		return -1;
	}

	void EntityManager::SetCapacity(int _newCapacity)
	{
		if (maxCapacity > 0)
		{
			_newCapacity = std::min(_newCapacity, maxCapacity);
		}

		currentCapacity = _newCapacity;
		name.resize(currentCapacity, "Invalid");
		state.resize(currentCapacity, EEntityState::Dead);
		uniqueId.resize(currentCapacity, -1);

		componentManager->ResizeEntityArray(currentCapacity);
	}

} // ECS