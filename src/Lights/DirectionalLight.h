#pragma once

// Includes
#include <Lights/Lights.h>
#include <Actor.h>

// Additional Includes

// Forward Declarations

/**
 * @class DirectionalLight,
 * @brief Inherits from light class, contains direction for light that hits the scene equally.
 * Simulates for example the sun.
 */
class DirectionalLight : public Light
{
public:
	// Returns the directional lights direction as a vec3
	virtual glm::vec3 GetDirection() const = 0;
};

// ---------------------------------------------------------------
// -------------- DirectionalLightActor --------------------------
// ---------------------------------------------------------------

/**
 * @class DirectionalLightActor,
 * @brief The DirectionalLightActor inherits from DirectionalLight class
 * for light properties and Actor class for scene inclusion.
 */
class DirectionalLightActor : public DirectionalLight, public Actor
{
public:
	// constructs as a regular actor
	DirectionalLightActor(const std::string& name) : Actor(name) {}

	// Inherited function returns the direction of this light source as a vec3.
	glm::vec3 GetDirection() const override
	{
		return GetRotation(TransformSpace::Local) * glm::vec3(0.f, 0.f, -1.f);
	}
};