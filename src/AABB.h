#pragma once

// Includes
#include "glm/vec3.hpp"

/**
 * @struct AABB
 * @brief A class used for handling collision interactions, it checks intersection when used by extent and centre.
 * Is good for heap memory as it is small and has a small life span. 
 */
 struct AABB
{
    // ----------- Variables ---------------

 	// Center of object 
    glm::vec3 center{ 0.0f,0.0f,0.0f };
    // Extent of object
    glm::vec3 extent{ 0.5f,0.5f,0.5f };

    // ----------- Functions ---------------

    // Constructor
    AABB(const glm::vec3& _center, const glm::vec3& _extent) : center(_center), extent(_extent) {}

    // Returns true if this and other are intersecting.
    // Additionally updates the minimum translation vector input with the updated intersection data.
    bool IsIntersecting(const AABB& _other, glm::vec3* _mtv) const;
};

