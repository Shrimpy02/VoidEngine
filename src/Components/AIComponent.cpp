
// includes 
#include <Components/AIComponent.h>
#include <Actor.h>

AIComponent::~AIComponent()
{
}

void AIComponent::Update(float _dt)
{
	if(mActivePath.size() > 0)
		FollowPath(_dt);
}

void AIComponent::FollowPath(float _dt)
{
	// If move to is true it increases the index giving the next position in the vector,
	// if it is false it moves the owning actor but does not iterate to the next point
	if(MoveTo(mActivePath[mPathIndex].mPosition, _dt))
	{
		// Updates the target point to the next in the path vector
		mPathIndex++;

		// if the index has reached the end of the path return to index 0. 
		if (mPathIndex >= mActivePath.size())
			mPathIndex = 1;
		
	}
}

bool AIComponent::MoveTo(const glm::vec3& _targetPoint, float _dt)
{
	// Gets the current position to the owning actor
	glm::vec3 currentPos = mOwner->GetPosition(Actor::TransformSpace::Global);

	// Gets the distance from owner position to target position to create a threshold range for reaching the position
	float distance = glm::distance(currentPos, _targetPoint);

	if (distance <= mTargetReachedThreshold)
		return true;
	else
	{
		// if target is not reached move the owner toward the target
		glm::vec3 direction = _targetPoint - currentPos;
		glm::vec3 normDirection = glm::normalize(direction);
		glm::vec3 movement = normDirection * _dt * mMovementSpeed;
		mOwner->SetPosition(currentPos + movement, Actor::TransformSpace::Global);
	}
	return false;
}


