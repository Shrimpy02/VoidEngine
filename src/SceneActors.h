#pragma once

// includes
// Inherit`ers
#include <Renderer.h>
#include <Actor.h>
#include <Collision/Collision.h>
// others
#include <Collision/AABB.h>
#include <Collision/BoundingSphere.h>

// CODE DECLARATION --------------
// Get AABB and BoundingSphere are basically the same function and could be converted to IBounded class for simplicity
// its collision base is what decides what one to use at collision time. Unoptomised and should be fixed. 

// ---------------------------------------------------------------
// --------------------- SceneGraph ------------------------------
// ---------------------------------------------------------------

/**
 * @class SceneGraph
 * @brief Represents a minimal actor in world context, is the top object in scene for render and update reference.
 * Has no functionality other han containing all scene objects as children.
 */
class SceneGraph : public Actor
{

public:
    // basic constructor
    SceneGraph(const std::string& _name) : Actor(_name) { };
};

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
    Mesh* mMesh{ nullptr };

    // The collision mesh if vis is enabled
    Mesh* mCollisionCube{ nullptr };
	Mesh* mCollisionSphere{ nullptr };

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // Constructor
    BaseActor(const std::string& _name, Mesh* _mesh);

    // Overriden from IRender passes draw call to mesh. 
    void Draw(const Shader* _shader = nullptr) const override;

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
    CollisionProperties* GetCollisionProperties() override;

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
    Mesh* mMesh{ nullptr };

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // Constructor
    VisualActor(const std::string& _name, Mesh* _mesh);

    // Overriden from IRender passes draw call to mesh. 
    virtual void Draw(const class Shader* _shader = nullptr) const override;


private:
    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

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
    CollisionProperties mCollisionProperties;

private:
    // ---------- Local Variables --------------

    // Collision mesh
    class Mesh* mCollisionMesh = nullptr;

public:
    // ---------- Global functions --------------

    // Constructor
    CollisionActor(const std::string& name, class Mesh* _mesh, const CollisionProperties _inCollisionProps = {});
    
    // Draw override inherited from IRender, only for collision visualization
    void Draw(const Shader* _shader) const override;

    // Overrides IBounded function, Returns an AABB object for collision processing.
    AABB GetAABB() const override;

    // Overrides IBounded function, Returns a BoundingSphere object for collision processing.
    BoundingSphere GetBoundingSphere() const override;

    // Overrides IBounded function, Returns the collision properties for this object
    CollisionProperties* GetCollisionProperties() override;

private:
    // ---------- Local functions --------------

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters

};