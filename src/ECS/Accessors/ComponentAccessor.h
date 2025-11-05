//
// Created by Radnom on 3/11/2025.
//

#pragma once
#include <cassert>
#include <map>
#include <vector>

#include "IComponentAccessor.h"
#include "Components/IComponent.h"

namespace ECS
{
	// Templated ComponentAccessor for managing a single Component type.
	// In most cases you can just define this directly and use it rather than creating a unique Controller,
	// i.e. ComponentAccessor<TreeComponent> treeAccessor;

	// In some cases you may want to do something more specific with the components, i.e. Depth has extra processing
	// to maintain a map of depth->entities.

    template<class T>
    class ComponentAccessor: public IComponentAccessor
    {
    public:
	    ~ComponentAccessor() override
	    {
	    	// Delete any temp components we added.
	    	using iterator = std::vector<T*>::iterator;
	    	for (iterator iter = addedComponents.begin(); iter != addedComponents.end(); ++iter)
	    	{
	    		T* pComponent = (*iter);
	    		delete pComponent;
	    	}
	    	addedComponents.clear();
	    }

	    virtual bool Initialise(int _initialCapacity, int _maxCapacity)
	    {
	    	if (isInitialised)
		    {
	    		return false;
		    }

	    	// todo: copy the Asserts from the EntityManager here
	    	maxCapacity = _maxCapacity;

	    	componentName = T::GetName();
	    	canHaveMultiple = T::CanEntityHaveMultiple();

	    	isInitialised = true;

	    	components.resize(_initialCapacity);
	    	for (int i = 0; i < _initialCapacity; ++i)
	    	{
	    		AbstractComponent* c = static_cast<AbstractComponent*>(&components[i]);
	    		c->Clear();
	    	}

	    	return true;
	    }

	    bool HasComponent(int _entityId)
		{
	    	if (canHaveMultiple)
	    	{
	    		return (entityIdComponentIndexMapMultiple.find(_entityId) != entityIdComponentIndexMapMultiple.end());
	    	}
			return (entityIdComponentIndexMap.find(_entityId) != entityIdComponentIndexMap.end());
		}

		bool IsComponentActive(int _componentId)
		{
			return (componentIdIndexMap.find(_componentId) != componentIdIndexMap.end());
		}

    	// Returns the first Component if there are multiple.
		T* GetComponent(int _entityId)
		{
	    	if (canHaveMultiple)
	    	{
	    		// We may have multiple -
	    		// use 'GetComponents' instead and return the first (if any).
	    		std::vector<T*> componentList;
	    		if (GetComponents(_entityId, componentList))
	    		{
	    			return componentList[0];
	    		}
	    		return nullptr;
	    	}

			std::map<int, int>::iterator iter = entityIdComponentIndexMap.find(_entityId);

			if (iter == entityIdComponentIndexMap.end())
			{
				// Cannot find the index for this Entity ID.
				return nullptr;
			}
			else if ((*iter).second < (int)components.size())
			{
				// Index is within our main components array.
				return &components[(*iter).second];
			}
			else if (addedComponents.size() > 0)
			{
				// Check if the Index is within our Pending list of components.
				for (typename std::vector<T*>::iterator iter = addedComponents.begin();
				     iter != addedComponents.end(); ++iter)
				{
					if ((*iter)->entityId == _entityId)
					{
						return (*iter);
					}
				}
			}
			return nullptr;
		}

