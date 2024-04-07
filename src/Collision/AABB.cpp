
// Includes
#include <Collision/AABB.h>
#include <Collision/BoundingSphere.h>

bool AABB::IsIntersecting(const AABB& _other, glm::vec3* _mtv) const
{
    // calculates the difference from one center to the other
       // and their extent sum
    glm::vec3 diff = _other.mCenter - this->mCenter;
    glm::vec3 sumExtent = this->mExtent + _other.mExtent;

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

bool AABB::IsIntersecting(const BoundingSphere& _other, glm::vec3* _mtv) const
{
    // Gets the point along the AABB box closest to the sphere 
    glm::vec3 clampedAABB = glm::clamp(_other.mCenter, mCenter - mExtent, mCenter + mExtent);

    // Gets the vector between the bounding sphere center and the aabb closest point
    glm::vec3 diff = clampedAABB - _other.mCenter;

    // gets the distance between these points
    float distanceSquared = glm::dot(diff, diff);

    // is true if the distance is more than the radius squared
    bool intersects = distanceSquared <= (_other.mRadius * _other.mRadius);

    if (!intersects) // if check so the rest of the code does not gray out and become a pain.
        return false; // should only proc return false so that no more computing is done for this scenario 

    // Calculate the minimum translation vector for the collision
    if (_mtv)
    {
        // Calculate the penetration depth
        float distance = glm::sqrt(distanceSquared);
        float penetration = _other.mRadius - distance;

        // Calculate the direction of the MTV
        glm::vec3 direction = glm::normalize(diff);

        // Multiply the direction by the penetration to get the MTV
        *_mtv = direction * penetration;
        // inverse mtv direction
        *_mtv *= -1;
    }

    return true; // intersection exists if function has come this far
}
