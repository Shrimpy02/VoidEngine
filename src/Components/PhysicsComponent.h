#pragma once

// Includes
#include <Components/Component.h>

// Additional Includes
#include <memory>
#include <glm/glm.hpp>

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

	std::vector<glm::vec3> mDebugBarryLocations;

	std::shared_ptr<VisualActor> mSurfaceReference{ nullptr };

	bool inContactWithGround = true;

	bool mGravityEnabled = true;
	double mLastJumpTime = 0.f;

public:
	// ---------- Global functions --------------

	// Constructs a physics components  and attaches it to input actor
	PhysicsComponent(const std::string& _name, std::shared_ptr<Actor> _owner)
		: Component(_name, _owner) {}

	~PhysicsComponent() override = default;

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

	// Resets acceleration and velocity
	void ResetForces();


	// This function makes sure the components owner conforms to ground geometry if mGroundReference is filled.
	void ConformToSurface();

	// Adds velocity upwards to the actor
	void Jump(float jumpStrength = 10, glm::vec3 _jumpDirection = glm::vec3(0.f, 1.f, 0.f));

private:
	// ---------- Local functions --------------



public:
	// ---------- Getters and setters --------------

	bool HasSurfaceReference() { if (mSurfaceReference) return true; return false; };

	std::shared_ptr<VisualActor> GetSurfaceReference() { return mSurfaceReference; }

	std::vector<glm::vec3> GetDebugSurfaceBarycentricPoints() { return mDebugBarryLocations; }

	void SetSurfaceReference(std::shared_ptr<VisualActor> _groundRef) { mSurfaceReference = _groundRef; }

	void SetGravityEnabled(bool _b) { mGravityEnabled = _b; }

	bool IsGravityEnabled() { return mGravityEnabled; }

};