//
// Created by Radnom on 3/11/2025.
//

#pragma once
#include <map>
#include <memory>
#include <vector>
#include "../External/IndexList.h"

#include "ComponentSettings.h"
#include "Entities/EntityManager.h"

namespace ECS
{
	struct WorldContext;

	// A Component stores plain data with no behaviour.
    // it maintains a vector for each piece of relevant data.
    class IComponent
    {
    public:
    	friend class ComponentManager;

	    virtual ~IComponent() = default;

    	template <typename T>
    	static ComponentSettings CreateSettings(int initialCapacity, int maxCapacity)
    	{
    		ComponentSettings settings;
    		settings.name = "IComponent"; // still not correct yet
    		settings.initialCapacity = initialCapacity;
    		settings.maxCapacity = maxCapacity;
    		settings.Constructor = []() -> T* { return new T(); };
    		return settings;
    	}

    	/*
		// PUBLIC FUNCTIONS:
		*/
    	int GetComponentId() const { return componentId; }

		bool InitialiseComponent(WorldContext* context, int _componentId, int _initialCapacity, int _maxCapacity);

    	int AddComponent(const Entity& _entity) { return AddComponent(_entity.index); }
    	int AddComponent(int _entityId);
    	void RemoveComponent(const Entity& _entity) { RemoveComponentsFromEntity(_entity.index); }
    	void RemoveComponentsFromEntity(int _entityId);
    	void RemoveComponent(int _componentIndex);

        void GetEntitiesWithComponent(std::vector<int>& _outEntityList);
		void FilterKeepEntitiesWithComponent(std::vector<int>& _entityListFilter);
    	void FilterKeepEntitiesWithoutComponent(std::vector<int>& _entityListFilter);

    	[[nodiscard]] bool HasComponent(int _entityId) const
    	{
    		return entityComponents[_entityId].size() > 0;
    	}

    	// Get component by index for direct access to data. However, if the component becomes inactive, it can be
    	// reused by another Entity, so don't hang on to the index without checking if the original Entity is still alive!
    	int GetComponentIndex(int _entityId) const
    	{
    		return entityComponents[_entityId][0];
    	}

    	// This will return a list of component indices belonging to this Entity.
    	const IndexList& GetComponentIndices(int _entityId)
    	{
			return entityComponents[_entityId];
    	}

    	[[nodiscard]] bool IsInitialised() const { return isInitialised; }
    	[[nodiscard]] int GetArraySize() const { return entityId.size(); }

    	/*
    	// PUBLIC PURE VIRTUAL FUNCTIONS:
    	*/
    	// The name of the component. i.e. "ThingComponent".
        [[nodiscard]] virtual const char* GetName() const = 0;
    	// if a single Entity can have multiple of this Component.
        [[nodiscard]] virtual bool IsUniquePerEntity() const = 0;


    protected:

    	/*
		// PROTECTED FUNCTIONS
		*/
    	void RemoveComponentByIndex(int _componentIndex);
    	// erase all values at given index
    	void ClearComponentAtIndex(int _componentIndex);

    	void ResizeEntityArray(int _newSize);

    	// For ComponentManager to assign.
    	void SetComponentId(int _index) { componentId = _index; }

    	/*
		// PROTECTED PURE VIRTUAL FUNCTIONS
		// (Called internally from IComponent)
		// These will need specific functionality added in derived Component classes.
		*/
    	// Reserve your arrays of data in here, up to _initialCapacity.
    	virtual bool InitialiseInternal(WorldContext* context, int _initialCapacity, int _maxCapacity) = 0;
    	// Called after a component is created.
    	// Initialise your data arrays to starting values here.
    	virtual void AddComponentInternal(int _entityId, int _componentIndex) = 0;
    	// This should reset the values at this index.
    	// Clear your data arrays to invalid/null values here.
    	virtual void ClearComponentAtIndexInternal(int _componentIndex) = 0;
    	// for resizing the arrays (vectors)
    	virtual void SetCapacityInternal(int _newCapacity) = 0;

    	/*
		// PROTECTED VIRTUAL FUNCTIONS
		// (Called internally from IComponent)
		// These might not be useful in all cases, so they are not pure virtual.
		*/
    	// Called when (technically just before) a Component is removed.
    	virtual void RemoveComponentInternal(int _componentIndex) {};

    	/*
		// COMPONENT DATA
		// (all Components share these)
		*/

        // The entity this Component belongs to.
        std::vector<int> entityId{};

    private:
    	/*
		// PRIVATE FUNCTIONS
		// (Called internally, derived classes cannot call these)
		*/
    	void SetCapacity(int _newCapacity);
    	int GetNextInactiveComponent();

    	/*
		// PROTECTED MEMBER VARIABLES
		// (derived classes cannot directly access these)
		*/

    	// What unique index this Component has.
    	// So that we can do quick Entity lookups like:
    	// int treeComponentIndex = entityManager->GetComponent( entity, treeComponent->GetComponentIndex() );
    	int componentId = -1;

    	// If this Component has been correctly initialised with World context.
    	bool isInitialised = false;

    	// MAX cap, we cannot make data for more components than this.
    	// This is to prevent crazy cases like infinite loops where we might allocate way too much memory.
    	int maxCapacity = -1;

    	// This is just to keep track of the last component index we activated,
    	// so we can quickly check the next one in order, rather than going from 0 each time.
    	int nextIndex = 0;

    	// up to date list of Entities that have this component.
    	// Updated when this component is added or removed from an entity.
    	// Perhaps not necessary if we're removing a lot of components at runtime?
    	std::vector<int> entityList;

    	bool entityListDirty = true;

    	// The size of this is the length of Entities!
    	// It stores a list of Component Indices for each Entity.
    	std::vector<IndexList> entityComponents{};
    };
} // ECS