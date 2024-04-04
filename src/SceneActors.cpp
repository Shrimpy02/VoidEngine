
// Includes
#include <SceneActors.h>
#include <Defines.h>

// ---------------------------------------------------------------
// --------------------- Mesh Actor ------------------------------
// ---------------------------------------------------------------

BaseActor::BaseActor(const std::string& _name, Mesh* _mesh)
    :Actor(_name), mMesh(_mesh)
{
    // Add debug mesh to show collision
    auto Tex = Texture::Load(SOURCE_DIRECTORY("assets/Textures/white.jpg"));
    auto defaultMat = Material::Load("Collision-Debug", { Tex }, {});
    mCollisionMesh = CreateCollisionCube(defaultMat, mMesh->GetVetices());
}

void BaseActor::Draw(const Shader* _shader) const
{
    if (!mMesh) return;

    mMesh->Draw(_shader);

    if (mCollisionMesh && mShouldDrawCollisionMesh) {
        // draws debug collision mesh if exists and has permission
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mCollisionMesh->Draw(_shader);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

AABB BaseActor::GetAABB() const
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

        // Construct and return the AABB
        return AABB(center, extent);
    }
    else {
        return AABB(GetPosition(TransformSpace::Global), GetScale(TransformSpace::Global) * 0.5f);
    }
}

CollisionProperties BaseActor::GetCollisionProperties() const
{
    return mCollisionProperties;
}

// ---------------------------------------------------------------
// --------------------- Visual Actor ------------------------------
// ---------------------------------------------------------------

VisualActor::VisualActor(const std::string& _name, Mesh* _mesh)
    : Actor(_name), mMesh(_mesh)
{


}

void VisualActor::Draw(const Shader* _shader) const
{
    if (!mMesh) return;

    mMesh->Draw(_shader);
}

// ---------------------------------------------------------------
// --------------------- AABBActor ------------------------------
// ---------------------------------------------------------------

AABBActor::AABBActor(const std::string& name, Mesh* _mesh)
    : Actor(name), mCollisionMesh(_mesh)
{
    // default extent init
    glm::vec3 maxExtent(0);
    glm::vec3 minExtent(0);
    glm::vec3 center(0);

    // For each axis
    for (int i = 0; i < 3; i++)
    {
        glm::vec3 pos(0);

        // check all vertices looking for the longest vertex away.  
        for (int j = 0; j < _mesh->GetVetices().size(); j++)
        {
            // Gets max extent for axis
            if (_mesh->GetVetices()[j].mPosition[i] > maxExtent[i])
                maxExtent[i] = _mesh->GetVetices()[j].mPosition[i];

            // Gets min extent for axis
            else if (_mesh->GetVetices()[j].mPosition[i] < minExtent[i])
                minExtent[i] = _mesh->GetVetices()[j].mPosition[i];

            // Gets center location for axis 
            pos[i] = _mesh->GetVetices()[j].mPosition[i];
        }
        center += pos;
    }

    // Divide by num vertices for average location of center
    center /= static_cast<float>(_mesh->GetVetices().size());

    // set the class values to the calculated values
    mMaxExtent = maxExtent;
    mMinExtent = minExtent;
    mCenter = center;

    // Add debug mesh to show collision
    _mesh->SetMaterial(_mesh->GetMaterial()->Load("Collision-Debug"));
}

void AABBActor::Draw(const Shader* _shader) const
{
    if (!mShouldDrawCollisionMesh) return;

    if (mCollisionMesh)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mCollisionMesh->Draw(_shader);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

AABB AABBActor::GetAABB() const
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

        // Construct and return the AABB
        return AABB(center, extent);
    }
    else {
        return AABB(GetPosition(TransformSpace::Global), GetScale(TransformSpace::Global) * 0.5f);
    }
}

CollisionProperties AABBActor::GetCollisionProperties() const
{
    return mCollisionProperties;
}
