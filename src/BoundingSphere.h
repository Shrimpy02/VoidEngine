#pragma once

// includes 
#include "Collision.h"

/**
 * @struct BoundingSphere
 * @brief A class used for handling collision interactions,
 * it checks intersection between other collision using center and radius.
 * Is good for heap memory as it is small and has a small life span.
 */
struct BoundingSphere
{
    // ----------- Variables ---------------

	// Center of collision object 
    glm::vec3 mCenter{ 0.0f,0.0f,0.0f };

    // Radius of collision object
    float mRadius = 0.5f;

    // ----------- Functions ---------------

	// Constructor
	BoundingSphere(const glm::vec3& _center, const float _radius) : mCenter(_center), mRadius(_radius) {}

    bool IsIntersecting(const struct AABB& _other, glm::vec3* _mtv) const;

    bool IsIntersecting(const BoundingSphere& _other, glm::vec3* _mtv) const;
};
