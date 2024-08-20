#pragma once

// Includes
#include <Collision/CollisionProperties.h>

// Additional Includes
#include <memory>
#include <glm/vec3.hpp>

// Forward Declarations
struct CollisionProperties;

/**
 * @class IBounded
 * @brief IBounded class is the base class for all objects that can collide in a scene.
 * It holds variables and functions objects use for collision.
 */
class IBounded
{
public:
    // ---------- Global Variables --------------

	glm::vec3 mCenter{ 0.f,0.f,0.f };
    glm::vec3 mMinExtent{ 0.f,0.f,0.f };
    glm::vec3 mMaxExtent{ 0.f,0.f,0.f };
    glm::vec3 mExtent{ 0.f,0.f,0.f };
    float mRadius{ 0.5f };

    // objects collision properties
    CollisionProperties mCollisionProperties = { CollisionType::STATIC, CollisionResponse::BLOCK, CollisionBase::AABB };

    // Is true if object is colliding with something else
    bool mIsColliding = false;

    // Is true if object has changed collision base this tick
    bool mChangedCollisionBase = false;

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // De-constructor
    virtual ~IBounded() = default;

    // Returns true if actor collides with input actor and updates mtv vector accordingly
    bool isIntersecting(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool AABBx2(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool BoundingSpherex2(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool AABBxBoundingSphere(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool Convexx2(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool ConvexxNotConvex(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool AABBxPoint(glm::vec3 _pointPos);

    // Returns true if this object is colliding with anything else. 
    bool GetIsColliding() { return mIsColliding; }

    // Sets the value of mIsColliding
    void SetIsColliding(bool _inBool) { mIsColliding = _inBool; }

    // Sets the collision type for this object
	void SetCollisionType(CollisionType _inType) { mCollisionProperties.mType = _inType; }
   
    // Sets the collision response for this object
    void SetCollisionResponse(CollisionResponse _inResponse) { mCollisionProperties.mResponse = _inResponse; }

    // Sets the collision base for this object, should be done 
    void SetCollisionBase(CollisionBase _inBase) { mCollisionProperties.mBase = _inBase; }


private:
        // ---------- Local functions --------------

	
public:
        // ---------- Getters / setters / Adders --------------

        // Getters -----------

        // Setters -----------

};