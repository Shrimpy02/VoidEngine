
// Includes
#include <Collision/Collision.h>
#include <Utilities/Logger.h>
#include <LevelActors/VisualActor.h>
#include <OctTree.h>

// Additional Includes


bool IBounded::IsIntersecting(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv)
{
	    // Both are AABB
	    // -----------------------------------
    if (mCollisionProperties.IsAABB() && _otherCollider->mCollisionProperties.IsAABB())
    {
        if(AABBx2(_otherCollider, _mtv))
        {
            // Both are convex
			// -----------------------------------
           if(mCollisionProperties.IsConvex() && _otherCollider->mCollisionProperties.IsConvex())
           {
				if(Convexx2(_otherCollider, _mtv))
				{
                    return true;
				}

           	// One is Convex and other is not convex
           	// -----------------------------------
           } else if (mCollisionProperties.mSubBase != _otherCollider->mCollisionProperties.mSubBase){

               if(ConvexxNotConvex(_otherCollider, _mtv))
               {
                   return true;
               }

           }

            return true;
        }

        return false;

        // Both are BoundingSphere
        // -----------------------------------
    } else if (mCollisionProperties.IsBoundingSphere() && _otherCollider->mCollisionProperties.IsBoundingSphere()) {

        if(BoundingSpherex2(_otherCollider, _mtv))
        {
	        // Additional tests go here
            return true; 
        }

        return false;

        // One is AABB and other is BoundingSphere
        // -----------------------------------
    } else if (mCollisionProperties.mBase != _otherCollider->mCollisionProperties.mBase) {

        if(AABBxBoundingSphere(_otherCollider, _mtv))
        {
            // Additional tests go here
            return true;
        }

        return false;
    }

		// All Failed
	LOG_ERROR("Collision Base Missmatch");
    return false;
}

bool IBounded::IsIntersectingLineTrace(glm::vec3 _point)
{
	// Linetrace vs AABB
	// -----------------------------------
    if (mCollisionProperties.IsAABB())
    {
        if (AABBxPoint(_point))
        {
           
            return true;
        }

        return false;

        // Linetrace vs BoundingSphere
        // -----------------------------------
    }
    else if (mCollisionProperties.IsBoundingSphere()) {

        if (BoundingSpherexPoint(_point))
        {
           
            return true;
        }

        return false;
    }

    // All Failed
    LOG_ERROR("Collision Base Missmatch");
    return false;
}

bool IBounded::IsIntersectingConstrictingBoxGeometry(std::shared_ptr<VisualActor> _boxCollider, glm::vec3* _mtv, glm::vec3* _boundaryNormal)
{
	if (mCollisionProperties.IsBoundingSphere()) {

        return BoundingSpherexConstrictingBox(_boxCollider, _mtv, _boundaryNormal);
    }

    return false;
}

bool IBounded::IsIntersectingConstrictingBoxGeometry(glm::vec3 _min, glm::vec3 _max)
{
    if (mCollisionProperties.IsBoundingSphere()) {

        return BoundingSpherexConstrictingBoxContain(_min, _max);
    }

    return false;
}

bool IBounded::AABBx2(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv)
{
    // calculates the difference from one center to the other
       // and their extent sum
    glm::vec3 diff = _otherCollider->mCenter - this->mCenter;
    glm::vec3 sumExtent = this->mExtent + _otherCollider->mExtent;

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
    if (_mtv)
    {
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
            *_mtv = direction * minMTV;
        }
    }

    return true; // intersection exists
}

bool IBounded::BoundingSpherex2(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv)
{
    // Calculate the vector between the centers of the two spheres
    glm::vec3 diff = _otherCollider->mCenter - mCenter;

    // Calculate the squared distance between the centers
    float distanceSquared = glm::dot(diff, diff);

    // Calculate the sum of the radii
    float sumRadii = mRadius + _otherCollider->mRadius;

    // Check if the squared distance is less than or equal to the squared sum of the radii
    bool intersects = distanceSquared <= sumRadii * sumRadii;

    // If the spheres are not intersecting, return false
    if (!intersects)
        return false;

    // If the caller wants to calculate the minimum translation vector
    if (_mtv)
    {
        // Calculate the distance between the centers
        float distance = glm::sqrt(distanceSquared);

        // Calculate the penetration depth
        float penetration = sumRadii - distance;

        // Calculate the direction of the MTV
        glm::vec3 direction = glm::normalize(diff);

        if (direction.x != direction.x)
            direction = glm::vec3(-1);

        // Multiply the direction by the penetration to get the MTV
        *_mtv = direction * penetration;
    }

    // Intersection exists
    return true;
}

