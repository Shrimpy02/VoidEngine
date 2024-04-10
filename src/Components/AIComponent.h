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

	std::vector<Points> mActivePath; 
	float mMovementSpeed = 5;
	int mPathIndex = 0;
public:
	// ---------- Global functions --------------

	// Constructs a physics components  and attaches it to input actor
	AIComponent(const std::string& _name, class Actor* _owner, class VisualActor* _groundReference = nullptr)
		: Component(_name, _owner) {};

	~AIComponent() override;

	// Overidden Update for the component, distribution to all tick functions from here. (Called each frame)
	void Update(float _dt) override;

	void FollowPath(float _dt);


	bool MoveTo(const glm::vec3& _targetPoint, float _dt);


private:
	// ---------- Local functions --------------



public:
	// ---------- Getters and setters --------------

	void SetActivePath(std::vector<Points> _inPath) { mActivePath = _inPath; };

};