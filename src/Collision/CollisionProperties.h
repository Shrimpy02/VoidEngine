#pragma once

// Includes

// Additional Includes

// Forward Declarations
struct CollisionProperties;

// Enum for all types of collision
enum class CollisionType
{
	STATIC,
	DYNAMIC,
	KINETIC
};

// Enum for all collision responses
enum class CollisionResponse
{
	BLOCK,
	OVERLAP,
	IGNORE
};

// Enum for all collision bases
enum class CollisionBase
{
	AABB,
	BoundingSphere
};

/**
 * @struct CollisionProperties
 * @brief A struct holding information on collision type, response and base.
 * Additionally holds checker and setter functions for each.
 */
struct CollisionProperties
{

	CollisionType mType{ CollisionType::STATIC };
	CollisionResponse mResponse{ CollisionResponse::BLOCK };
	CollisionBase mBase{ CollisionBase::AABB };

	// ----------- Functions ---------------

	//TypeChecks

	// Returns true if collision type is STATIC.
	bool IsStatic() const { return mType == CollisionType::STATIC; }

	// Returns true if collision type is DYNAMIC.
	bool IsDynamic() const { return mType == CollisionType::DYNAMIC; }

	// Returns true if collision type is KINETIC.
	bool IsKinetic() const { return mType == CollisionType::KINETIC; }

	// Returns true if collision response is BLOCK.
	bool IsBlockResponse() const { return mResponse == CollisionResponse::BLOCK; }

	// Returns true if collision response is OVERLAP.
	bool IsOverlapResponse() const { return mResponse == CollisionResponse::OVERLAP; }

	// Returns true if collision response is IGNORE.
	bool IsIgnoreResponse() const { return mResponse == CollisionResponse::IGNORE; }

	// Returns true if collision response is BLOCK.
	bool IsAABB() const { return mBase == CollisionBase::AABB; }

	// Returns true if collision response is OVERLAP.
	bool IsBoundingSphere() const { return mBase == CollisionBase::BoundingSphere; }

	// Setters ---------------

	// Sets current collision type
	void SetCollisionType(CollisionType _inType) { mType = _inType; }

	// Sets current collision response
	void SetCollisionResponse(CollisionResponse _inType) { mResponse = _inType; }

	// Sets current collision base
	void SetCollisionBase(CollisionBase _inType) { mBase = _inType; }
};