bool IBounded::AABBxBoundingSphere(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv)
{
    // if this object is aabb other is bounding sphere
    if(mCollisionProperties.IsAABB())
    {
        // Gets the point along the AABB box closest to the sphere 
        glm::vec3 clampedAABB = glm::clamp(_otherCollider->mCenter, mCenter - mExtent, mCenter + mExtent);

        // Gets the vector between the bounding sphere center and the aabb closest point
        glm::vec3 diff = clampedAABB - _otherCollider->mCenter;

        // gets the distance between these points
        float distanceSquared = glm::dot(diff, diff);

        // is true if the distance is more than the radius squared
        bool intersects = distanceSquared <= (_otherCollider->mRadius * _otherCollider->mRadius);

        if (!intersects) // if check so the rest of the code does not gray out and become a pain.
            return false; // should only proc return false so that no more computing is done for this scenario 

        // Calculate the minimum translation vector for the collision
        if (_mtv)
        {
            // Calculate the penetration depth
            float distance = glm::sqrt(distanceSquared);
            float penetration = _otherCollider->mRadius - distance;

            // Calculate the direction of the MTV
            glm::vec3 direction = glm::normalize(diff);

            // Multiply the direction by the penetration to get the MTV
            *_mtv = direction * penetration;
            // inverse mtv direction
            *_mtv *= -1;
        }

        // else this is bounding sphere and other is aabb
    } else {

		// Gets the point along the AABB box closest to the sphere
        glm::vec3 clampedAABB = glm::clamp(mCenter, _otherCollider->mCenter - _otherCollider->mExtent, _otherCollider->mCenter + _otherCollider->mExtent);

        // Gets the vector between the bounding sphere center and the AABB closest point
        glm::vec3 diff = clampedAABB - mCenter;

        // Gets the distance between these points
        float distanceSquared = glm::dot(diff, diff);

        // Is true if the distance is more than the radius squared
        bool intersects = distanceSquared <= (mRadius * mRadius);

        if (!intersects) // If check so the rest of the code does not gray out and become a pain.
            return false; // Should only proc return false so that no more computing is done for this scenario 

        // Calculate the minimum translation vector for the collision
        if (_mtv)
        {
            // Calculate the penetration depth
            float distance = glm::sqrt(distanceSquared);
            float penetration = mRadius - distance;

            // Calculate the direction of the MTV
            glm::vec3 direction = glm::normalize(diff);

            // Multiply the direction by the penetration to get the MTV
            *_mtv = direction * penetration;
        }

        return true;

    }

    return true; // intersection exists if function has come this far
}

bool IBounded::AABBxBoundingSphere(std::pair <glm::vec3, glm::vec3> _extents)
{

    glm::vec3 centre = glm::vec3{
(_extents.first.x + _extents.second.x) / 2.f,
(_extents.first.y + _extents.second.y) / 2.f,
(_extents.first.z + _extents.second.z) / 2.f
    };

    // Gets the point along the AABB box closest to the sphere
    glm::vec3 clampedAABB = glm::clamp(mCenter, centre - _extents.first, _extents.second);

    // Gets the vector between the bounding sphere center and the AABB closest point
    glm::vec3 diff = clampedAABB - mCenter;

    // Gets the distance between these points
    float distanceSquared = glm::dot(diff, diff);

    // Is true if the distance is more than the radius squared
    bool intersects = distanceSquared <= (mRadius * mRadius);

    if (!intersects) // If check so the rest of the code does not gray out and become a pain.
        return false; // Should only proc return false so that no more computing is done for this scenario 

    return true;
}

bool IBounded::Convexx2(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv)
{
    return false;
}

bool IBounded::ConvexxNotConvex(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv)
{
    return false;
}