    	bool GetComponents(int _entityId, std::vector<T*>& _outList)
	    {
	    	_outList.clear();

	    	// If we can't have multiple, then we don't use the List map.
	    	// Use GetComponent instead and get the only (if any) from the singular list.
	    	if (!canHaveMultiple)
	    	{
	    		T* first = GetComponent(_entityId);
	    		if (first)
	    		{
	    			_outList.push_back(first);
	    			return true;
	    		}
	    		return false;
	    	}

	    	auto mapIter = entityIdComponentIndexMapMultiple.find(_entityId);
			if (mapIter == entityIdComponentIndexMapMultiple.end())
			{
				return false;
			}

	    	std::vector<int>& entityComponentList = mapIter->second;

	    	for (int index : entityComponentList)
	    	{
	    		if (index < (int)components.size())
	    		{
	    			// Index is within our main components array.
	    			_outList.push_back( &components[index] );
	    		}
	    		else if (!addedComponents.empty())
	    		{
	    			// Check if the Index is within our Pending list of components.
	    			for (typename std::vector<T*>::iterator iter = addedComponents.begin();
						 iter != addedComponents.end(); ++iter)
	    			{
	    				if ((*iter)->entityId == _entityId)
	    				{
	    					return (*iter);
	    				}
	    			}
	    		}
	    	}

	    	return (!_outList.empty());
	    }

		virtual T* AddComponent(int _entityId)
		{
			if (_entityId < 0)
			{
				assert(0 && "Invalid entity Id");
				return nullptr;
			}

			// Return it if it already exists.
			T* component = GetComponent(_entityId);
			if (component != nullptr)
			{
				return component;
			}

			int componentIndex = GetNextInactiveComponent();
			if (componentIndex == -1)
			{
				// If the next inactive component is invalid, that means we've filled the list up and
				// need to make more space. So we create a new component and when we get an opportunity,
				// we can resize the components vector and copy the new ones across to keep them contiguous.
				componentIndex = (int)(components.size() + addedComponents.size());

				if (componentIndex >= maxCapacity)
				{
					//hit our capacity. Not allowed to make more than this.
					return nullptr;
				}

				component = new T();
				addedComponents.push_back(component);
			}
			else
			{
				component = &components[componentIndex];
				component->Clear();
				nextIndex = componentIndex + 1;
			}

			// Set up the newly activated component
			component->active = true;
			component->entityId = _entityId;
			component->componentId = nextId;
			nextId++;

			// Point the component ID map to the component index
			componentIdIndexMap[component->entityId] = componentIndex;

	    	if (canHaveMultiple)
	    	{
	    		entityIdComponentIndexMapMultiple.insert(std::make_pair(_entityId, componentIndex));
	    	}
	    	else
	    	{
	    		// Point the entity map to the component index
	    		entityIdComponentIndexMap[_entityId] = componentIndex;
	    	}

			return component;
		}

		// This will return a list of entities that have this component.
		virtual void GetEntitiesWithComponent(std::vector<int>& _entityList)
		{
			// We keep this map of entity ID -> component index up-to-date, so we can just return its keys.
			_entityList.clear();
			for (std::map<int, int>::iterator iter = entityIdComponentIndexMap.begin(); iter != entityIdComponentIndexMap.end(); ++iter)
			{
				_entityList.push_back((*iter).first);
			}
		}

		virtual void GetComponents(std::vector<T*>& _componentList)
		{
			_componentList.clear();
			for (typename std::vector<T>::iterator iter = components.begin();
			     iter != components.end(); ++iter)
			{
				_componentList.push_back(&(*iter));
			}
			if (!addedComponents.empty())
			{
				for (typename std::vector<T*>::iterator iter = addedComponents.begin();
				     iter != addedComponents.end(); ++iter)
				{
					_componentList.push_back(*iter);
				}
			}
		}

		// This will take a list of entity IDs and remove entities that don't have this component
		virtual void FilterKeepEntitiesWithComponent(std::vector<int>& _entityListFilter)
		{
			std::vector<int>::iterator iter = _entityListFilter.begin();
			while (iter != _entityListFilter.end())
			{
				int entityId = *iter;
				if (entityIdComponentIndexMap.find(entityId) != entityIdComponentIndexMap.end())
				{
					++iter;
				}
				else
				{
					iter = _entityListFilter.erase(iter);
				}
			}
		}

		// This will take a list of entity IDs and remove entities that DO have this component
		virtual void FilterKeepEntitiesWithoutComponent(std::vector<int>& _entityListFilter)
		{
			std::vector<int>::iterator iter = _entityListFilter.begin();
			while (iter != _entityListFilter.end())
			{
				int entityId = *iter;
				if (entityIdComponentIndexMap.find(entityId) != entityIdComponentIndexMap.end())
				{
					iter = _entityListFilter.erase(iter);
				}
				else
				{
					iter++;
				}
			}
		}

