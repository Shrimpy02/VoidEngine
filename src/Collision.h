#pragma once

struct CollisionProperties;

class IBounded
{
public:
	// ---------- Global Variables --------------

private:
	// ---------- Local Variables --------------

public:
	// ---------- Global functions --------------
	virtual struct AABB GetAABB() const = 0;
	virtual CollisionProperties GetCollisionProperties() const = 0;


	//  class Mesh* CreateCollisionCube(Material* _material, std::vector<Vertex>& _existingMesh, const std::string _key = "CollisionCube");


private:
	// ---------- Local functions --------------

public:
	// ---------- Getters / setters / Adders --------------

};

enum class CollisionType
{
	STATIC,
	DYNAMIC,
	KINETIC
};

enum class CollisionResponse
{
	BLOCK,
	OVERLAP,
	IGNORE
};

struct CollisionProperties
{

private:
    // ----------- Variables ---------------
     // ----------- Functions ---------------
public:
    // ----------- Variables ---------------

	CollisionType mType{ CollisionType::STATIC };
	CollisionResponse mResponse{ CollisionResponse::BLOCK };

	// ----------- Functions ---------------

		//TypeChecks
	bool IsStatic() const { return mType == CollisionType::STATIC; }
	bool IsDynamic() const { return mType == CollisionType::DYNAMIC; }
	bool IsKinetic() const { return mType == CollisionType::KINETIC; }

	// Response checks
	bool IsBlockResponse() const { return mResponse == CollisionResponse::BLOCK; }
	bool IsOverlapResponse() const { return mResponse == CollisionResponse::OVERLAP; }
	bool IsIgnoreResponse() const { return mResponse == CollisionResponse::IGNORE; }

    // Adders ---------------


    // Setters ---------------
	void SetCollisionType(CollisionType _inType) { mType = _inType; }
	void SetCollisionResponse(CollisionResponse _inType) { mResponse = _inType; }

    // Getters ---------------


};