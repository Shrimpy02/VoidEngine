#pragma once

// Includes
#include <Components/Component.h>
#include <glm/glm.hpp>
#include <Core/SMath.h>

/**
 * @class AIComponent
 * @brief Inherits from component and is responsible for ai handling,
 * following path and eventually path finding and more advanced applications.
 */
class AIComponent : public Component
{
	// Re-iterate that Actor classes are friends, even though its called in the base component.
	friend class Actor;
public:
	// ---------- Global Variables --------------

private:
	// ---------- Local Variables --------------

	// The assigned path this ai will follow
	std::vector<Points> mActivePath;
		// The path index for the active path
	int mPathIndex = 1;

	// The movement speed this components owner will move with
	float mMovementSpeed = 5;
	// The range the owner must be within in order to have reached a point
	float mTargetReachedThreshold = 0.1f;


public:
	// ---------- Global functions --------------

	// Constructs an ai components  and attaches it to input actor
	AIComponent(const std::string& _name, class Actor* _owner, class VisualActor* _groundReference = nullptr)
		: Component(_name, _owner) {};

	// De-constructor
	~AIComponent() override;

	// Overidden Update for the component, distribution to all tick functions from here. (Called each frame)
	void Update(float _dt) override;

	// Follows mActivePath if there is one assigned to component. 
	void FollowPath(float _dt);

	// Returns true if owning actor has reached target position, moves owning actor for each tick.
	bool MoveTo(const glm::vec3& _targetPoint, float _dt);


private:
	// ---------- Local functions --------------



public:
	// ---------- Getters and setters --------------

	// Getters

	// Returns the active path for this component as an address to this classes vector of points
	std::vector<Points>& GetActivePath() { return mActivePath; }

	// Returns the movement speed this components owner uses in ai context as a float
	float GetMovementSpeed() { return mMovementSpeed; }

	// Returns the threshold this components owner needs to be within to have reached a point as a float
	float GetTargetReachedThreshold() { return mTargetReachedThreshold; }


	// Setters

	// Sets mActivePath from input vector of points
	void SetActivePath(std::vector<Points> _inPath) { mActivePath = _inPath; }

	// Sets the movement speed this components owner will move with
	void SetMovementSpeed(const float _inMoveSpeed) { mMovementSpeed = _inMoveSpeed; }

	// Sets the threshold this components owner needs to be within to have reached a point
	void SetTargetReachedThreshold(const float _inThreshold) { mTargetReachedThreshold = _inThreshold; }

};