#pragma once

// Includes
#include <glm/vec3.hpp>
#include <Logger.h>

#include "BoundingSphere.h"

/**
 * @struct AABB
 * @brief A class used for handling collision interactions, it checks intersection when used by extent and centre.
 * Is good for heap memory as it is small and has a small life span. 
 */
 struct AABB
{
    // ----------- Variables ---------------

 	// Center of collision object 
    glm::vec3 mCenter{ 0.0f,0.0f,0.0f };
    // Extent of collision object
    glm::vec3 mExtent{ 0.5f,0.5f,0.5f };

    // ----------- Functions ---------------

    // Constructor
    AABB(const glm::vec3& _center, const glm::vec3& _extent) : mCenter(_center), mExtent(_extent) {}

    // Returns true if this and an AABB are colliding
	// Additionally updates the minimum translation vector input with the updated intersection data for collision handling.
    bool IsIntersecting(const AABB& _other, glm::vec3* _mtv) const;
 

    // Overloaded function returns true if this and a BoundingSphere are colliding
 	// Additionally updates the minimum translation vector input with the updated intersection data for collision handling.
    bool IsIntersecting(const BoundingSphere& _other, glm::vec3* _mtv) const;

};

