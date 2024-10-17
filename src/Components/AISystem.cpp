
// includes 
#include <Components/AISystem.h>
#include <Actor.h>

AISystem::~AISystem()
{
}

void AISystem::Update(float _dt)
{

	for(std::shared_ptr<AIComponent> comp : mAIComponents)
	{
		if(mAiFollowPath == AiFollowType::AiFT_FollowPath)
		{
			if (comp->mActivePath.size() > 0)
				FollowPath(comp, _dt);
		} else if (mAiFollowPath == AiFollowType::AiFT_FollowObject)
		{
			if (comp->mFollowTarget)
				FollowTarget(comp, _dt);
		}
	}
}

void AISystem::FollowPath(std::shared_ptr<AIComponent> _aiComponent, float _dt)
{
	// If move to is true it increases the index giving the next position in the vector,
	// if it is false it moves the owning actor but does not iterate to the next point
	if (MoveTo(_aiComponent, _aiComponent->mActivePath[_aiComponent->mPathIndex]->GetGlobalPosition(), _dt))
	{
		// Updates the target point to the next in the path vector
		_aiComponent->mPathIndex++;

		// if the index has reached the end of the path return to index 0. 
		if (_aiComponent->mPathIndex >= _aiComponent->mActivePath.size())
			_aiComponent->mPathIndex = 1;
	}
}

void AISystem::FollowTarget(std::shared_ptr<AIComponent> _aiComponent, float _dt)
{
	if (MoveTo(_aiComponent, _aiComponent->mFollowTarget->GetGlobalPosition(), _dt))
	{
		
	}
}

bool AISystem::MoveTo(std::shared_ptr<AIComponent> _aiComponent, const glm::vec3& _targetPoint, float _dt)
{
	if (!_aiComponent || !_aiComponent->GetOwner()) return false;

	// Gets the current position to the owning actor
	glm::vec3 currentPos = _aiComponent->GetOwner()->GetGlobalPosition();

	// Gets the distance from owner position to target position to create a threshold range for reaching the position
	float distance = glm::distance(currentPos, _targetPoint);

	if (distance <= _aiComponent->mTargetReachedThreshold)
		return true;
	else
	{
		// if target is not reached move the owner toward the target
		glm::vec3 direction = _targetPoint - currentPos;
		glm::vec3 normDirection = glm::normalize(direction);
		glm::vec3 movement = normDirection * _dt * _aiComponent->mMovementSpeed;
		_aiComponent->GetOwner()->SetGlobalPosition(currentPos + movement);
	}
	return false;
}

void AISystem::AddComponent(std::shared_ptr<AIComponent> _inComponent)
{
	mAIComponents.push_back(_inComponent);
}

std::vector<std::shared_ptr<Actor>>& AISystem::GetActivePath(std::shared_ptr<AIComponent> _aiComp)
{
	return _aiComp->mActivePath;
}

float AISystem::GetMovementSpeed(std::shared_ptr<AIComponent> _aiComp)
{
	return _aiComp->mMovementSpeed;
}

float AISystem::GetTargetReachedThreshold(std::shared_ptr<AIComponent> _aiComp)
{
	return _aiComp->mTargetReachedThreshold;
}

void AISystem::SetTarget(std::shared_ptr<AIComponent> _aiComp, std::shared_ptr<Actor> _inActor)
{
	_aiComp->mFollowTarget = _inActor;
}

void AISystem::SetActivePath(std::shared_ptr<AIComponent> _aiComp, std::vector<std::shared_ptr<Actor>> _inPath)
{
	_aiComp->mActivePath = _inPath;
}

void AISystem::SetMovementSpeed(std::shared_ptr<AIComponent> _aiComp, const float _inMoveSpeed)
{
	_aiComp->mMovementSpeed = _inMoveSpeed;
}

void AISystem::SetTargetReachedThreshold(std::shared_ptr<AIComponent> _aiComp, const float _inThreshold)
{
	_aiComp->mTargetReachedThreshold = _inThreshold;
}



