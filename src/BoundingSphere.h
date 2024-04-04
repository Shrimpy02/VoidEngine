#pragma once

#include <glm/vec3.hpp>

#include "Collision.h"


struct BoundingSphere
{
    // ----------- Variables ---------------

    glm::vec3 mCenter{ 0.0f,0.0f,0.0f };

    float mRadius = 1.f;

    // ----------- Functions ---------------

	BoundingSphere(const glm::vec3& _center, const float _radius) : mCenter(_center), mRadius(_radius) {}



};
