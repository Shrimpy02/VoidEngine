
// Includes
#include <LevelActors/GraphActor.h>

GraphActor::GraphActor(const std::string& _name, std::vector<glm::vec3> _points) : Actor(_name)
{
	for(glm::vec3 point : _points)
	{
		mGraphPoints.push_back(std::make_shared<GraphPoint>(point));
	}
}

void GraphActor::Draw(const std::shared_ptr<Shader> _shader) const
{
	if(_shader)
	{
		
	}
}

void GraphActor::Update(float _dt)
{
}
