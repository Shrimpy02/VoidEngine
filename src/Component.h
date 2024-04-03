#pragma once
// Includes
#include <Tag.h>

/**
 * @class Component
 * @brief A Component base that can be added to actors for additional functionality.
 * Contains default init and update functions.
 */
class Component
{
	// Friend classes should technically be avoided because they break encapsulation
	// But will be used in this case cause we only want an actor to create, init and update components
	// For all derived classes add actor as a friend
	friend class Actor;
public:
	// ---------- Global Variables --------------

protected:
	// The component owner
	class Actor* mOwner;

private:
	// ---------- Local Variables --------------
	// Components name
	TagUnique mTag;

public:
	// ---------- Global functions --------------

	// Remove copy and move functionality
	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;
	Component(Component&&) = delete;
	Component& operator=(Component&&) = delete;

	// De-constructor
	virtual ~Component() { }

protected:
	// Constructor
	Component(const std::string& _name, class Actor* _owner)
		:mTag(_name), mOwner(_owner)
	{}

	// Init runs when the component is created and added to an actor
	virtual void Init() {}
	// is called each frame
	virtual void Update(float _dt) {}

private:
	// ---------- Local functions --------------



public:
	// ---------- Getters and setters --------------

	// Getters


};