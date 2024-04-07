#pragma once

// Includes 
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

/**
 * @struct Vertex,
 * @brief A struct containing vertex information like position, normal and texture coordinates.
 * Also holds the Attribute setup for this vertex kind in case there are multiple variants. 
 */
struct Vertex
{
    // ----------- Variables ---------------

    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec2 mTexCoords;

    // ----------- Functions ---------------

    // Constructor
    Vertex(const glm::vec3& _position, const glm::vec3& _normal, const glm::vec2& _texCoords)
        : mPosition(_position), mNormal(_normal), mTexCoords(_texCoords) {}

    // Custom setup of attributes for this vertex type that contains position, normals and texture coordinates
    static void SetupAttributes()
    {
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mPosition));
        glEnableVertexAttribArray(0);

        // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mNormal));
        glEnableVertexAttribArray(1);

        // texture coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mTexCoords));
        glEnableVertexAttribArray(2);
    }

    
};