		virtual void EntityDestroyed(int _entityId) override
		{
			RemoveComponent(_entityId);
		}

		virtual void RemoveComponent(int _entityId)
		{
			AbstractComponent* pCom = static_cast<AbstractComponent*>(GetComponent(_entityId));

			if (pCom != nullptr)
			{
				componentIdIndexMap.erase(pCom->componentId);
				pCom->Clear();
			}

			entityIdComponentIndexMap.erase(_entityId);
		}

		void ProcessEndOfFrame() override
		{
			// Ideally we won't hit this at all, and we'd preallocate enough space.
			if (!addedComponents.empty())
			{
				// Figure out how big we need to resize the Component list to fit the new Components.

				int currentCapacity = (int)components.size();

				// We want to make it at LEAST 50% bigger to avoid doing this often.
				int newCapacity = static_cast<int>(static_cast<float>(currentCapacity) * 1.5f) + 1;
				// Make it at LEAST as big as the capacity plus all the entities we've queued for adding.
				newCapacity = std::max(newCapacity, (int)(components.size() + addedComponents.size()));
				// Finally, make sure we don't exceed maxCapacity.
				assert( newCapacity <= maxCapacity && "Exceeded max capacity!");

				// We know how big we want the vector to be so we don't have to add them one by one.
				int addedComponentCount = (int)addedComponents.size();
				components.resize(newCapacity);

				// Copy the added components in.
				for (int i = 0; i < (int)addedComponents.size(); ++i)
				{
					int index = currentCapacity + i;
					components[index] = T(*addedComponents[i]);
				}

				// Clear out any extra components at the end.
				for (int i = currentCapacity + addedComponentCount; i < newCapacity; ++i)
				{
					AbstractComponent* c = static_cast<AbstractComponent*>(&components[i]);
					c->componentId = (int)i;
					c->entityId = -1;
					c->active = false;
				}

				// Delete the components we added.
				for (typename std::vector<T*>::iterator iter = addedComponents.begin(); iter != addedComponents.end(); ++iter)
				{
					T* pCom = (*iter);
					delete pCom;
				}

				addedComponents.clear();
			}
		}


	private:
		// Finds an inactive component in the list
		virtual int GetNextInactiveComponent()
		{
			int capacity = (int)components.size();
			for (int i = 0; i < capacity; ++i)
			{
				int index = (nextIndex + i) % capacity;

				if (!components[index].active)
				{
					return index;
				}
			}
			return -1;
		}


    	const char* GetComponentName() const override { return componentName; }
    	bool CanHaveMultipleComponentsAssignedToOneEntity() const { return  canHaveMultiple; }

	protected:

    	// cannot ever exceed this value
    	int maxCapacity = 100000;

		const char* componentName = "INVALID";

    	bool canHaveMultiple = false;

		// The component's index will remain constant, but the ID will change
		// so that we can track when a component is destroyed.

		// This is just to keep track of the last component index we activated,
		// so we can quickly check the next one in order, rather than going from 0 each time.
		int nextIndex = 0;

		// The ID of the next component to be activated.
		// This increases for each entity so it should be unique.
		int nextId = 0;

		// This map converts Component ID to Index.
		std::map<int, int> componentIdIndexMap;

		// Map of entity ID to component index for quick lookup.
    	// Used for Components that cannot have multiple of the Component assigned to a single Entity.
		std::map<int, int> entityIdComponentIndexMap;

    	// Map of entity ID to a vector of components that belong to it (via index).
    	// Used for Component types that can have multiple of the same Component assigned to a single Entity.
    	std::map<int, std::vector<int>> entityIdComponentIndexMapMultiple;

		// Our components in contiguous memory.
		std::vector<T> components;

		// These guys have to be added into the original array to be contiguous,
		// but will be done at a safe time to avoid invalidating any pointers
		// that an active system is using.
		std::vector<T*> addedComponents;
    };

} // ECS