#pragma once

// Includes 
#include <glad/glad.h>
#include <glm/glm.hpp>

/**
 * @struct GraphVertex,
 * @brief A struct containing vertex information for graph related meshes.
 * Also holds the attribute setup for this vertex kind in case there are multiple variants.
 */
struct GraphVertex
{
    // ----------- Variables ---------------

    glm::vec3 mPosition;

    // ----------- Functions ---------------

    // Constructor
    GraphVertex(const glm::vec3& _position)
        : mPosition(_position) {}

    // Custom setup of attributes for this vertex type that contains position, normals and texture coordinates
    static void SetupAttributes()
    {
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GraphVertex), (void*)offsetof(GraphVertex, mPosition));
        glEnableVertexAttribArray(0);
    }

};