#pragma once

// includes 
#include <Collision/Collision.h>

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

    // NOTE :: For Handle collision template function it indiscriminately calls intersect regardless of what object it is.
    // So AABB and Bounding sphere intersect functions must be identical in coding technicality. 
    // Definitely a bad way of coding it but i could not think of a different way to call the functions dynamically during runtime. 

    // Returns true if this and an AABB object are colliding
    // Additionally updates the minimum translation vector input with the updated intersection data for collision handling.
    bool IsIntersecting(const struct AABB& _other, glm::vec3* _mtv) const;

    // Overloaded function returns true if this and a BoundingSphere object are colliding
    // Additionally updates the minimum translation vector input with the updated intersection data for collision handling.
    bool IsIntersecting(const BoundingSphere& _other, glm::vec3* _mtv) const;
};
