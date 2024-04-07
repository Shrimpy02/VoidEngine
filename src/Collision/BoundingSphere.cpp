
// Includes
#include <Collision/BoundingSphere.h>
#include <Collision/AABB.h>

bool BoundingSphere::IsIntersecting(const AABB& _other, glm::vec3* _mtv) const
{
    // Gets the point along the AABB box closest to the sphere 
    glm::vec3 clampedAABB = glm::clamp(mCenter, _other.mCenter - _other.mExtent, _other.mCenter + _other.mExtent);

    // Gets the vector between the bounding sphere center and the AABB closest point
    glm::vec3 diff = clampedAABB - mCenter;

    // gets the distance between these points
    float distanceSquared = glm::dot(diff, diff);

    // is true if the distance is more than the radius squared
    bool intersects = distanceSquared <= (mRadius * mRadius);

    if (!intersects) // if check so the rest of the code does not gray out and become a pain.
        return false; // should only proc return false so that no more computing is done for this scenario 

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

    return true; // intersection exists if function has come this far
}

bool BoundingSphere::IsIntersecting(const BoundingSphere& _other, glm::vec3* _mtv) const
{
    // Calculate the vector between the centers of the two spheres
    glm::vec3 diff = _other.mCenter - mCenter;

    // Calculate the squared distance between the centers
    float distanceSquared = glm::dot(diff, diff);

    // Calculate the sum of the radii
    float sumRadii = mRadius + _other.mRadius;

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
