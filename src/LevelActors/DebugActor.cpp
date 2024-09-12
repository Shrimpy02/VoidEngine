
// Includes
#include <LevelActors/DebugActor.h>
#include <RenderElements/Mesh.h>

#include "Core/Shader.h"

// Additional Includes


DebugActor::DebugActor(const std::string& _name, bool _enableLifetime, std::vector<glm::vec3> _points)
	: Actor(_name), mEnableLifetime(_enableLifetime)
{
	SetShaderObjectType(ShaderObjectType::Debug);
	if (!_points.empty())
		mVisualMesh = Mesh::CreateDebugLine(_points);

	if (mEnableLifetime)
		InitializeLifeTime();
}

DebugActor::DebugActor(const std::string& _name, glm::vec3 _color, bool _enableLifetime, std::vector<glm::vec3> _points)
	: Actor(_name), mEnableLifetime(_enableLifetime), mDebugColor(_color)
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
	_shader->setVec3("DebugColor", mDebugColor);
	mVisualMesh->DrawDebugLines(_shader);
}

void DebugActor::Update(float _dt)
{
}

void DebugActor::SetVisualMesh(std::vector<glm::vec3>& _newPoints)
{
	mVisualMesh = Mesh::CreateDebugLine(_newPoints);
}

void DebugActor::SetVisualMesh(std::shared_ptr<Mesh> _mesh)
{
	mVisualMesh = Mesh::CreateDebugLine(_mesh);
}

void DebugActor::SetVisualMesh(std::pair<glm::vec3, glm::vec3> _inExtent)
{
	mVisualMesh = Mesh::CreateDebugLine(_inExtent);
}
