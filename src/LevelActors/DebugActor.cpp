
// Includes
#include <LevelActors/DebugActor.h>
#include <RenderElements/Mesh.h>

// Additional Includes


DebugActor::DebugActor(const std::string& _name, bool _enableLifetime, std::vector<glm::vec3> _points)
	: Actor(_name), mEnableLifetime(_enableLifetime)
{
	SetShaderObjectType(ShaderObjectType::Debug);
	if(!_points.empty())
		mVisualMesh = Mesh::CreateDebugLine(_points);

	if(mEnableLifetime)
		InitializeLifeTime();
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

void DebugActor::UpdateVisualMesh(std::shared_ptr<Mesh> _mesh)
{
	mVisualMesh = Mesh::CreateDebugLine(_mesh);
}
