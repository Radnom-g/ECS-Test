//
// Created by Radnom on 3/11/2025.
//

#include "EntityManager.h"

#include <cassert>
#include <debugapi.h>

#include "ControllerManager.h"
#include "Components/TreeComponent.h"

namespace ECS
{
	bool EntityManager::Initialise(ControllerManager* _controllerManager, int _initialCapacity, int _maxCapacity)
	{
		if (isInitialised)
		{
			return false;
		}

		controllerManager = _controllerManager;
		maxCapacity = _maxCapacity;

		assert(_initialCapacity > 0 && "Initial Capacity must be > 0.");
		assert(_maxCapacity >= _initialCapacity && "Max Capacity must be >= Initial Capacity");

		treeController = controllerManager->GetComponentController<TreeComponent>();

		assert(treeController && "Cannot get TreeComponent component manager.");

		isInitialised = true;

		entities.resize(_initialCapacity);
		for (int i = 0; i < _initialCapacity; ++i)
		{
			entities[i].Reset();
		}

		return true;
	}

	EntityManager::~EntityManager()
	{
		// Delete the entities we added.
		for (std::vector<Entity*>::iterator iter = addedEntities.begin(); iter != addedEntities.end(); ++iter)
		{
			Entity* pEntity = (*iter);
			delete pEntity;
		}
		addedEntities.clear();
	}

	void EntityManager::ResetAll()
	{
		// Delete the entities we added.
		for (std::vector<Entity*>::iterator iter = addedEntities.begin(); iter != addedEntities.end(); ++iter)
		{
			Entity* pEntity = (*iter);
			delete pEntity;
		}

		addedEntities.clear();

		for (std::vector<Entity>::iterator iter = entities.begin(); iter != entities.end(); ++iter)
		{
			if ((*iter).active)
			{
				DeactivateEntity((*iter).id);
			}
		}
	}

	int EntityManager::ActivateEntity(const char* _name, int _parentId)
	{
		Entity* entity = nullptr;

		int entityIndex = GetNextInactiveEntityIndex();

		// If the next inactive entity is invalid, that means we've filled the list up and
		// need to make more space. So we create a new entity and when we get an opportunity,
		// we can resize the entities vector to keep them contiguous.
		if (entityIndex == -1)
		{
			if (entities.size() + addedEntities.size() >= maxCapacity)
			{
				// reached capacity.
				return -1;
			}

			entityIndex = (int)(entities.size() + addedEntities.size());

			// std::ostringstream oss;
			// oss << "Trying to add entity, cap is: " << entities.size() << "\n";
			// std::string str = oss.str();
			// DisplayLog(str.c_str());

			entity = new Entity();
			addedEntities.push_back(entity);
		}
		else
		{
			entity = &entities[entityIndex];
			nextIndex = entityIndex + 1;
		}

		// Set up a new Entity
		entity->active = true;
		entity->id = nextId;
		nextId++;

		// Point the map to the entity index
		entityIndexMap[entity->id] = entityIndex;

		char numstr[64];
		sprintf_s(numstr, "%d_%s", entity->id, _name);
		entity->name = numstr;

		OutputDebugString("Create Entity: ");
		OutputDebugString(numstr);
		OutputDebugString("\n");

		if (_parentId != -1)
		{
			SetEntityParent(entity->id, _parentId);
		}

		return entity->id;
	}

	void EntityManager::SetEntityParent(int _childId, int _parentId)
	{
		// Setting parent and child to something sensible.
		if (_parentId != -1 && _childId != -1)
		{
			bool alreadySet = false;

			// This will get the child component and add it if it doesn't exist.
			TreeComponent* childComponent = treeController->AddComponent(_childId);

			if (HasEntityInChildren(childComponent, _parentId))
			{
				assert(false && "SetEntityParent: Parent is a Child of the Child already!");
				return;
			}

			if (childComponent->parentId != -1)
			{
				if (childComponent->parentId == _parentId)
				{
					// We're setting the parent id, but it's already set on the child!
					alreadySet = true;
				}
				else
				{
					// We're changing the parent ID! we should clear it out first.
					SetEntityParent(_childId, -1);
				}
			}

			if (!alreadySet)
			{
				childComponent->parentId = _parentId;

				// This will get the parent component and add it if it doesn't exist
				TreeComponent* parentComponent = treeController->AddComponent(_parentId);
				parentComponent->AddChild(_childId);
			}

		}
		// Setting parent ID to -1.
		else if (_parentId == -1 && _childId != -1)
		{
			// parent ID is being set to -1, which means we should deparent.
			// Get the child component if there is one.
			TreeComponent* childComponent = treeController->GetComponent(_childId);

			RemoveFromParent(childComponent);
		}
		else
		{
			assert(false && "SetEntityParent: invalid Child and Parent IDs");
		}
	}

