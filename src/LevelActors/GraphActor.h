#pragma once

// Includes
#include <Renderer.h>
#include <Actor.h>

// Additional Includes
#include <memory>

// Forward Declarations
class Shader;
class Mesh;
class GraphPoint;

enum GraphMethod
{
    DeCasteljau,
    Neville,
    DeBoor,
    BSpline
};

enum GraphType
{
    Interpolated,
    Approximated,
    BSpline_Clamped
};

/**
 * @class GraphActor
 * @brief Represents an object managing a series of points that makes a graph.
 */
class GraphActor : public Actor, public IRender
{
public:
    // ---------- Global Variables --------------

    std::vector<std::shared_ptr<GraphPoint>> mControlPoints;

private:
    // ---------- Local Variables --------------

    
    std::vector<std::shared_ptr<GraphPoint>> mGraphPoints;
    std::vector<glm::vec3> mOldPositions;
    GraphMethod mGraphMethod = DeCasteljau;
    GraphType mGraphType = Interpolated;
    float mStep = 10.f;

public:
    // ---------- Global functions --------------

    // Constructor
    GraphActor(const std::string& _name, glm::vec3 _pos = glm::vec3(0), glm::vec3 _scale = glm::vec3(1), glm::quat _rotation = glm::quat());

    // Overriden from IRender passes draw call to mesh. 
    virtual void Draw(const std::shared_ptr<Shader> _shader = nullptr) const override;

    virtual void Update(float _dt) override;

    void CreateGraph(GraphMethod _inMethod, GraphType _inType, const float _step = 0.10f, int _dimension = 2);

    // Helper Graph creating functions
    void SetControlPoints(const std::vector<glm::vec3>& _controlPoints);

private:
    // ---------- Local functions --------------

    void ClearGraph();

    void ClearControlPoints();

    void CreateControlPoints(const std::vector<glm::vec3>& _controlPoints);


    std::vector<glm::vec3> GetPointsFromMethod(GraphMethod _inMethod, GraphType _inType, const float _step, int _dimension);

public:
    // ---------- Getters / setters / Adders --------------

    // Getters

    // Adders

    // Setters
    void SetPoints(std::vector<glm::vec3> _pointPositions);

};
