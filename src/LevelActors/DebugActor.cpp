
// Includes
#include <LevelActors/DebugActor.h>
#include <RenderElements/Mesh.h>

// Additional Includes


DebugActor::DebugActor(const std::string& _name, std::vector<glm::vec3> _points)
	: Actor(_name)
{
	SetShaderObjectType(ShaderObjectType::Debug);
	mVisualMesh = Mesh::CreateDebugLine(_points);
}
void DebugActor::Draw(const std::shared_ptr<Shader> _shader) const
{
	if (!mVisualMesh) return;

	mVisualMesh->DrawDebugLines(_shader);
}

void DebugActor::Update(float _dt)
{
}

void DebugActor::UpdateVisualMesh(std::vector<glm::vec3>& _newPoints)
{
	mVisualMesh = Mesh::CreateDebugLine(_newPoints);
}