bool IBounded::AABBxPoint(glm::vec3 _pointPos)
{
	// calculates the difference from one center to the other
    // and their extent sum
    glm::vec3 diff = _pointPos - this->mCenter;
    glm::vec3 sumExtent = this->mExtent;

    // Check each axis for non intersection
    for (int i = 0; i < 3; i++)
    {
        // if the difference in length is larger then the sum extent
        // in each axis there is no intersection.
        if (abs(diff[i]) > sumExtent[i])
        {
            return false; // no intersection for this axis
        }
    }
    return true;
}

bool IBounded::BoundingSpherexPoint(glm::vec3 _pointPos)
{
    // Calculate the vector from the center of the sphere to the point
    glm::vec3 diff = _pointPos - mCenter;

    // Calculate the squared distance between the center and the point
    float distanceSquared = glm::dot(diff, diff);

    // Compare the squared distance with the squared radius of the sphere
    return distanceSquared <= (mRadius * mRadius);
}

bool IBounded::BoundingSpherexConstrictingBox(std::shared_ptr<VisualActor> _boxCollider, glm::vec3* _mtv, glm::vec3* _boundaryNormal)
{
    // Get the closest point on the AABB to the sphere's center
    glm::vec3 clampedAABB = glm::clamp(mCenter, _boxCollider->mCenter - _boxCollider->mExtent+ (mRadius*2), _boxCollider->mCenter + _boxCollider->mExtent- (mRadius*2));

    // Compute the vector between the sphere's center and the closest point on the AABB
    glm::vec3 diff = clampedAABB - mCenter;

    // Calculate the square of the distance between the sphere's center and the closest point
    float distanceSquared = glm::dot(diff, diff);

    // Check if the sphere is inside the AABB (i.e., distance from the center to the closest point is less than the sphere's radius)
    bool inside = distanceSquared <= (mRadius * mRadius);

    // Early out: if the sphere is inside the AABB, return false
    if (inside)
    {
        return false;
    }

    // If the sphere is outside the AABB, calculate the minimum translation vector (MTV)
    if (_mtv)
    {
        // Calculate the actual distance between the sphere's center and the closest point on the AABB
        float distance = glm::sqrt(distanceSquared);

        // Calculate the penetration depth (how much the sphere is penetrating the AABB)
        float penetration = mRadius - distance;

        // Calculate the direction of the MTV by normalizing the difference vector
        glm::vec3 direction = glm::normalize(diff);

        // Compute the MTV by multiplying the direction by the penetration depth
        *_mtv = direction * -penetration;

        //if (_boundaryNormal)
        //{
        //    *_boundaryNormal = -direction;  // Normal points outward from the boundary
        //}

        // TODO Must be a better way to do this::
        if (_boundaryNormal)
        {
            glm::vec3 boxMin = _boxCollider->mCenter - _boxCollider->mExtent;
            glm::vec3 boxMax = _boxCollider->mCenter + _boxCollider->mExtent;
            glm::vec3 distances = glm::abs(clampedAABB - mCenter);  // Distances to each side of the box
        
            // Find the smallest difference to determine the nearest axis
            if (distances.x > distances.y && distances.x > distances.z)
            {
                // Closest to X-axis boundary
                _boundaryNormal->x = (mCenter.x > _boxCollider->mCenter.x) ? 1.0f : -1.0f;
                _boundaryNormal->y = 0.0f;
                _boundaryNormal->z = 0.0f;
            }
            else if (distances.y > distances.x && distances.y > distances.z)
            {
                // Closest to Y-axis boundary
                _boundaryNormal->x = 0.0f;
                _boundaryNormal->y = (mCenter.y > _boxCollider->mCenter.y) ? 1.0f : -1.0f;
                _boundaryNormal->z = 0.0f;
            }
            else
            {
                // Closest to Z-axis boundary
                _boundaryNormal->x = 0.0f;
                _boundaryNormal->y = 0.0f;
                _boundaryNormal->z = (mCenter.z > _boxCollider->mCenter.z) ? 1.0f : -1.0f;
            }
        }
    }

    // Return true since the sphere is outside the AABB and the MTV has been calculated
    return true;
}

