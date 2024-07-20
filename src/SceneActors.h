#pragma once

// Includes
// Inherit`ers
#include <Renderer.h>
#include <Actor.h>
#include <Collision/Collision.h>

// Additional Includes
#include <string>
#include <memory>

// Forward Declarations
class Shader;
class Mesh;

// ---------------------------------------------------------------
// --------------------- BaseActor ------------------------------
// ---------------------------------------------------------------

/**
 * @class BaseActor
 * @brief Represents a complete object in a scene, it inherits from actor for world location,
 * IRender for mesh rendering and IBounded for collision detection and processing.
 */
class BaseActor : public Actor, public IRender, public IBounded
{
public:
    // ---------- Global Variables --------------

    // The visual mesh
    std::shared_ptr<Mesh> mVisualMesh{ nullptr };
    std::shared_ptr<Mesh> mCollisionMesh{ nullptr };

    bool mCustomCollisionMesh = false;
    bool doOnce = true; // used

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // Constructor
    BaseActor(const std::string& _name, std::shared_ptr<Mesh> _visualMesh, std::shared_ptr<Mesh> _collisionMesh = nullptr);

    ~BaseActor() = default;

    // Overriden from IRender passes draw call to mesh. 
    void Draw(const std::shared_ptr<Shader> _shader = nullptr) const override;

	// Overriden from Actor, handles local tick logic
    void Update(float _dt) override;

    void SetMinMaxExtent();

    void UpdateExtent();

    void UpdateCollisionMeshBasedOnCollisionBase();

private:


	// ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters
    glm::vec3 GetExtent() { return mExtent; }

    // Getters

	// Returns the mesh for this actor as Mesh
    std::shared_ptr<Mesh> GetActorVisualMesh(){ return mVisualMesh; }

    // Gets this classes collision properties.
};

// ---------------------------------------------------------------
// --------------------- Visual Actor ------------------------------
// ---------------------------------------------------------------

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

    glm::vec3 mExtent = glm::vec3(0);

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // Constructor
    VisualActor(const std::string& _name, std::shared_ptr<Mesh>);

    // Overriden from IRender passes draw call to mesh. 
    virtual void Draw(const std::shared_ptr<Shader> _shader = nullptr) const override;


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
