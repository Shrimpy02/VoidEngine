#pragma once

// includes
#include <Renderer.h>
#include <Actor.h>
#include <Collision.h>

#include <AABB.h>
#include <BoundingSphere.h>

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

    // The collision mesh
    Mesh* mCollisionCube{ nullptr };
	Mesh* mCollisionSphere{ nullptr };

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // Constructor
    BaseActor(const std::string& _name, Mesh* _mesh);

    // Overriden from IRender passes draw call to mesh. 
    virtual void Draw(const class Shader* _shader = nullptr) const override;

    // Generates an AABB object to process collisions and returns it. 
    virtual AABB GetAABB() const override;

    virtual BoundingSphere GetBoundingSphere() const override;

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
    virtual CollisionProperties GetCollisionProperties() const override;

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
// --------------------- AABBActor ------------------------------
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

    BoundingSphere GetBoundingSphere() const override;

    // Overrides IBounded function, Returns the collision properties for this object
    CollisionProperties GetCollisionProperties() const override;


private:
    // ---------- Local functions --------------

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters

};