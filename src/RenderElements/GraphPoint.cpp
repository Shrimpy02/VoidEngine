
// Includes
#include <RenderElements/GraphPoint.h>
#include <RenderElements/Mesh.h>
#include <Core/Shader.h>

// Additional Includes

GraphPoint::GraphPoint(const std::string& _name, PointType _inType)
	: Actor(_name), mPointType(_inType)
{
	mVisualPointMesh = Mesh::CreateGraphSphere();
	SetShaderObjectType(ShaderObjectType::Graph);

	if (mPointType == PointType::ControlPoint)
		mColor = glm::vec3(1, 0, 0);
}

void GraphPoint::Draw(const std::shared_ptr<Shader> _shader) const
{
	if (!mVisualPointMesh) return;

	_shader->setVec3("GraphColor", mColor);
	mVisualPointMesh->Draw(_shader);
}

void GraphPoint::Update(float _dt)
{

}