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
 * @class VisualActor
 * @brief Represents a visual object in a scene, it inherits from actor for world location and
 * IRender for mesh rendering.
 */
class VisualActor : public Actor, public IRender
{
public:
    // ---------- Global Variables --------------

    // The visual mesh
    std::shared_ptr<Mesh> mVisualMesh{ nullptr };

    // Variables for terrain extent
    glm::vec3 mCenter{ 0.f,0.f,0.f };
    glm::vec3 mMinExtent{ 0.f,0.f,0.f };
    glm::vec3 mMaxExtent{ 0.f,0.f,0.f };
    glm::vec3 mExtent{ 0.f,0.f,0.f };

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // Constructor
    VisualActor(const std::string& _name, std::shared_ptr<Mesh> _mesh);

    // Overriden from IRender passes draw call to mesh. 
    virtual void Draw(const std::shared_ptr<Shader> _shader = nullptr) const override;

    virtual void Update(float _dt) override;

    void SetMinMaxExtent();

    void UpdateExtent();

private:
    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

    // Getters

    // Returns the mesh for this actor as Mesh
    std::shared_ptr<Mesh> GetActorVisualMesh() { return mVisualMesh; }

    // Adders

    // Setters


};
