
// Includes
#include <Collision/Collision.h>
#include <Utilities/Logger.h>

#include "glm/ext/quaternion_geometric.hpp"

// Additional Includes


bool IBounded::isIntersecting(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv)
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
    glm::vec3 sumExtent = this->mExtent + glm::vec3(0);

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


	//return (_pointPos.x >= -this->mExtent.x && _pointPos.x <= this->mExtent.x &&
    //    _pointPos.y >= -this->mExtent.y && _pointPos.y <= this->mExtent.y &&
    //    _pointPos.z >= -this->mExtent.z && _pointPos.z <= this->mExtent.z);
}
