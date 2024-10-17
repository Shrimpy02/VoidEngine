#pragma once

// Includes
#include <Components/System.h>
#include <glm/glm.hpp>
#include <Core/SMath.h>

// Additional Includes
#include <memory> 

// Forward Declarations
class Actor;
class VisualActor;
class AIComponent;

// Enums
enum AiFollowType
{
	AiFT_FollowBase,
	AiFT_FollowPath,
	AiFT_FollowObject
};

/**
 * @class AISystem
 * @brief 
 */
class AISystem : public System
{
public:
	// ---------- Global Variables --------------

private:
	// ---------- Local Variables --------------

	// All AI Components
	std::vector<std::shared_ptr<AIComponent>> mAIComponents;

	AiFollowType mAiFollowPath = AiFT_FollowBase;

public:
	// ---------- Global functions --------------

	// Constructs an ai components  and attaches it to input actor
	AISystem(AiFollowType inFollowType) : System(), mAiFollowPath(inFollowType) {};

	// De-constructor
	~AISystem() override;

	// Overidden Update for the component, distribution to all tick functions from here. (Called each frame)
	void Update(float _dt);


private:
	// ---------- Local functions --------------

		// Follows mActivePath if there is one assigned to component. 
	void FollowPath(std::shared_ptr<AIComponent> _aiComponent, float _dt);

	void FollowTarget(std::shared_ptr<AIComponent> _aiComponent, float _dt);

	// Returns true if owning actor has reached target position, moves owning actor for each tick.
	bool MoveTo(std::shared_ptr<AIComponent> _aiComponent, const glm::vec3& _targetPoint, float _dt);


public:
	// ---------- Getters and setters --------------

	void AddComponent(std::shared_ptr<AIComponent> _inComponent);

	// Getters

	// Returns the active path for this component as an address to this classes vector of points
	std::vector<std::shared_ptr<Actor>>& GetActivePath(std::shared_ptr<AIComponent> _aiComp);

	// Returns the movement speed this components owner uses in ai context as a float
	float GetMovementSpeed(std::shared_ptr<AIComponent> _aiComp);

	// Returns the threshold this components owner needs to be within to have reached a point as a float
	float GetTargetReachedThreshold(std::shared_ptr<AIComponent> _aiComp);


	// Setters

	void SetTarget(std::shared_ptr<AIComponent> _aiComp, std::shared_ptr<Actor> _inActor);

	// Sets mActivePath from input vector of points
	void SetActivePath(std::shared_ptr<AIComponent> _aiComp, std::vector<std::shared_ptr<Actor>> _inPath);

	// Sets the movement speed this components owner will move with
	void SetMovementSpeed(std::shared_ptr<AIComponent> _aiComp, const float _inMoveSpeed);

	// Sets the threshold this components owner needs to be within to have reached a point
	void SetTargetReachedThreshold(std::shared_ptr<AIComponent> _aiComp, const float _inThreshold);

};