#pragma once

// Includes 
#include <glad/glad.h>
#include <glm/glm.hpp>

/**
 * @struct GraphPoint
 * @brief A struct containing point information.
 * Also holds the Attribute setup for this point.
 */
struct GraphPoint
{
    // ----------- Variables ---------------

    glm::vec3 mPosition;

    // ----------- Functions ---------------

    // Constructor
    GraphPoint(const glm::vec3& _position)
        : mPosition(_position) {}

    // Custom setup of attributes for this vertex type that contains position, normals and texture coordinates
    static void SetupAttributes()
    {
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GraphPoint), (void*)offsetof(GraphPoint, mPosition));
        glEnableVertexAttribArray(0);
    }


};