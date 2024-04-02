#include "AABB.h"

void AABBActor::Draw(const Shader* _shader) const
{
   if (mDrawCollisionMesh)
   {
       if (mCollisionMesh)
       {
           glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
           mCollisionMesh->Draw(_shader);
           glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
       }
   }
}

AABB AABBActor::GetAABB() const
{
    //auto aabb = AABB(mAABB.center, mAABB.extent);
    //aabb.center += GetPosition(Actor::TransformSpace::Global);
    //aabb.extent *= GetScale(Actor::TransformSpace::Global);
    //// AABBs should not really be able to rotate, but this is okay as long as they rotate to a new aligned axis
    //aabb.extent = abs(GetRotation(Actor::TransformSpace::Global) * aabb.extent);
    //return aabb;

  // Calculate scaled min and max extents
  glm::vec3 scaledMinExtent = mMinExtent * GetScale(TransformSpace::Global);
  glm::vec3 scaledMaxExtent = mMaxExtent * GetScale(TransformSpace::Global);

  // Calculate extent of the AABB
  glm::vec3 extent = (scaledMaxExtent - scaledMinExtent) * 0.5f;

  glm::vec3 rotatedExtent = glm::vec3(extent.x, extent.z, extent.y);

	// get center based on its position
  glm::vec3 center = mCenter + GetPosition(TransformSpace::Global);

  // Construct and return the AABB
  return AABB(GetPosition(TransformSpace::Global), extent);
}

CollisionProperties AABBActor::GetCollisionProperties() const
{
    return mCollisionProperties;
}
