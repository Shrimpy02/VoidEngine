
// Includes
#include <RenderElements/GraphPoint.h>
#include <RenderElements/Mesh.h>

// Additional Includes

GraphPoint::GraphPoint(const std::string& _name)
	: Actor(_name)
{
	mVisualPointMesh = Mesh::CreateGraphSphere();
	SetShaderObjectType(ShaderObjectType::Graph);
}

void GraphPoint::Draw(const std::shared_ptr<Shader> _shader) const
{
	if (!mVisualPointMesh) return;

	mVisualPointMesh->Draw(_shader);
}

void GraphPoint::Update(float _dt)
{
}