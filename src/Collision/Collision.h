#pragma once

// Includes
#include <Collision/CollisionProperties.h>

// Additional Includes
#include <memory>
#include <glm/vec3.hpp>

// Forward Declarations
struct CollisionProperties;
class VisualActor;
class Actor;

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

    //tmp
    bool isWeapon = false;
    bool isPlayer = false;
    bool isBullet = false;
    bool isEnemy = false;

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

    // Collision Checkers
    // Returns true if actor collides with input actor and updates mtv vector accordingly
    bool IsIntersecting(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool IsIntersectingLineTrace(glm::vec3 _point);

    bool IsIntersectingConstrictingBoxGeometry(std::shared_ptr<VisualActor> _boxCollider, glm::vec3* _mtv, glm::vec3* _boundaryNormal);

    bool IsIntersectingConstrictingBoxGeometry(glm::vec3 _min, glm::vec3 _max);

    // Collision Proccesors

	bool AABBx2(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool BoundingSpherex2(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool AABBxBoundingSphere(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool AABBxBoundingSphere(std::pair <glm::vec3, glm::vec3> _extents);

    bool Convexx2(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool ConvexxNotConvex(std::shared_ptr<IBounded> _otherCollider, glm::vec3* _mtv);

    bool AABBxPoint(glm::vec3 _pointPos);

    bool BoundingSpherexPoint(glm::vec3 _pointPos);

    bool BoundingSpherexConstrictingBox(std::shared_ptr<VisualActor> _boxCollider, glm::vec3* _mtv, glm::vec3* _boundaryNormal);

    bool BoundingSpherexConstrictingBoxContain(glm::vec3 _minExtent, glm::vec3 _maxExtent);

    bool BoundingSpherexConstrictingBoxIntersect(glm::vec3 _minExtent, glm::vec3 _maxExtent);

    // _elasticityCoefficient = 1 is completly elastic collision.
    static void BoundingSpherex2PhysicsCollision(std::shared_ptr<Actor> _colliderA, std::shared_ptr<Actor> _colliderB, float _elasticityCoefficient = 1.0f);

    // _elasticityCoefficient = 1 is completly elastic collision.
    static void BoundingSpherexBoundryPhysicsCollision(std::shared_ptr<Actor> _collider, std::shared_ptr<VisualActor> _conformCollider, glm::vec3 _boundaryNormal, float _elasticityCoefficient = 1.0f);


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