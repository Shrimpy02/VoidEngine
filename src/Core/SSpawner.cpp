
// Includes
#include <Core/SSpawner.h>
#include <Core/SMath.h>
#include <LevelActors/VisualActor.h>
#include <LevelActors/BaseActor.h>

// Additional includes

void SSpawner::SetObjectLocationWithinBoundsRandomly(std::vector<std::shared_ptr<BaseActor>>& _objects, std::shared_ptr<VisualActor> _confineObject)
{
	// Finds the largest extent of all objects
	glm::vec3 largestExtent{glm::vec3(0)};
	for(std::shared_ptr<Actor> actor : _objects)
	{
		if(std::shared_ptr<IBounded> boundedActor = std::dynamic_pointer_cast<IBounded>(actor))
		{
			if (boundedActor->mExtent.x > largestExtent.x &&
				boundedActor->mExtent.y > largestExtent.y &&
				boundedActor->mExtent.z > largestExtent.z)
				largestExtent = boundedActor->mExtent;
		}
	}

	// Gets the extent of the confine box
	glm::vec3 boxMin = _confineObject->mCenter - _confineObject->mExtent;
	glm::vec3 boxMax = _confineObject->mCenter + _confineObject->mExtent;

	// Sets each actor location within the confine box extent
	for (std::shared_ptr<Actor> actor : _objects)
	{
		float posX = SMath::GetRandomFloatBetweenMinMax(boxMin.x, boxMax.x);
		float posY = SMath::GetRandomFloatBetweenMinMax(boxMin.y, boxMax.y);
		float posZ = SMath::GetRandomFloatBetweenMinMax(boxMin.z, boxMax.z);
		
		actor->SetGlobalPosition(glm::vec3(posX, posY, posZ));
		
	}

	// Clear actor references
	_objects.clear();
}


void SSpawner::SetObjectLocationWithinBoundsRandomly(std::shared_ptr<BaseActor> _object, std::shared_ptr<VisualActor> _confineObject)
{
	// Finds the largest extent of all objects
	glm::vec3 largestExtent{ glm::vec3(0) };
	if (std::shared_ptr<IBounded> boundedActor = std::dynamic_pointer_cast<IBounded>(_object))
	{
		if (boundedActor->mExtent.x > largestExtent.x &&
			boundedActor->mExtent.y > largestExtent.y &&
			boundedActor->mExtent.z > largestExtent.z)
			largestExtent = boundedActor->mExtent;
	}

	// Gets the extent of the confine box
	glm::vec3 boxMin = _confineObject->mCenter - _confineObject->mExtent;
	glm::vec3 boxMax = _confineObject->mCenter + _confineObject->mExtent;

	float posX = SMath::GetRandomFloatBetweenMinMax(boxMin.x, boxMax.x);
	float posY = SMath::GetRandomFloatBetweenMinMax(boxMin.y, boxMax.y);
	float posZ = SMath::GetRandomFloatBetweenMinMax(boxMin.z, boxMax.z);

	_object->SetGlobalPosition(glm::vec3(posX, posY, posZ));
}

void SSpawner::SetObjectLocationWithinBoundsRandomlyIgnoreY(std::shared_ptr<BaseActor> _object,
	std::shared_ptr<VisualActor> _confineObject)
{
	// Gets the extent of the confine box
	glm::vec3 boxMin = _confineObject->mCenter - _confineObject->mExtent;
	glm::vec3 boxMax = _confineObject->mCenter + _confineObject->mExtent;

	float posX = SMath::GetRandomFloatBetweenMinMax(boxMin.x, boxMax.x);
	float posZ = SMath::GetRandomFloatBetweenMinMax(boxMin.z, boxMax.z);

	_object->SetGlobalPosition(glm::vec3(posX, 1, posZ));
}
