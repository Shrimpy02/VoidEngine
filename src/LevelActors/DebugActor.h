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
    glm::vec3 mDebugColor{1.f,0.f,0.f};

public:
    // ---------- Global functions --------------

    // Constructor
    DebugActor(const std::string& _name, bool _enableLifetime = false, std::vector<glm::vec3> _points = std::vector<glm::vec3>{0});
    DebugActor(const std::string& _name, glm::vec3 _color, bool _enableLifetime = false, std::vector<glm::vec3> _points = std::vector<glm::vec3>{ 0 });

    // Overriden from IRender passes draw call to mesh. 
    virtual void Draw(const std::shared_ptr<Shader> _shader = nullptr) const override;

    virtual void Update(float _dt) override;

    void SetVisualMesh(std::vector<glm::vec3>& _newPoints);

    void SetVisualMesh(std::shared_ptr<Mesh> _mesh);

    void SetVisualMesh(std::pair<glm::vec3, glm::vec3> _inExtent);

private:
    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

    void SetColor(glm::vec3 _inColor) { mDebugColor = _inColor; }

    // Getters
    std::shared_ptr<Mesh> GetVisualMesh() { return mVisualMesh; }

    bool IsLifetimeEnabled() { return mEnableLifetime; }

    // Adders

};
