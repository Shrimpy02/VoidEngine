#pragma once

// Includes
#include <vector>
#include <glm/vec3.hpp>
#include <Types.h>
#include <Mesh.h>

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

/**
 * @struct CollisionProperties
 * @brief A struct holding information on collision type and response.
 * Additionally holds checker and setter functions for types and responses.
 */
struct CollisionProperties
{
	CollisionType mType{ CollisionType::STATIC };
	CollisionResponse mResponse{ CollisionResponse::BLOCK };

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

	// Setters ---------------

	// Sets current collision type
	void SetCollisionType(CollisionType _inType) { mType = _inType; }

	// Sets current collision response
	void SetCollisionResponse(CollisionResponse _inType) { mResponse = _inType; }
};

// ---------------------------------------------------------------
// --------------------- IBounded --------------------------------
// ---------------------------------------------------------------

/**
 * @class IBounded
 * @brief IBounded class is the base class for all objects that can collide in a scene.
 * It Holds the GetCollisionTypes and GetCollisionProperties functions.  
 */
class IBounded
{
public:

	// AABB variables
	glm::vec3 mCenter{ 0.f,0.f,0.f };
	glm::vec3 mMaxExtent{ 0.f,0.f,0.f };
	glm::vec3 mMinExtent{ 0.f,0.f,0.f };

    // objects collision properties
    CollisionProperties mCollisionProperties{ CollisionType::STATIC, CollisionResponse::BLOCK };

	// ---------- Global functions --------------

	// Gets an AABB object for collision handling, function = 0 as since it is a base. 
	virtual struct AABB GetAABB() const = 0;

	// Gets CollisionProperties object for collision handling, = 0 as base function. 
	virtual struct CollisionProperties GetCollisionProperties() const = 0;

	class Mesh* CreateCollisionCube(class Material* _material, std::vector<struct Vertex>& _existingMesh)
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
        mCenter = center;

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

        return new Mesh("CollisionMesh", std::move(vertices), std::move(indices), _material);
    }

};
