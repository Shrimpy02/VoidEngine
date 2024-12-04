
// includes 
#include <Components/CollisionSystem.h>
#include <Components/CollisionComponent.h>
#include <Actor.h>
#include <LevelActors/BaseActor.h>
#include <RenderElements/Mesh.h>




CollisionSystem::CollisionSystem()
	: System() 
{

}

CollisionSystem::~CollisionSystem()
{
}

void CollisionSystem::Update(float _dt)
{
    for (int i = 0; i < mCollisionComponents.size(); i++)
    {
        // Update extent and centre dynamically
        if(std::shared_ptr<BaseActor> baseActor = std::dynamic_pointer_cast<BaseActor>(mCollisionComponents[i].GetOwner()))
        {
            std::pair<glm::vec3, glm::vec3> pair = baseActor->GetActorVisualMesh()->GetMeshMinMaxExtent();
            mCollisionComponents[i].mMinExtent = pair.first;
            mCollisionComponents[i].mMaxExtent = pair.second;

            glm::vec3 extent = (mCollisionComponents[i].mMaxExtent - mCollisionComponents[i].mMinExtent) * glm::vec3(0.5f);

            extent *= baseActor->GetGlobalScale(); // Access via baseActor for clarity

            glm::quat actorRotation = baseActor->GetGlobalRotation();

            glm::vec3 xAxis = actorRotation * glm::vec3(extent.x, 0.0f, 0.0f);
            glm::vec3 yAxis = actorRotation * glm::vec3(0.0f, extent.y, 0.0f);
            glm::vec3 zAxis = actorRotation * glm::vec3(0.0f, 0.0f, extent.z);

            glm::vec3 rotatedExtents(
                glm::abs(xAxis.x) + glm::abs(yAxis.x) + glm::abs(zAxis.x),
                glm::abs(xAxis.y) + glm::abs(yAxis.y) + glm::abs(zAxis.y),
                glm::abs(xAxis.z) + glm::abs(yAxis.z) + glm::abs(zAxis.z));

            mCollisionComponents[i].mExtent = rotatedExtents;
            mCollisionComponents[i].mCenter = ((mCollisionComponents[i].mMinExtent + mCollisionComponents[i].mMaxExtent) * 0.5f) + baseActor->GetGlobalPosition();
        }
    }
}

void CollisionSystem::CheckCollision()
{
	for(int i = 0; i < mCollisionComponents.size(); i++)
	{
		for(int j = i +1 ; j < mCollisionComponents.size(); j++)
		{
			glm::vec3 mtv;
			if(IntersectsAABBx2(mCollisionComponents[i], mCollisionComponents[j], mtv))
			{
                // mtv vector init for each object
                glm::vec3 mtvA(0.f), mtvB(0.f);

                // If both actors are dynamic, split the MTV between them
                if (mCollisionComponents[i].mCollisionProperties.IsDynamic() && mCollisionComponents[j].mCollisionProperties.IsDynamic()) {
                    mtvA = mtv * -0.5f;
                    mtvB = mtv * 0.5f;
                }

                // If only actor A is dynamic, apply the full MTV to A
                else if (mCollisionComponents[i].mCollisionProperties.IsDynamic())
                    mtvA = -mtv;

                // If only actor B is dynamic, apply the full MTV to B
                else if (mCollisionComponents[j].mCollisionProperties.IsDynamic())
                    mtvB = mtv;

                // No adjustment for static objects
                // Apply MTV adjustments to objects it has effected
                if (mCollisionComponents[i].mCollisionProperties.IsDynamic())
                    mCollisionComponents[i].GetOwner()->SetGlobalPosition(mCollisionComponents[i].GetOwner()->GetGlobalPosition() + mtvA);

                if (mCollisionComponents[j].mCollisionProperties.IsDynamic())
                    mCollisionComponents[j].GetOwner()->SetGlobalPosition(mCollisionComponents[j].GetOwner()->GetGlobalPosition() + mtvB);
			}
		}
	}
}

bool CollisionSystem::IntersectsAABBx2(CollisionComponent& _c1, CollisionComponent& _c2, glm::vec3& _mtv)
{
    // calculates the difference from one center to the other
      // and their extent sum
    glm::vec3 diff = _c2.mCenter - _c1.mCenter;
    glm::vec3 sumExtent = _c1.mExtent + _c2.mExtent;

    // Check each axis for non intersection
    for (int i = 0; i < 3; i++)
    {
        // if the difference in length is larger then the sum extent
        // in each axis there is no intersection.
        if (abs(diff[i]) >= sumExtent[i])
        {
            return false; // no intersection for this axis
        }
    }

    // If there is an intersection (ie function has not returned yet)
    // and the caller wants to calculate the minimum translation vector
	// Gets collision values from the check earlier
    glm::vec3 mtvValues;
    for (int i = 0; i < 3; i++)
    {
        mtvValues[i] = sumExtent[i] - abs(diff[i]);
    }

    // Find the minimum non-zero translation vector
    // by iterating through each axis and selecting the smallest non zero value.
    float minMTV = FLT_MAX;
    int minAxis = -1;

    // axis
    for (int i = 0; i < 3; i++)
    {
        // if an axis value is less than minMTV and greater than 0
        if (mtvValues[i] < minMTV && mtvValues[i] > 0)
        {
            // set new minMTV and its axis
            minMTV = mtvValues[i];
            minAxis = i;
        }
    }

    // calculates the direction of the mtv based on the axis with the smallest mtv 
    if (minAxis != -1)
    {
        // Calc direction
        glm::vec3 direction = glm::vec3(0);
        direction[minAxis] = diff[minAxis] > 0.f ? 1.f : -1.f;
        // Assigns the calculated mtv ti the mtv pointer.
        _mtv = direction * minMTV;
    }

    return true; // intersection exists

}

void CollisionSystem::CrateComponent(std::string _name, std::shared_ptr<Actor> _actor, CollisionResponse _inRespones, CollisionType _inType, CollisionBase _inBase)
{
    mCollisionComponents.emplace_back(_name, _actor);
    mCollisionComponents.back().mCollisionProperties.SetCollisionResponse(_inRespones);
    mCollisionComponents.back().mCollisionProperties.SetCollisionType(_inType);
    mCollisionComponents.back().mCollisionProperties.SetCollisionBase(_inBase);
}