	bool EntityManager::IsActive(int _id)
	{
		std::map<int, int>::iterator mapIter = entityIndexMap.find(_id);
		if (mapIter != entityIndexMap.end())
		{
			int index = (*mapIter).second;
			if (index >= 0 && index < (int)entities.size())
			{
				return entities[index].active;
			}
			else if (index >= (int)entities.size())
			{
				// This is a new Entity that hasn't been integrated into the contiguous memory yet.
				for (std::vector<Entity*>::iterator iter = addedEntities.begin(); iter != addedEntities.end(); ++iter)
				{
					if ((*iter)->id == _id)
					{
						return (*iter);
					}
				}
			}
		}

		assert(false && "IsActive with invalid ID");
		return false;
	}

	int EntityManager::GetNextInactiveEntityIndex()
	{
		int capacity = (int)entities.size();
		for (int i = 0; i < capacity; ++i)
		{
			// we can save off the last id we checked so we don't have to iterate over the first few
			// almost-certainly occupied indices
			int index = (nextIndex + i) % capacity;

			if (!entities[index].active)
			{
				return index;
			}
		}
		return -1;
	}

	void EntityManager::DeactivateEntity(int _id)
	{
		// See if we have it in our deactivate list.
		// Normally this gets processed at the end of the frame, but we don't want to do it twice.
		if (!deactivatedEntities.empty())
		{
			std::vector<int>::iterator iter = deactivatedEntities.begin();
			while (iter != deactivatedEntities.end())
			{
				if (_id == (*iter))
				{
					deactivatedEntities.erase(iter);
					break;
				}
				iter++;
			}
		}

		Entity* entity = GetEntity(_id);
		if (entity != nullptr)
		{
			OutputDebugString("Destroy Entity: ");
			OutputDebugString(entity->name.c_str());
			OutputDebugString("\n");

			if (entity->active)
			{
				entity->active = false;

				TreeComponent* treeComponent = treeController->GetComponent(_id);
				if (treeComponent != nullptr)
				{
					DeactivateChildren(treeComponent);
					RemoveFromParent(treeComponent);
				}
			}

			controllerManager->EntityDestroyed(_id);

			entityIndexMap.erase(_id);
		}
		else
		{
			assert(false && "Entity ID out of range");
		}
	}

	void EntityManager::LateDeactivateEntity(int _id)
	{
		for (std::vector<int>::iterator entityIter = deactivatedEntities.begin(); entityIter != deactivatedEntities.end(); ++entityIter)
		{
			if ((*entityIter) == _id)
			{
				return;
			}
		}
		deactivatedEntities.push_back(_id);
	}

	// For parent entities, recursively deactivate their children.
	void EntityManager::DeactivateChildren(TreeComponent* _parentComponent)
	{
		if (_parentComponent == nullptr)
		{
			return;
		}
		for (std::vector<int>::iterator entityIter = _parentComponent->childrenIds.begin(); entityIter != _parentComponent->childrenIds.end(); ++entityIter)
		{
			int childEntityId = *entityIter;
			TreeComponent* childComponent = treeController->GetComponent(childEntityId);
			if (childComponent != nullptr)
			{
				// Clear the parent because we don't want them to process the deparenting
				childComponent->parentId = -1;
			}

			DeactivateEntity(childEntityId);
		}
		_parentComponent->childrenIds.clear();
	}

