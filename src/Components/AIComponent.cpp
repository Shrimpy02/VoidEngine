
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

	if(MoveTo(mActivePath[mPathIndex].mPosition, _dt))
	{
		mPathIndex++;

		if (mPathIndex >= mActivePath.size())
			mPathIndex = 0;
		
	}
}

bool AIComponent::MoveTo(const glm::vec3& _targetPoint, float _dt)
{
	glm::vec3 currentPos = mOwner->GetPosition(Actor::TransformSpace::Global);

	float distance = glm::distance(currentPos, _targetPoint);
	float limit = 0.1f;

	if (distance <= limit)
		return true;
	else
	{
		glm::vec3 direction = _targetPoint - currentPos;
		glm::vec3 normDirection = glm::normalize(direction);
		glm::vec3 movement = normDirection * _dt * mMovementSpeed;
		mOwner->SetPosition(currentPos + movement, Actor::TransformSpace::Global);
	}
	return false;
}


