
// Includes
#include <SceneActors.h>
#include <Utilities/Logger.h>

// ---------------------------------------------------------------
// --------------------- Visual Actor ------------------------------
// ---------------------------------------------------------------

VisualActor::VisualActor(const std::string& _name, Mesh* _mesh)
    : Actor(_name), mMesh(_mesh)
{
    LOG("VisualActor Created: %s", _name.c_str());
}

void VisualActor::Draw(const Shader* _shader) const
{
    if (!mMesh) return;

    mMesh->Draw(_shader);
}

// ---------------------------------------------------------------
// --------------------- BaseActor ------------------------------
// ---------------------------------------------------------------

BaseActor::BaseActor(const std::string& _name, Mesh* _mesh)
    :Actor(_name), mMesh(_mesh)
{
    // Creates and remembers both collision meshes for viualization and custom changing of collision base at run time. Is unoptomized.
    mCollisionCube = CreateCollisionCubeFromMesh(Material::Load("Debug"), mMesh->GetVertices());
    mCollisionSphere = CreateCollisionSphereFromMesh(Material::Load("Debug"), mMesh->GetVertices());
    LOG("BaseActor Created: %s", _name.c_str());
}

BaseActor::~BaseActor()
{
    
}

void BaseActor::Draw(const Shader* _shader) const
{
    // Draw visual mesh first
    if (!mMesh) return;
    mMesh->Draw(_shader);

    // Draw collision mesh after depending on collision base, draw as wire frame
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (mShouldDrawCollisionMesh) {

        if (mCollisionCube && mCollisionProperties.IsAABB())
            mCollisionCube->Draw(_shader);

    	else if(mCollisionSphere && mCollisionProperties.IsBoundingSphere())
            mCollisionSphere->Draw(_shader);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void BaseActor::Update(float _dt)
{
   
}

AABB BaseActor::GetAABB() const
{
    if (mCollisionCube)
    {
        // Calculate scaled min and max extents
        glm::vec3 scaledMinExtent = mMinExtent * GetScale(TransformSpace::Global);
        glm::vec3 scaledMaxExtent = mMaxExtent * GetScale(TransformSpace::Global);

        // Apply object's position
        glm::vec3 position = GetPosition(TransformSpace::Global);

        // Calculate local center of the AABB, there is a center variable but is calculated at setup and there fore is not dynamic
        glm::vec3 center = (scaledMinExtent + scaledMaxExtent) * 0.5f + position;

        // calculated total extent of the AABB from centre 
        glm::vec3 extent = (scaledMaxExtent - scaledMinExtent) * 0.5f;

        // Construct and return the AABB
        return AABB(center, extent);

    }
    else {
        // Return default AABB
        LOG_WARNING("Default AABB Returned");
        return AABB(GetPosition(TransformSpace::Global), GetScale(TransformSpace::Global) * 0.5f);
    }
}

BoundingSphere BaseActor::GetBoundingSphere() const
{
    if (mCollisionSphere)
    {
        // getes the average scale so that the sphere is kind of scaled with mesh scaleing. Temp fix
        float averageScale(0);
        for (int i = 0; i < 3; i++)
        {
            averageScale += GetScale(Actor::TransformSpace::Global)[i];
        }
        averageScale /= 3;

        // Calculate scaled min and max extents
        float scaledRadius = mRadius * averageScale;

        // Apply object's position
        glm::vec3 position = GetPosition(TransformSpace::Global);

        // Calculate local center of the AABB
        glm::vec3 center = glm::vec3(0.f) + position;

        // Calculate extent of the AABB
        float radius = glm::length(scaledRadius);

        // Construct and return the AABB
        return BoundingSphere(center, radius);
    }
    else {
        // Return default Bounding sphere
        LOG_WARNING("Default BoundignSphere Returned");
        return BoundingSphere(GetPosition(TransformSpace::Global), 0.5f);
    }
}

CollisionProperties* BaseActor::GetCollisionProperties()
{
    // Returns pointer to class object
    return &mCollisionProperties;
}

// ---------------------------------------------------------------
// --------------------- CollisionActor ------------------------------
// ---------------------------------------------------------------

CollisionActor::CollisionActor(const std::string& _name, Mesh* _mesh, CollisionProperties _inCollisionProps)
    : Actor(_name), mCollisionMesh(_mesh), mCollisionProperties(_inCollisionProps)
{
    // System to calculates extent and centre conforming to init collision mesh. 
    // TODO : kind of does`nt work with un-even scale or with object rotation. That needs to be figured out

	// Calculate the bounding box (min and max extents) of the existing mesh
    std::vector<Vertex> existingMesh = _mesh->GetVertices();

	glm::vec3 maxExtent = existingMesh[0].mPosition;
    glm::vec3 minExtent = existingMesh[0].mPosition;
    float largetsDiff(0.f);

    // Calc extent
    for (const auto& vertex : existingMesh)
    {
        minExtent = glm::min(minExtent, vertex.mPosition);
        maxExtent = glm::max(maxExtent, vertex.mPosition);
    }

    // calc sphere radius
    for (int i = 0; i < 3; i++)
    {
        // check all vertices looking for the longest vertex away.  
        for (int j = 0; j < existingMesh.size(); j++)
        {

             // Finds the vertex that is the furthest from the center
             if (largetsDiff < abs(existingMesh[j].mPosition[i]))
                 largetsDiff = abs(existingMesh[j].mPosition[i]);

        }
    }

    // set the class values to the calculated values
    mMaxExtent = maxExtent;
    mMinExtent = minExtent;
    mRadius = largetsDiff;

    // Gets the debug material from material cache
    std::string debugString = "Debug";
    mCollisionMesh->SetMaterial(Material::GetMaterialFromCache(debugString));

    // Crude log for assimp import test
    if(mCollisionProperties.IsAABB())
        LOG("CollisionActor(AABB) Created: %s", _name.c_str());
    else
        LOG("CollisionActor(BoundingSphere) Created: %s", _name.c_str());
}

void CollisionActor::Draw(const Shader* _shader) const
{
    if (!mShouldDrawCollisionMesh) return;

    if (mCollisionMesh)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mCollisionMesh->Draw(_shader);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

AABB CollisionActor::GetAABB() const
{
    if (mCollisionMesh)
    {
        // Calculate scaled min and max extents
        glm::vec3 scaledMinExtent = mMinExtent * GetScale(TransformSpace::Global);
        glm::vec3 scaledMaxExtent = mMaxExtent * GetScale(TransformSpace::Global);

        // Apply object's position
        glm::vec3 position = GetPosition(TransformSpace::Global);

        // Gets the center position after transform. Witch is good. 
        // Calculate local center of the AABB
        glm::vec3 center = (scaledMinExtent + scaledMaxExtent) * 0.5f + position;

        // Calculate extent of the AABB
        glm::vec3 extent = (scaledMaxExtent - scaledMinExtent) * 0.5f;

       // Dirty swap of z and y axis so importing from blender gets correct coordinates
       glm::vec3 newExtent(extent.x, extent.z, extent.y);


        // Construct and return the AABB
        return AABB(center, newExtent);
    }
    else {

        // Return default AABB
        LOG_WARNING("Default AABB Returned");
        return AABB(GetPosition(TransformSpace::Global), GetScale(TransformSpace::Global) * 0.5f);
    }
}

BoundingSphere CollisionActor::GetBoundingSphere() const
{
    if (mCollisionMesh)
    {
        // getes the average scale so that the sphere is kind of scaled with mesh scaleing. Temp fix
        float averageScale(0);
        for(int i = 0; i < 3; i++)
        {
            averageScale += GetScale(Actor::TransformSpace::Global)[i];
        }
        averageScale /= 3;

        // Calculate scaled min and max extents
        float scaledRadius = mRadius * averageScale;

        // Apply object's position
        glm::vec3 position = GetPosition(TransformSpace::Global);

        // Gets the center position after transform. Witch is good. 
        // Calculate local center of the AABB
        glm::vec3 center = glm::vec3(0.f) + position;

        // Calculate extent of the AABB
        float radius = glm::length(scaledRadius);

        // Construct and return the AABB
        return BoundingSphere(center, radius);
    }
    else {
        // Return default Bounding sphere
        LOG_WARNING("Default BoundignSphere Returned");
        return BoundingSphere(GetPosition(TransformSpace::Global), 0.5f);
    }

}

CollisionProperties* CollisionActor::GetCollisionProperties() 
{
    return &mCollisionProperties;
}
