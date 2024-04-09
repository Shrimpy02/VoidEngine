#pragma once

// Includes
#include <Components/Component.h>
#include <glm/glm.hpp>

/**
 * @class PhysicsComponent
 * @brief Inherits from component and is responsible for physics handling and behaviour of attached actor.
 * It updates the transform based on velocity and acceleration. 
 */
class PhysicsComponent : public Component
{
	// Re-iterate that Actor classes are friends, even though its called in the base component.
	friend class Actor;
public:
	// ---------- Global Variables --------------

private:
	// ---------- Local Variables --------------

	glm::vec3 mVelocity{ 0.f };
	glm::vec3 mAcceleration{ 0.f };
	float mMaxSpeed = 4.f;

	class VisualActor* mGroundReference{ nullptr };

public:
	// ---------- Global functions --------------

	// Constructs a physics components  and attaches it to input actor
	PhysicsComponent(const std::string& _name, class Actor* _owner, class VisualActor* _groundReference = nullptr )
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

	// Returns the barycentric coordinate between three triangle points as a vec3
	glm::vec3 GetBarycentricCoordinates(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _actorPos);

	// Returns the height of a barycentric coordinate in a tringle of three points
	float GetHightFromBarycentricCoordinates(const glm::vec3& _barCoords, const  glm::vec3& _p1, const  glm::vec3& _p2, const  glm::vec3& _p3);


private:
	// ---------- Local functions --------------



public:
	// ---------- Getters and setters --------------

	void SetGroundReference(class VisualActor* _groundRef) { mGroundReference = _groundRef; }

};