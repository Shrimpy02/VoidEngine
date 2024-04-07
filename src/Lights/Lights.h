#pragma once

// Includes
#include <glm/vec3.hpp>

/**
 * @class Light,
 * @brief The Light class is the base class for all lights,
 * it contains all general light properties like color and ambient strength.
 */
class Light
{
public:
	// virtual de-constructor
	virtual ~Light() = default;

	// ambient strength
	glm::vec3 mAmbient{ 0.5f,0.5f,0.5f };

	// light color
	glm::vec3 mColor{ 1.0f,1.0f,1.0f };

};