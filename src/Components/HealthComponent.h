#pragma once

// Includes
#include <Components/Component.h>

// Additional Includes
#include <memory> 

// Forward Declarations

/**
 * @class HealthComponent
 * @brief 
 */
class HealthComponent : public Component
{
	// Re-iterate that Actor classes are friends, even though its called in the base component.
	friend Actor;
public:
	// ---------- Global Variables --------------

	int mHealth;

private:
	// ---------- Local Variables --------------


public:
	// ---------- Global functions --------------

	// Constructs an ai components  and attaches it to input actor
	HealthComponent(const std::string& _name, std::shared_ptr<Actor> _owner)
		: Component(_name, _owner) {};

	// De-constructor
	~HealthComponent() override;

private:
	// ---------- Local functions --------------

public:
	// ---------- Getters and setters --------------


};