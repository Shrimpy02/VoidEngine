#pragma once

// Includes
#include <Renderer.h>
#include <Actor.h>
#include <LifeTime.h>

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
class DebugActor : public Actor, public IRender, public ILifeTime
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

    std::shared_ptr<Mesh> mVisualMesh;
    bool mEnableLifetime = false;

public:
    // ---------- Global functions --------------

    // Constructor
    DebugActor(const std::string& _name, bool _enableLifetime = false, std::vector<glm::vec3> _points = std::vector<glm::vec3> {0});

    // Overriden from IRender passes draw call to mesh. 
    virtual void Draw(const std::shared_ptr<Shader> _shader = nullptr) const override;

    virtual void Update(float _dt) override;

    void UpdateVisualMesh(std::vector<glm::vec3>& _newPoints);

    void UpdateVisualMesh(std::shared_ptr<Mesh> _mesh);

private:
    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

    // Getters
    std::shared_ptr<Mesh> GetVisualMesh() { return mVisualMesh; }

    bool IsLifetimeEnabled() { return mEnableLifetime; }

    // Adders

};
