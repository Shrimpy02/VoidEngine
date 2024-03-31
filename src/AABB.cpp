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
  // Calculate scaled min and max extents
  glm::vec3 scaledMinExtent = mMinExtent * GetScale(TransformSpace::Global);
  glm::vec3 scaledMaxExtent = mMaxExtent * GetScale(TransformSpace::Global);

  // Apply object's position
  glm::vec3 position = GetPosition(TransformSpace::Global);

  // Calculate center of the AABB
  glm::vec3 center = (scaledMinExtent + scaledMaxExtent) * 0.5f + position;

  // Calculate extent of the AABB
  glm::vec3 extent = (scaledMaxExtent - scaledMinExtent) * 0.5f;

    // done when importing from blender
	glm::vec3 rotatedExtent(extent.y, extent.z, extent.x); // Swapping extents

  // Construct and return the AABB
  return AABB(center, rotatedExtent);
}

CollisionProperties AABBActor::GetCollisionProperties() const
{
    return mCollisionProperties;
}
