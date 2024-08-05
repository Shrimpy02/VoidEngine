
// Includes
#include <LevelActors/GraphActor.h>
#include <RenderElements/GraphPoint.h>

// Additional Includes

GraphActor::GraphActor(const std::string& _name)
	: Actor(_name)
{

}

void GraphActor::Draw(const std::shared_ptr<Shader> _shader) const
{
}

void GraphActor::Update(float _dt)
{
}

void GraphActor::SetPoints(std::vector<glm::vec3> _pointPositions)
{
	int nameIterator = 0;
	for(glm::vec3 pointPosition : _pointPositions)
	{
		nameIterator++;
		std::string pointName = GetTag() + "_" + std::to_string(nameIterator);

		std::shared_ptr<GraphPoint> point = std::make_shared<GraphPoint>(pointName);
		point->SetGlobalPosition(pointPosition);
		point->SetGlobalScale(glm::vec3(0.5f));
		AddChild(point);
	}
}
