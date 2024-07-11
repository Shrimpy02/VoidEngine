#pragma once

// Includes
#include <Collision/Collision.h>
#include <Collision/CollisionProperties.h>

// Additional Includes
#include <glm/vec3.hpp>


/**
 * @class IBounded
 * @brief IBounded class is the base class for all objects that can collide in a scene.
 * It holds variables and functions objects use for collision.
 */
class IBounded
{
public:
    // ---------- Global Variables --------------

	// AABB / boundingSphere variables
    // mCenter is not used currently, better to calc center when calling aabb/boundingsphere for correct scaling
	glm::vec3 mCenter{ 0.f,0.f,0.f };
	glm::vec3 mMaxExtent{ 0.f,0.f,0.f };
	glm::vec3 mMinExtent{ 0.f,0.f,0.f };
    float mRadius{ 0.5f };

    // objects collision properties
    CollisionProperties mCollisionProperties = { CollisionType::STATIC, CollisionResponse::BLOCK, CollisionBase::AABB };

    // Is true if object is colliding with something else
    bool mIsColliding = false;

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // TODO : Get AABB and BoundingSphere should contain logic rather than them (currently) being repeated in each scene actor. 

    // De-constructor
    virtual ~IBounded() = default;

	// Gets an AABB object for collision handling, function = 0 as since it is a base. 
	virtual struct AABB GetAABB() const = 0;

    // Gets an AABB object for collision handling, function = 0 as since it is a base. 
    virtual struct BoundingSphere GetBoundingSphere() const = 0;

	// Gets CollisionProperties object for collision handling, = 0 as base function. 
	//virtual  CollisionProperties& GetCollisionProperties() = 0;
    

    // Returns true if this object is colliding with anything else. 
    bool GetIsColliding() { return mIsColliding; }

    // Sets the value of mIsColliding
    void SetIsColliding(bool _inBool) { mIsColliding = _inBool; }

    // Sets the collision type for this object
	void SetCollisionType(CollisionType _inType) { mCollisionProperties.mType = _inType; }
   
    // Sets the collision response for this object
    void SetCollisionResponse(CollisionResponse _inResponse) { mCollisionProperties.mResponse = _inResponse; }

private:
        // ---------- Local functions --------------

public:
        // ---------- Getters / setters / Adders --------------

        // Getters -----------

        // Setters -----------

};