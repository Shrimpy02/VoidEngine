#pragma once
struct CollisionProperties;

class IBounded
{
public:
    // ---------- Global Variables --------------

    glm::vec3 mMinExtent;
    glm::vec3 mMaxExtent;

private:
    // ---------- Local Variables --------------
    bool mIsColliding = false;

public:
    // ---------- Global functions --------------
    class Mesh* CreateCollisionCube(Material* _material, std::vector<Vertex>& _existingMesh, const std::string _key = "CollisionCube");

    virtual struct AABB GenAABB() = 0;
    virtual struct BoundingSphere GenBoundingSphere() = 0;

private:
    // ---------- Local functions --------------

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters
    void SetIsColliding(bool _isColiding) { mIsColliding = _isColiding; }

    // Getters
    virtual CollisionProperties* GetCollisionProperties() = 0;
    bool IsColliding() { return mIsColliding; }
    glm::vec3 GetMinExtent() { return mMinExtent; }
    glm::vec3 GetMaxExtent() { return mMaxExtent; }


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