#pragma once

// Includes
#include <Renderer.h>
#include <Actor.h>

// Additional Includes
#include <memory>

// Forward Declarations
class Shader;
class Mesh;
class Mesh;

/**
 * @class DebugActor
 * @brief Represents an object managing a series of points that makes a graph.
 */
class DebugActor : public Actor, public IRender
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

    std::shared_ptr<Mesh> mVisualMesh;

public:
    // ---------- Global functions --------------

    // Constructor
    DebugActor(const std::string& _name, std::vector<glm::vec3> _points);

    // Overriden from IRender passes draw call to mesh. 
    virtual void Draw(const std::shared_ptr<Shader> _shader = nullptr) const override;

    virtual void Update(float _dt) override;

    void UpdateVisualMesh(std::vector<glm::vec3>& _newPoints);

private:
    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

    // Getters

    // Adders

    // Setters

};
