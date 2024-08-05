#pragma once

// Includes
#include <Renderer.h>
#include <Actor.h>

// Additional Includes
#include <memory>

// Forward Declarations
class Shader;
class Mesh;

/**
 * @class GraphActor
 * @brief Represents an object managing a series of points that makes a graph.
 */
class GraphActor : public Actor, public IRender
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // Constructor
    GraphActor(const std::string& _name);

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
    void SetPoints(std::vector<glm::vec3> _pointPositions);

};
