
// Includes
#include <LevelActors/GraphActor.h>
#include <RenderElements/GraphPoint.h>
#include <Utilities/Logger.h>
// Additional Includes

GraphActor::GraphActor(const std::string& _name, glm::vec3 _pos, glm::vec3 _scale, glm::quat _rotation)
	: Actor(_name)
{
	mShaderObjectType = ShaderObjectType::Graph;
	SetGlobalPosition(_pos);
	SetGlobalScale(_scale);
	SetGlobalRotation(_rotation);
}

void GraphActor::Draw(const std::shared_ptr<Shader> _shader) const
{
}

void GraphActor::Update(float _dt)
{
	for(int i = 0; i < mControlPoints.size(); i++)
	{
		if(mControlPoints[i]->GetLocalPosition() != mOldPositions[i])
		{

			ClearGraph();
			CreateGraph(mGraphMethod,mGraphType, mStep);

		}
		mOldPositions[i] = mControlPoints[i]->GetLocalPosition();
	}
}


void GraphActor::CreateGraph(GraphMethod _inMethod, GraphType _inType, const float _step)
{

	mGraphMethod = _inMethod;
	mGraphType = _inType;
	mStep = _step;
	
	std::vector<glm::vec3> graphNodes = GetPointsFromMethod(_inMethod, _inType, _step);

	for (int i = 0; i < graphNodes.size(); i++)
	{
		std::string pointName = GetTag() + "_GraphPoint_" + std::to_string(i);
	
		std::shared_ptr<GraphPoint> point = std::make_shared<GraphPoint>(pointName, PointType::RegularPoint);
		point->SetGlobalPosition(graphNodes[i]);
		point->SetGlobalScale(GetGlobalScale() / glm::vec3(2.f));
		mGraphPoints.push_back(point);
		AddChild(point);
	}
}

void GraphActor::SetControlPoints(const std::vector<glm::vec3>& _controlPoints)
{
	if(!mControlPoints.empty())
		ClearControlPoints();

	CreateControlPoints(_controlPoints);
}

void GraphActor::CreateControlPoints(const std::vector<glm::vec3>& _controlPoints)
{
	
	for(int i = 0; i < _controlPoints.size(); i++)
	{
		std::string name = GetTag() + "_ControlPoint_" + std::to_string(i);
		std::shared_ptr<GraphPoint> pointPtr = std::make_shared<GraphPoint>(name, PointType::ControlPoint);
		pointPtr->SetLocalPosition(_controlPoints[i]);
		mOldPositions.push_back(_controlPoints[i]);
		mControlPoints.push_back(pointPtr);
		AddChild(pointPtr);
	}
}

void GraphActor::ClearGraph()
{
	for (int i = 0; i < mGraphPoints.size(); i++)
	{
		RemoveChild(mGraphPoints[i]);
	}
	mGraphPoints.clear();
}

void GraphActor::ClearControlPoints()
{
	for (int i = 0; i < mControlPoints.size(); i++)
	{
		RemoveChild(mControlPoints[i]);
	}
	mControlPoints.clear();
}

std::vector<glm::vec3> GraphActor::GetPointsFromMethod(GraphMethod _inMethod, GraphType _inType, const float _step)
{
	if (_inMethod == DeCasteljau && _inType == Approximated)
		return SMath::DeCastApproximationPoints(mControlPoints, _step);

	else if (_inMethod == DeCasteljau && _inType == Interpolated)
		LOG_WARNING("No DeCast interpolation functions");

	else if (_inMethod == Neville && _inType == Approximated)
		LOG_WARNING("No Neville approximated functions");

	else if (_inMethod == Neville && _inType == Interpolated)
		return SMath::NevillInterpolatedPoints(mControlPoints, _step);

	else if (_inMethod == DeBoor && _inType == Approximated)
		LOG_WARNING("No DeBoor approximated functions");

	else if (_inMethod == DeBoor && _inType == Interpolated)
		LOG_WARNING("No DeBoor interpolation functions");

	return std::vector<glm::vec3>();
}


