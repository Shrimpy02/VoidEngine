#pragma once

// Includes 
#include <glad/glad.h>
#include <glm/glm.hpp>

/**
 * @struct PointCloudVertex,
 * @brief A struct containing vertex information for terrain related meshes.
 * Also holds the attribute setup for this vertex kind in case there are multiple variants.
 */
struct PointCloudVertex
{
    // ----------- Variables ---------------

    glm::vec3 mPosition;
    glm::vec3 mColor;

    // ----------- Functions ---------------

    // Constructor
    PointCloudVertex(const glm::vec3& _position, const glm::vec3& _color)
        : mPosition(_position), mColor(_color) {}

    // Custom setup of attributes for this vertex type that contains position, normals and texture coordinates
    static void SetupAttributes()
    {
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PointCloudVertex), (void*)offsetof(PointCloudVertex, mPosition));
        glEnableVertexAttribArray(0);

        // Color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PointCloudVertex), (void*)offsetof(PointCloudVertex, mColor));
        glEnableVertexAttribArray(1);
    }
};