#pragma once

// Includes
#include <Components/System.h>

// Additional Includes
#include <memory> 

// Forward Declarations
class PhysicsComponent;

// Enums

/**
 * @class PhysicsSystem
 * @brief
 */
class PhysicsSystem : public System
{
public:
	// ---------- Global Variables --------------

private:
	// ---------- Local Variables --------------

	// All AI Components
	std::vector<std::shared_ptr<PhysicsComponent>> mPhysicsComponents;

public:
	// ---------- Global functions --------------

	//
	PhysicsSystem() : System() {};

	// De-constructor
	~PhysicsSystem() override;

	void Update(float _dt);


private:
	// ---------- Local functions --------------



public:
	// ---------- Getters and setters --------------

	void AddComponent(std::shared_ptr<PhysicsComponent> _inComponent);

};