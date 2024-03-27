#pragma once

#include <glm/vec3.hpp>

#include "Collision.h"


struct BoundingSphere
{
private:
    // ----------- Variables ---------------

     // ----------- Functions ---------------

public:
    // ----------- Variables ---------------
    glm::vec3 center{ 0.0f,0.0f,0.0f };
    glm::vec3 extent{ 0.5f,0.5f,0.5f };

    // ----------- Functions ---------------
    BoundingSphere(const glm::vec3& center, const glm::vec3& extent) : center(center), extent(extent) {}

    bool IsIntersecting(const AABB& other, glm::vec3* mtv) const
    {
        
    }
    bool IsIntersecting(const BoundingSphere& other, glm::vec3* mtv) const
    {

    }

    // Adders ---------------


    // Setters ---------------



    // Getters ---------------

};
