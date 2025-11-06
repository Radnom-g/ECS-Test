//
// Created by Radnom on 3/11/2025.
//

#pragma once
#include <map>
#include <memory>
#include <vector>

#include "ComponentSettings.h"

namespace ECS
{
	struct WorldContext;
	// A Component stores plain data with no behaviour.
    // it maintains a vector for each piece of relevant data.
    class IComponent
    {
    public:
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
		bool InitialiseComponent(WorldContext* context, int _initialCapacity, int _maxCapacity);
    	int AddComponent(int _entityId);
    	void RemoveComponentsFromEntity(int _entityId);
    	void RemoveComponent(int _componentIndex);

    	// Call once per physics frame.
    	// Clears out dirty flags.
    	void ProcessPhysics(float _delta);

        [[nodiscard]] bool HasComponent(int _entityId) const;

        void GetEntitiesWithComponent(std::vector<int>& _outEntityList);
		void FilterKeepEntitiesWithComponent(std::vector<int>& _entityListFilter);
    	void FilterKeepEntitiesWithoutComponent(std::vector<int>& _entityListFilter);

    	// Get component by index for direct access to data. However, if the component becomes inactive, it can be
    	// reused by another Entity, so don't hang on to the index!
    	// -1 means invalid.
    	int GetComponentIndex(int _entityId) const;
    	// This will return a list of component indices belonging to this Entity.
    	bool GetComponentIndices(int _entityId, std::vector<int>& _componentIndexList);

    	[[nodiscard]] bool IsInitialised() const { return isInitialised; }


    	/*
    	// PUBLIC PURE VIRTUAL FUNCTIONS:
    	*/
    	// The name of the component. i.e. "ThingComponent".
        [[nodiscard]] virtual const char* GetName() const = 0;
    	// if a single Entity can have multiple of this Component.
        [[nodiscard]] virtual bool IsUniquePerEntity() const = 0;

    	// if this Component can interpolate between physics frames.
    	// This determines if it cares about 'dirty'.
    	[[nodiscard]] virtual bool CanInterpolate() const = 0;


    protected:

    	/*
		// PROTECTED FUNCTIONS
		*/
    	void RemoveComponentByIndex(int _componentIndex);
    	// erase all values at given index
    	void ClearComponentAtIndex(int _componentIndex);
    	// returns the index of a component ID.
    	inline int EntityIdToComponentIndex(int _entityId) const { return entityIdComponentIndex.at(_entityId); }
    	// returns the start->end of the range of components (by index) belonging to this entity.
    	inline std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> EntityIdToComponentRange(int _entityId) { auto thing = entityIdComponentIndexMap.equal_range(_entityId); return thing; }


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
    	// Called end of physics tick.
    	// This is where Components with interpolation should set prev states to current.
    	// If it has no interpolation, leave empty.
    	virtual void ProcessPhysicsInternal(float _delta) = 0;

    	/*
		// PROTECTED VIRTUAL FUNCTIONS
		// (Called internally from IComponent)
		// These might not be useful in all cases, so they are not pure virtual.
		*/
    	// Called when (technically just before) a Component is removed.
    	virtual void RemoveComponentsFromEntityInternal(int _entityId) {};
    	// Called when (technically just before) a Component is removed.
    	virtual void RemoveComponentInternal(int _componentIndex) {};

    	/*
		// COMPONENT DATA
		// (all Components share these)
		*/

        // The entity this Component belongs to.
        std::vector<int> entityId{};
        // Whether this component is currently Active.
        std::vector<bool> active{};

    	// If this component is 'dirty' and can interpolate.
    	std::vector<int> dirty{};

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

    	// If this Component has been correctly initialised with World context.
    	bool isInitialised = false;

    	// MAX cap, we cannot make data for more components than this.
    	// This is to prevent crazy cases like infinite loops where we might allocate way too much memory.
    	int maxCapacity = -1;

    	// A component's index will remain constant, but the ID will change
    	// so that we can track when a component is destroyed.

    	// This is just to keep track of the last component index we activated,
    	// so we can quickly check the next one in order, rather than going from 0 each time.
    	int nextIndex = 0;

    	// Map of entity ID to the components that belong to it (via index).
    	// Used if 'IsUniquePerEntity' is false.
    	std::multimap<int, int> entityIdComponentIndexMap;

    	// Map of entity ID to the component (single) that belong to it (via index).
    	// Used if 'IsUniquePerEntity' is true.
    	std::map<int, int> entityIdComponentIndex;

    	// up to date list of Entities that have this component.
    	std::vector<int> entityList;

    	bool entityListDirty = true;
    };
} // ECS