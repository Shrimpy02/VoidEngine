#pragma once

// Includes
#include <Renderer.h>
#include <Actor.h>
#include <RenderElements/GraphPoint.h>

// Additional Includes
#include <memory>

// Forward Declarations
class Mesh;
class Shader;

/**
 * @class GraphActor
 * @brief Represents an actor in the scene that keeps track of a path, curve,
 * graph or other structures containing multiple points. 
 */
class GraphActor : public Actor, IRender
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

    std::vector<std::shared_ptr<GraphPoint>> mGraphPoints;

public:
    // ---------- Global functions --------------

    // Constructor
    GraphActor(const std::string& _name, std::vector<glm::vec3> _points);

    ~GraphActor() = default;

    // Overriden from IRender passes draw call to mesh. 
    void Draw(const std::shared_ptr<Shader> _shader = nullptr) const override;

    // Overriden from Actor, handles local tick logic
    void Update(float _dt) override;


private:


    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters

};