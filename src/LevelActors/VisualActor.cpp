
// Includes
#include <LevelActors/VisualActor.h>
#include <RenderElements/Mesh.h>
#include <Utilities/Logger.h>

// Additional Includes

VisualActor::VisualActor(const std::string& _name, std::shared_ptr<Mesh> _mesh)
    : Actor(_name), mVisualMesh(_mesh)
{
    SetMinMaxExtent();
    LOG("VisualActor Created: %s", _name.c_str());
}

void VisualActor::Draw(const std::shared_ptr<Shader> _shader) const
{
    if (!mVisualMesh) return;

    mVisualMesh->Draw(_shader);
}

void VisualActor::Update(float _dt)
{
    UpdateExtent();
}

void VisualActor::SetMinMaxExtent()
{



    if (!mVisualMesh) return;

    std::pair<glm::vec3, glm::vec3> extents = Mesh::GetMeshMinMaxExtent(mVisualMesh);

    mMinExtent = extents.first;
    mMaxExtent = extents.second;
}

void VisualActor::UpdateExtent()
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
