#pragma once

// Includes
#include <glm/vec3.hpp>
#include <Utilities/Types.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/Vertex.h>

// Additional Includes
#include <memory> 
#include <vector>

#include "Collision.h"

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

// ---------------------------------------------------------------
// --------------------- IBounded --------------------------------
// ---------------------------------------------------------------

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
    //CollisionProperties mCollisionProperties = { CollisionType::STATIC, CollisionResponse::BLOCK, CollisionBase::AABB };

    // Decides if mCollisionMesh (if there is one) should be rendered.
    bool mShouldDrawCollisionMesh = true;
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

    // Helper functions to create default cube meshe based on existing mesh vertex geometry.
	std::shared_ptr<Mesh> CreateCollisionCubeFromMesh(std::shared_ptr<Material> _material, std::vector<struct Vertex>& _existingMesh)
	{
        // Calculate the bounding box (min and max extents) of the existing mesh
        glm::vec3 maxExtent = _existingMesh[0].mPosition;
        glm::vec3 minExtent = _existingMesh[0].mPosition;

        for (const auto& vertex : _existingMesh)
        {
            minExtent = glm::min(minExtent, vertex.mPosition);
            maxExtent = glm::max(maxExtent, vertex.mPosition);
        }

        // Calculate the center of the bounding box
        glm::vec3 center = (minExtent + maxExtent) * 0.5f;

        // set the class values to the calculated values
        mMaxExtent = maxExtent;
        mMinExtent = minExtent;

        // Then create the collision mesh using these extents (Mesh as cube for AABB object)
        // generate a cube using extents
        std::vector<Vertex> vertices = {
            // Front face
            {{minExtent.x, minExtent.y,  maxExtent.z}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // Bottom-left
            {{ maxExtent.x, minExtent.y,  maxExtent.z}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // Bottom-right
            {{ maxExtent.x,  maxExtent.y,  maxExtent.z}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // Top-right
            {{minExtent.x,  maxExtent.y,  maxExtent.z}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // Top-left
            // Back face
            {{minExtent.x, minExtent.y, minExtent.z}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
            {{ maxExtent.x, minExtent.y, minExtent.z}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
            {{ maxExtent.x,  maxExtent.y, minExtent.z}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
            {{minExtent.x,  maxExtent.y, minExtent.z}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
            // Left face
            {{minExtent.x, minExtent.y, minExtent.z}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
            {{minExtent.x, minExtent.y,  maxExtent.z}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
            {{minExtent.x,  maxExtent.y,  maxExtent.z}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
            {{minExtent.x,  maxExtent.y, minExtent.z}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
            // Right face
            {{ maxExtent.x, minExtent.y, minExtent.z}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
            {{ maxExtent.x, minExtent.y,  maxExtent.z}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
            {{ maxExtent.x,  maxExtent.y,  maxExtent.z}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
            {{ maxExtent.x,  maxExtent.y, minExtent.z}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
            // Top face
            {{minExtent.x,  maxExtent.y, minExtent.z}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
            {{minExtent.x,  maxExtent.y,  maxExtent.z}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
            {{ maxExtent.x,  maxExtent.y,  maxExtent.z}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
            {{ maxExtent.x,  maxExtent.y, minExtent.z}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
            // Bottom face
            {{minExtent.x, minExtent.y, minExtent.z}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
            {{minExtent.x, minExtent.y,  maxExtent.z}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
            {{ maxExtent.x, minExtent.y,  maxExtent.z}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
            {{ maxExtent.x, minExtent.y, minExtent.z}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}
        };

        // Generate cube indices
        std::vector<Index> indices = {
            // Front face
            0, 1, 2, 0, 2, 3,
            // Back face
            4, 5, 6, 4, 6, 7,
            // Left face
            8, 9, 10, 8, 10, 11,
            // Right face
            12, 13, 14, 12, 14, 15,
            // Top face
            16, 17, 18, 16, 18, 19,
            // Bottom face
            20, 21, 22, 20, 22, 23
        };

        return std::make_shared<Mesh>("CollisionCube", std::move(vertices), std::move(indices), _material);
    }

    // Helper functions to create default cube meshe based on existing mesh vertex geometry.
    std::shared_ptr<Mesh> CreateCollisionSphereFromMesh(std::shared_ptr<Material> _material, std::vector<struct Vertex>& _existingMesh)
    {
        // default extent init
        glm::vec3 maxExtent(0);
        glm::vec3 minExtent(0);
        glm::vec3 center(0);
        float largetsDiff(0.f);

        // For each
        for (int i = 0; i < 3; i++)
        {
            glm::vec3 pos(0);

            // check all vertices looking for the longest vertex away.  
            for (int j = 0; j < _existingMesh.size(); j++)
            {
                // Gets max extent for axis
                if (_existingMesh[j].mPosition[i] > maxExtent[i])
                    maxExtent[i] = _existingMesh[j].mPosition[i];

                // Gets min extent for axis
                else if (_existingMesh[j].mPosition[i] < minExtent[i])
                    minExtent[i] = _existingMesh[j].mPosition[i];

                // Gets center location for axis 
                pos[i] = _existingMesh[j].mPosition[i];

                // Finds the vertex that is the furthest from the center
                if (largetsDiff < abs(_existingMesh[j].mPosition[i]))
                    largetsDiff = abs(_existingMesh[j].mPosition[i]);
            }
            center += pos;
        }

        // Divide by num vertices for average location of center
        center /= static_cast<float>(_existingMesh.size());

        // set the class values to the calculated values
        mRadius = (float)largetsDiff * (float)1.5;

        std::vector<Vertex> vertices;
        std::vector<Index> indices;
        Mesh::GenSphere(vertices,indices,2,mRadius);

        return std::make_shared<Mesh>("CollisionSphere", std::move(vertices), std::move(indices), _material);
    }

    // Returns true if this object is colliding with anything else. 
    bool GetIsColliding() { return mIsColliding; }

    // Sets the value of mIsColliding
    void SetIsColliding(bool _inBool) { mIsColliding = _inBool; }

    // Sets the collision type for this object
   // void SetCollisionType(CollisionType _inType) { mCollisionProperties.mType = _inType; }
   //
   // // Sets the collision response for this object
   // void SetCollisionResponse(CollisionResponse _inResponse) { mCollisionProperties.mResponse = _inResponse; }

private:
        // ---------- Local functions --------------

public:
        // ---------- Getters / setters / Adders --------------

        // Getters -----------

        // Setters -----------

};