	// For child entities, remove them from their parent's list.
	void EntityManager::RemoveFromParent(TreeComponent* _childComponent)
	{
		if (_childComponent == nullptr)
		{
			return;
		}
		int _parentId = _childComponent->parentId;

		if (_parentId != -1)
		{
			int _childId = _childComponent->entityId;
			TreeComponent* parentComponent = treeController->GetComponent(_parentId);
			if (parentComponent != nullptr)
			{
				parentComponent->RemoveChild(_childId);
			}
		}
	}

	bool EntityManager::HasEntityInChildren(TreeComponent *_treeComponent, int _entityToCheck)
	{
		if (_treeComponent == nullptr)
		{
			return false;
		}
		for (std::vector<int>::iterator entityIter = _treeComponent->childrenIds.begin(); entityIter != _treeComponent->childrenIds.end(); ++entityIter)
		{
			int childEntityId = *entityIter;

			if (childEntityId == _entityToCheck)
				return true;

			TreeComponent* childComponent = treeController->GetComponent(childEntityId);
			if (childComponent != nullptr)
			{
				if (HasEntityInChildren(childComponent, _entityToCheck))
				{
					return true;
				}
			}
		}
		return false;
	}

	Entity* EntityManager::GetEntity(int _id)
	{
		// Need to grab the index from the ID.
		std::map<int, int>::iterator mapIter = entityIndexMap.find(_id);
		if (mapIter != entityIndexMap.end())
		{
			int index = (*mapIter).second;

			if (index >= 0 && index < (int)entities.size())
			{
				return &entities[index];
			}
			// If we've had to allocate an entity outside of our safe range, check the list of floating
			// added entities we haven't integrated yet.
			else if (index >= (int)entities.size())
			{
				for (std::vector<Entity*>::iterator iter = addedEntities.begin(); iter != addedEntities.end(); ++iter)
				{
					if ((*iter)->id == _id)
					{
						return (*iter);
					}
				}
			}
		}
		// The entity is out of range.
		return nullptr;
	}

	void EntityManager::ProcessEndOfFrame()
	{
		if (!deactivatedEntities.empty())
		{
			std::vector<int>::iterator iter = deactivatedEntities.begin();
			while ( iter != deactivatedEntities.end())
			{
				int id = (*iter);
				iter = deactivatedEntities.erase(iter);
				if (IsActive(id))
				{
					DeactivateEntity(id);
				}
			}
			deactivatedEntities.clear();
		}

		// Ideally we won't hit this at all, and we'd preallocate enough space.
		if (addedEntities.size() > 0)
		{
			// Figure out how big we need to resize the Entity list.
			int currentCapacity = static_cast<int>(entities.size());
			// We want to make it at LEAST 50% bigger to avoid doing this often.
			int newCapacity = static_cast<int>(static_cast<float>(currentCapacity) * 1.5f) + 1;
			// Make it at LEAST as big as the capacity plus all the entities we've queued for adding.
			newCapacity = std::max(newCapacity, (int)(entities.size() + addedEntities.size()));
			// Finally, make sure we don't exceed maxCapacity.
			assert( newCapacity <= maxCapacity && "Exceeded max capacity!");

			entities.resize(newCapacity);

			// We know how big we want the vector to be so we don't have to add them one by one.
			int addedEntityCount = (int)addedEntities.size();

			// Copy the added entities in.
			for (int i = 0; i < (int)addedEntities.size(); ++i)
			{
				int index = currentCapacity + i;

				entities[index] = Entity(*addedEntities[i]);

				// Ensure the map is pointing to the correct index.
				entityIndexMap[entities[index].id] = index;
			}

			// Clear out any extra entities at the end.
			for (int i = currentCapacity + addedEntityCount; i < newCapacity; ++i)
			{
				entities[i].Reset();
			}

			// Delete the temporary entities we added.
			for (std::vector<Entity*>::iterator iter = addedEntities.begin(); iter != addedEntities.end(); ++iter)
			{
				Entity* pEntity = (*iter);
				delete pEntity;
			}
			addedEntities.clear();
		}
	}

} // ECS