bool IBounded::BoundingSpherexConstrictingBoxContain(glm::vec3 _minExtent, glm::vec3 _maxExtent)
{
    // Get the closest point on the AABB to the sphere's center
    glm::vec3 clampedAABB = glm::clamp(mCenter, _minExtent + (mRadius * 2), _maxExtent - (mRadius * 2));

    // Compute the vector between the sphere's center and the closest point on the AABB
    glm::vec3 diff = clampedAABB - mCenter;

    // Calculate the square of the distance between the sphere's center and the closest point
    float distanceSquared = glm::dot(diff, diff);

    // Check if the sphere is inside the AABB (i.e., distance from the center to the closest point is less than the sphere's radius)
    bool inside = distanceSquared <= (mRadius * mRadius);

    // Correct return: if the sphere is inside the AABB, return true
    return inside;
}

bool IBounded::BoundingSpherexConstrictingBoxIntersect(glm::vec3 _minExtent, glm::vec3 _maxExtent)
{
    // Clamp the sphere's center to the nearest point on the AABB
    glm::vec3 clampedPoint = glm::clamp(mCenter, _minExtent, _maxExtent);

    // Compute the vector between the sphere's center and the closest point on the AABB
    glm::vec3 diff = clampedPoint - mCenter;

    // Calculate the square of the distance between the sphere's center and the closest point
    float distanceSquared = glm::dot(diff, diff);

    // Check if the distance is less than or equal to the sphere's radius squared
    bool intersecting = distanceSquared <= (mRadius * mRadius);

    return intersecting;
}

void IBounded::BoundingSpherex2PhysicsCollision(std::shared_ptr<Actor> _colliderA, std::shared_ptr<Actor> _colliderB, float _elasticityCoefficient)
{
    // Compute the normal vector of the collision
    glm::vec3 collisionNormal = glm::normalize(_colliderB->GetGlobalPosition() - _colliderA->GetGlobalPosition());

    // Compute the relative velocity in the direction of the collision
    float relativeVelocity = glm::dot(_colliderB->GetPhysicsComponent()->GetVelocity() - _colliderA->GetPhysicsComponent()->GetVelocity(), collisionNormal);

    // Calculate the impulse scalar
    float inverseMassA = 1 / _colliderA->GetPhysicsComponent()->GetMass();
    float inverseMassB = 1 / _colliderB->GetPhysicsComponent()->GetMass();
    float totalInverseMass = inverseMassA + inverseMassB;
    float restitutionTerm = -(1 + _elasticityCoefficient);
    float impulse = (restitutionTerm * relativeVelocity) / totalInverseMass;

    // Impulse vector applied in the direction of the collision normal
    glm::vec3 impulseVector = impulse * collisionNormal;

    _colliderA->GetPhysicsComponent()->AddVelocity(-(impulseVector / _colliderA->GetPhysicsComponent()->GetMass()));
    _colliderB->GetPhysicsComponent()->AddVelocity(impulseVector / _colliderB->GetPhysicsComponent()->GetMass());
}

void IBounded::BoundingSpherexBoundryPhysicsCollision(std::shared_ptr<Actor> _collider, std::shared_ptr<VisualActor> _conformCollider, glm::vec3 _boundaryNormal, float _elasticityCoefficient)
{
    // Normalize the boundary normal to ensure it's a unit vector
    glm::vec3 boundaryNormal = glm::normalize(_boundaryNormal);

    // Get the collider's velocity
    glm::vec3 colliderVelocity = _collider->GetPhysicsComponent()->GetVelocity();

    // Project the velocity onto the boundary normal to get the velocity component along the normal
    float velocityNormalComponent = glm::dot(colliderVelocity, boundaryNormal);

    // Compute the impulse scalar for the collision with a static boundary (boundary velocity = 0)
    float inverseMassCollider = 1 / _collider->GetPhysicsComponent()->GetMass();
    float restitutionTerm = -(1 + _elasticityCoefficient);
    float impulse = restitutionTerm * velocityNormalComponent / inverseMassCollider;

    // Impulse vector applied along the collision normal
    glm::vec3 impulseVector = impulse * boundaryNormal;

    // Apply the impulse to the collider (only the collider is affected since the boundary is static)
    _collider->GetPhysicsComponent()->AddVelocity(impulseVector / _collider->GetPhysicsComponent()->GetMass());
}
