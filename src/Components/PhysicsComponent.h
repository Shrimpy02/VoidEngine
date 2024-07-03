#pragma once

// Includes
#include <Components/Component.h>
#include <glm/glm.hpp>

// Additional Includes
#include <memory> 

// Forward Declarations
class Actor;
class VisualActor;

/**
 * @class PhysicsComponent
 * @brief Inherits from component and is responsible for physics handling and behaviour of attached actor.
 * It updates the transform based on velocity and acceleration. 
 */
class PhysicsComponent : public Component
{
	// Re-iterate that Actor classes are friends, even though its called in the base component.
	friend Actor;
public:
	// ---------- Global Variables --------------

private:
	// ---------- Local Variables --------------

	glm::vec3 mVelocity{ 0.f };
	glm::vec3 mAcceleration{ 0.f };
	float mMaxSpeed = 20.f;

	std::shared_ptr<VisualActor> mGroundReference{ nullptr };
	bool inContactWithGround = true;

	double mLastJumpTime;

public:
	// ---------- Global functions --------------

	// Constructs a physics components  and attaches it to input actor
	PhysicsComponent(const std::string& _name, std::shared_ptr<Actor> _owner, std::shared_ptr<VisualActor> _groundReference = nullptr )
		: Component(_name, _owner) {};

	~PhysicsComponent() override;

	// Overidden Update for the component, distribution to all tick functions from here. (Called each frame)
	void Update(float _dt) override;

	// Updates all forces affecting the component.
	void UpdateForces();

	// Updates the volicty of the component based on acceleration and deltatime.
	void UpdateVelocity(float _dt);

	// Updates the position of the component based on acceleration
	void UpdatePosition(float _dt);

	// Resets all physics values, typically called on collisions. (Simple solution)
	void ResetValues();

	// This function makes sure the components owner conforms to ground geometry if mGroundReference is filled.
	void ConformToGround(float _parentExtent);

	// Adds velocity upwards to the actor
	void Jump(float jumpStrength = 10, glm::vec3 _jumpDirection = glm::vec3(0.f, 1.f, 0.f));

private:
	// ---------- Local functions --------------



public:
	// ---------- Getters and setters --------------

	void SetGroundReference(std::shared_ptr<VisualActor> _groundRef) { mGroundReference = _groundRef; }

};