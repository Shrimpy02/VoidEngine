#pragma once

// Includes 
#include <assimp/scene.h>
#include <glm/glm.hpp>

// Additional Includes

// Forward Declarations
class aiNode;


// Inline function returns transformation matrix of an in aiNode* as an aiMatrix4x4
inline aiMatrix4x4 GetTransformationMatrix(const aiNode* node)
{
	if (node->mParent == nullptr) // Root node
	{ 
		return node->mTransformation;
	}
	else
	{
		return node->mParent->mTransformation * GetTransformationMatrix(node->mParent);
	}
}

// Inline function returns transformed positions from an in transfor matrix and in position as an ai-Vector3D
inline aiVector3D TransformPosition(const aiMatrix4x4& transform, const aiVector3D& position)
{
	aiVector3D result;
	result.x = transform.a1 * position.x + transform.a2 * position.y + transform.a3 * position.z + transform.a4;
	result.y = transform.b1 * position.x + transform.b2 * position.y + transform.b3 * position.z + transform.b4;
	result.z = transform.c1 * position.x + transform.c2 * position.y + transform.c3 * position.z + transform.c4;
	return result;
}

// Inline function returns a glm matrix from an in aiMatrix4x4 as a glm::mat4
inline glm::mat4 AiMatrix4x4ToGlm(const aiMatrix4x4& from)
{
	glm::mat4 to;

	to[0][0] = from.a1; to[1][0] = from.a2;
	to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2;
	to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2;
	to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2;
	to[2][3] = from.d3; to[3][3] = from.d4;

	return to;
}