#pragma once

// Includes
#include <Components/Component.h>
#include <glm/glm.hpp>
#include <Core/SMath.h>

// Additional Includes
#include <memory> 

// Forward Declarations
class Actor;
class VisualActor;

/**
 * @class AIComponent
 * @brief Inherits from component and is responsible for ai handling,
 * following path and eventually path finding and more advanced applications.
 */
class AIComponent : public Component
{
	// Re-iterate that Actor classes are friends, even though its called in the base component.
	friend Actor;
public:
	// ---------- Global Variables --------------

	// The assigned path this ai will follow
	std::vector<std::shared_ptr<Actor>> mActivePath;

	std::shared_ptr<Actor> mFollowTarget{ nullptr };

	// The path index for the active path
	int mPathIndex = 1;
	// The movement speed this components owner will move with
	float mMovementSpeed = 5;
	// The range the owner must be within in order to have reached a point
	float mTargetReachedThreshold = 0.1f;

private:
	// ---------- Local Variables --------------


public:
	// ---------- Global functions --------------

	// Constructs an ai components  and attaches it to input actor
	AIComponent(const std::string& _name, std::shared_ptr<Actor> _owner, std::shared_ptr<Actor> _target = nullptr, std::vector<std::shared_ptr<Actor>> path = std::vector<std::shared_ptr<Actor>>{})
		: Component(_name, _owner) {};

	// De-constructor
	~AIComponent() override;

private:
	// ---------- Local functions --------------

public:
	// ---------- Getters and setters --------------

	// Getters

	std::shared_ptr<Actor> GetOwner() { return mOwner; }

};