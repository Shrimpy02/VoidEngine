#pragma once

// Includes
#include <Renderer.h>
#include <Actor.h>

// Additional Includes
#include <memory>

// Forward Declarations
class Shader;
class Mesh;

enum PointType
{
    ControlPoint,
    RegularPoint
};

/**
 * @class GraphPoint
 * @brief Represents each point on a graph.
 */
class GraphPoint : public Actor, public IRender
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

    std::shared_ptr<Mesh> mVisualPointMesh{ nullptr };

    PointType mPointType;

    glm::vec3 mColor = glm::vec3(0, 0, 1);

public:
    // ---------- Global functions --------------

    // Constructor
    GraphPoint(const std::string& _name, PointType _inType);

    // Overriden from IRender passes draw call to mesh. 
    virtual void Draw(const std::shared_ptr<Shader> _shader = nullptr) const override;

    virtual void Update(float _dt) override;

private:
    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

    // Getters

    // Adders

    // Setters


};
