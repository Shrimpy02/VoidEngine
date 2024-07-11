#pragma once

// includes
// Inherit`ers
#include <Renderer.h>
#include <Actor.h>
#include <Collision/Collision.h>
// others
#include <Collision/AABB.h>
#include <Collision/BoundingSphere.h>

// Additional Includes
#include <string>
#include <memory>

// Forward Declarations
class Shader;

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

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // Constructor
    BaseActor(const std::string& _name, std::shared_ptr<Mesh> _visualMesh, std::shared_ptr<Mesh> _collisionMesh = nullptr);

    ~BaseActor();

    // Overriden from IRender passes draw call to mesh. 
    void Draw(const std::shared_ptr<Shader> _shader = nullptr) const override;

	// Overriden from Actor, handles local tick logic
    void Update(float _dt) override;

    // Overrides IBounded function, Returns an AABB object for collision processing.
    AABB GetAABB() const override;

    // Overrides IBounded function, Returns a BoundingSphere object for collision processing.
    BoundingSphere GetBoundingSphere() const override;

private:
    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Set if draw collision mesh is true or false
    void SetDrawDebugCollisionMesh(bool _state) { mShouldDrawCollisionMesh = _state; }

    // Getters

    // Gets this classes collision properties.
    //CollisionProperties& GetCollisionProperties() override;

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
    std::shared_ptr<Mesh> mMesh{ nullptr };

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

    // Returns the mesh for this actor as Mesh*
    std::shared_ptr<Mesh> GetActorMesh() { return mMesh; }

    // Adders

    // Setters


};

// ---------------------------------------------------------------
// --------------------- CollisionActor ------------------------------
// ---------------------------------------------------------------

/**
* @class CollisionActor
* @brief Represents a collision actor in a scene, it inherits from actor for world location and
* IBounded for collision processing. It also inherits from IRender for optional visualization of collision mesh.
*/
class CollisionActor : public Actor, public IBounded, public IRender
{
public:
    // ---------- Global Variables --------------

    // Collision properties inherited from IBounded
    //CollisionProperties mCollisionProperties;

private:
    // ---------- Local Variables --------------

    // Collision mesh
    std::shared_ptr<Mesh> mCollisionMesh = nullptr;

public:
    // ---------- Global functions --------------

    // Constructor
    CollisionActor(const std::string& name, std::shared_ptr<Mesh> _mesh, const CollisionProperties _inCollisionProps = {});
    
    // Draw override inherited from IRender, only for collision visualization
    void Draw(const std::shared_ptr<Shader> _shader) const override;

    // Overrides IBounded function, Returns an AABB object for collision processing.
    AABB GetAABB() const override;

    // Overrides IBounded function, Returns a BoundingSphere object for collision processing.
    BoundingSphere GetBoundingSphere() const override;

    // Overrides IBounded function, Returns the collision properties for this object
    //CollisionProperties& GetCollisionProperties() override;

private:
    // ---------- Local functions --------------

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters
    void SetDrawDebugCollisionMesh(bool _state) { mShouldDrawCollisionMesh = _state; }

    // Getters

};