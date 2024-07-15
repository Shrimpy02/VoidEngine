
// Includes
#include <SceneActors.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/Vertex.h>
#include <RenderElements/Texture.h>
#include <RenderElements/Material.h>
#include <Utilities/Logger.h>

// Additional Includes
#include <glad/glad.h>



// ---------------------------------------------------------------
// --------------------- BaseActor ------------------------------
// ---------------------------------------------------------------

BaseActor::BaseActor(const std::string& _name, std::shared_ptr<Mesh> _visualMesh, std::shared_ptr<Mesh> _collisionMesh)
    :Actor(_name)
{

    if (_visualMesh) {

        mVisualMesh = _visualMesh;
        if(_collisionMesh)
        {
            mCollisionMesh = _collisionMesh;
            mCustomCollisionMesh = true;
        } else {
	        
        }

    } else {
        LOG_WARNING("BaseActor `%s` has no visible mesh", _name.c_str());
    }

    //mCollisionMesh->SetIsVisible(false);

    LOG("BaseActor Created: %s", _name.c_str());
}

BaseActor::~BaseActor()
{
    
}

void BaseActor::Draw(const std::shared_ptr<Shader> _shader) const
{
    // Draw visual mesh first
    if (!mVisualMesh) return;
    mVisualMesh->Draw(_shader);

    // Then collision mesh
    if (!mCollisionMesh) return;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    mCollisionMesh->Draw(_shader);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void BaseActor::Update(float _dt)
{
    UpdateCollisionMeshBasedOnCollisionBase();

	UpdateExtent();
}


void BaseActor::SetExtent()
{
    // Calculate the bounding box (min and max extents) of the existing mesh
    std::vector<Vertex>& collisionMeshVertices = mCollisionMesh->GetVertices();
    glm::vec3 maxExtent = collisionMeshVertices[0].mPosition;
    glm::vec3 minExtent = collisionMeshVertices[0].mPosition;

    for (Vertex& vertex : collisionMeshVertices)
    {
        minExtent = glm::min(minExtent, vertex.mPosition);
        maxExtent = glm::max(maxExtent, vertex.mPosition);
    }

    // Slight offsett
    maxExtent += 0.001f;
    minExtent -= 0.001f;

    // Updates extents
    mMinExtent = minExtent;
    mMaxExtent = maxExtent;
}

void BaseActor::UpdateExtent()
{
    if (mCollisionProperties.IsAABB())
    {
        glm::quat actorRotation = GetGlobalRotation();

        // Get the total extent
        glm::vec3 extent = (mMaxExtent - mMinExtent) * 0.5f * GetGlobalScale();

        // Rotate the total extent by the actor's rotation for each axis
        glm::vec3 xAxis = actorRotation * glm::vec3(extent.x, 0.0f, 0.0f);
        glm::vec3 yAxis = actorRotation * glm::vec3(0.0f, extent.y, 0.0f);
        glm::vec3 zAxis = actorRotation * glm::vec3(0.0f, 0.0f, extent.z);

        // Calculate new rotated extent
        glm::vec3 rotatedExtents(
            glm::abs(xAxis.x) + glm::abs(yAxis.x) + glm::abs(zAxis.x),
            glm::abs(xAxis.y) + glm::abs(yAxis.y) + glm::abs(zAxis.y),
            glm::abs(xAxis.z) + glm::abs(yAxis.z) + glm::abs(zAxis.z)
        );

        mExtent = rotatedExtents;
        mCenter = ((mMinExtent + mMaxExtent) * 0.5f) + GetGlobalPosition();

    }
    else if (mCollisionProperties.IsBoundingSphere()) {

        glm::vec3 extent = Mesh::GetExtentByMesh(mVisualMesh);

        mRadius = glm::length(extent);
        mCenter = ((mMinExtent + mMaxExtent) * 0.5f) + GetGlobalPosition();
    }

}

void BaseActor::UpdateCollisionMeshBasedOnCollisionBase()
{
    if (mCollisionProperties.mBase != mCollisionProperties.mBaseCompare && !mCustomCollisionMesh)
    {
        std::shared_ptr<Texture> debugTex = Texture::LoadWhiteTexture();
        std::shared_ptr<Material> debugMat = Material::Load("Debug", { debugTex }, { {glm::vec3(1.0f,0.0f,0.0f)}, {64} });
        std::string collisionName = GetTag();

        if (mCollisionProperties.IsAABB()) {

            collisionName.append("-CollisionCubeAABB");
            mCollisionMesh = Mesh::CreateCubeByExtent(mVisualMesh, debugMat, collisionName);
            SetExtent();
        }
        else if (mCollisionProperties.IsBoundingSphere()) {

            collisionName.append("-CollisionSphere");
            mCollisionMesh = Mesh::CreateSphereByExtent(mVisualMesh, debugMat, collisionName);
            SetExtent();
        }
        mCollisionProperties.mBaseCompare = mCollisionProperties.mBase;

    }
    else if (mCustomCollisionMesh && doOnce) {
        SetExtent();
        doOnce = false;
    }
}


// ---------------------------------------------------------------
// --------------------- Visual Actor ------------------------------
// ---------------------------------------------------------------

VisualActor::VisualActor(const std::string& _name, std::shared_ptr<Mesh> _mesh)
    : Actor(_name), mVisualMesh(_mesh)
{
    LOG("VisualActor Created: %s", _name.c_str());
}

void VisualActor::Draw(const std::shared_ptr<Shader> _shader) const
{
    if (!mVisualMesh) return;

    mVisualMesh->Draw(_shader);
}
