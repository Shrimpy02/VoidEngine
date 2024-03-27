
#include "Mesh.h"
#include "Collision.h"

Mesh* IBounded::CreateCollisionCube(Material* _material, std::vector<Vertex>& _existingMesh, const std::string _key)
{
    /* auto it = Mesh::mCache.find(_key);
     if (it != Mesh::mCache.end())
     {
         return it->second;
     }*/

    glm::vec3 maxExtent(0);
    glm::vec3 minExtent(0);

    // For each axis
    for (int i = 0; i < 3; i++)
    {
        // check all vertices looking for the longest vertex away.  
        for (int j = 0; j < _existingMesh.size(); j++)
        {
            if (_existingMesh[j].mPosition[i] > maxExtent[i])
                maxExtent[i] = _existingMesh[j].mPosition[i];

            else if (_existingMesh[j].mPosition[i] < minExtent[i])
                minExtent[i] = _existingMesh[j].mPosition[i];
        }
    }

    std::vector<Vertex> vertices = {
        // Front face
        {{minExtent.x, minExtent.y,  maxExtent.z}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // Bottom-left
        {{ maxExtent.x, minExtent.y,  maxExtent.z}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // Bottom-right
        {{ maxExtent.x,  maxExtent.y,  maxExtent.z}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // Top-right
        {{minExtent.x,  maxExtent.y,  maxExtent.z}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // Top-left
        // Back face
        {{minExtent.x, minExtent.y, minExtent.z}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ maxExtent.x, minExtent.y, minExtent.z}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{ maxExtent.x,  maxExtent.y, minExtent.z}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        {{minExtent.x,  maxExtent.y, minExtent.z}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
        // Left face
        {{minExtent.x, minExtent.y, minExtent.z}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{minExtent.x, minExtent.y,  maxExtent.z}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{minExtent.x,  maxExtent.y,  maxExtent.z}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{minExtent.x,  maxExtent.y, minExtent.z}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        // Right face
        {{ maxExtent.x, minExtent.y, minExtent.z}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ maxExtent.x, minExtent.y,  maxExtent.z}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ maxExtent.x,  maxExtent.y,  maxExtent.z}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{ maxExtent.x,  maxExtent.y, minExtent.z}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        // Top face
        {{minExtent.x,  maxExtent.y, minExtent.z}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        {{minExtent.x,  maxExtent.y,  maxExtent.z}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ maxExtent.x,  maxExtent.y,  maxExtent.z}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ maxExtent.x,  maxExtent.y, minExtent.z}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        // Bottom face
        {{minExtent.x, minExtent.y, minExtent.z}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{minExtent.x, minExtent.y,  maxExtent.z}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        {{ maxExtent.x, minExtent.y,  maxExtent.z}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ maxExtent.x, minExtent.y, minExtent.z}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}
    };

    std::vector<Index> indices = {
        // Front face
        0, 1, 2, 0, 2, 3,
        // Back face
        4, 5, 6, 4, 6, 7,
        // Left face
        8, 9, 10, 8, 10, 11,
        // Right face
        12, 13, 14, 12, 14, 15,
        // Top face
        16, 17, 18, 16, 18, 19,
        // Bottom face
        20, 21, 22, 20, 22, 23
    };

    mMinExtent = minExtent;
    mMaxExtent = maxExtent;

    //Mesh::mCache[_key] = new Mesh(_key, std::move(vertices), std::move(indices), _material);

    //return Mesh::mCache[_key];

    return new Mesh(_key, std::move(vertices), std::move(indices), _material);
}
