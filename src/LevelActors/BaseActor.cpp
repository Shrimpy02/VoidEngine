
// Includes
#include <LevelActors/BaseActor.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/Texture.h>
#include <RenderElements/Material.h>
#include <Utilities/Logger.h>

// Additional Includes

BaseActor::BaseActor(const std::string& _name, std::shared_ptr<Mesh> _visualMesh, std::shared_ptr<Mesh> _collisionMesh)
    :Actor(_name)
{

    if (_visualMesh) {

        mVisualMesh = _visualMesh;
        if (_collisionMesh)
        {
            mCollisionMesh = _collisionMesh;
            mCustomCollisionMesh = true;
        }
        else {

        }

    }
    else {
        LOG_WARNING("BaseActor `%s` has no visible mesh", _name.c_str());
    }

    //mCollisionMesh->SetIsVisible(false);

    LOG("BaseActor Created: %s", _name.c_str());
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

void BaseActor::SetMinMaxExtent()
{
    std::pair<glm::vec3, glm::vec3> extents = Mesh::GetMeshMinMaxExtent(mVisualMesh);

    mMinExtent = extents.first;
    mMaxExtent = extents.second;
}

void BaseActor::UpdateExtent()
{
    if (mCollisionProperties.IsAABB())
    {

        glm::vec3 extent = (mMaxExtent - mMinExtent) * glm::vec3(0.5);

        extent *= GetGlobalScale();

        glm::quat actorRotation = GetGlobalRotation();

        // Rotate the total extent by the actor's rotation for each axis
        glm::vec3 xAxis = actorRotation * glm::vec3(extent.x, 0.0f, 0.0f);
        glm::vec3 yAxis = actorRotation * glm::vec3(0.0f, extent.y, 0.0f);
        glm::vec3 zAxis = actorRotation * glm::vec3(0.0f, 0.0f, extent.z);

        // Calculate new rotated extent
        glm::vec3 rotatedExtents(
            glm::abs(xAxis.x) + glm::abs(yAxis.x) + glm::abs(zAxis.x),
            glm::abs(xAxis.y) + glm::abs(yAxis.y) + glm::abs(zAxis.y),
            glm::abs(xAxis.z) + glm::abs(yAxis.z) + glm::abs(zAxis.z));

        mExtent = rotatedExtents;
        mCenter = ((mMinExtent + mMaxExtent) * 0.5f) + GetGlobalPosition();

    }
    else if (mCollisionProperties.IsBoundingSphere()) {

        mRadius = glm::length(((mMaxExtent - mMinExtent) * glm::vec3(0.5)) * GetGlobalScale());
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
        	SetMinMaxExtent();
        }
        else if (mCollisionProperties.IsBoundingSphere()) {

            collisionName.append("-CollisionSphere");
            mCollisionMesh = Mesh::CreateSphereByExtent(mVisualMesh, debugMat, collisionName);
            SetMinMaxExtent();
        }
        mCollisionProperties.mBaseCompare = mCollisionProperties.mBase;
    }
    else if (mCustomCollisionMesh && doOnce) {
        SetMinMaxExtent();
        doOnce = false;
    }
}