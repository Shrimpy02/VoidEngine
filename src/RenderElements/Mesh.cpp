// Includes
#include <RenderElements/Mesh.h>
#include <RenderElements/Vertex.h>
#include <RenderElements/GraphVertex.h>
#include <RenderElements/DebugVertex.h>
#include <RenderElements/Material.h>
#include <Utilities/Logger.h>
#include <Utilities/Defines.h>
#include <corecrt_math_defines.h>

#include "Texture.h"


// static cache of meshes
std::unordered_map<std::string, std::shared_ptr<Mesh>> Mesh::mCache;

Mesh::Mesh(const std::string _name, std::vector<Vertex>&& _vertices, std::vector<Index>&& _indices, std::shared_ptr<Material> material)
    : mName(_name), mVertices(std::move(_vertices)), mIndices(std::move(_indices)), mMaterial(material)
{
    // generates gl specific buffers for mesh init.
    SetupMesh();

    if(!mMaterial)
        mMaterial = Material::Load(mName, {Texture::LoadWhiteTexture()}, {});

}

Mesh::Mesh(const std::string _name, std::vector<GraphVertex>&& _vertices, std::vector<Index>&& _indices)
    : mName(_name), mGraphVertices(std::move(_vertices)), mIndices(std::move(_indices))
{
    SetupGraphMesh();
}

Mesh::Mesh(const std::string _name, std::vector<DebugVertex>&& _vertices)
    : mName(_name), mDebugVertices(std::move(_vertices))
{
    SetupDebugMesh();
}

Mesh::~Mesh()
{
    // deletes gl mesh buffers
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}

void Mesh::Draw(const std::shared_ptr<Shader> _shader) const
{
    if (!mVisible) return;

    if(mMaterial)
        mMaterial->Bind(_shader);
    
    // binds VAO and draws all geometry by given indices and vertices
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mIndices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::DrawDebugLines(const std::shared_ptr<Shader> _shader) const
{
    if (!mVisible) return;

    if (mMaterial)
        mMaterial->Bind(_shader);

    // binds VAO and draws all geometry by given indices and vertices
    glBindVertexArray(mVAO);
    glLineWidth(10.0f);
    glDrawArrays(GL_LINES, 0,static_cast<GLsizei>(mDebugVertices.size()));
    //glDrawElements(GL_LINES, static_cast<GLsizei>(mIndices.size()), GL_UNSIGNED_INT, 0);
	glLineWidth(1.0f);
	glBindVertexArray(0);
}

std::shared_ptr<Mesh> Mesh::CreateCube(std::shared_ptr<Material> _material, std::string _customName)
{
    // Create default cube key
    std::string cubeKey = "DefaultCube";

    if(!_customName.empty()){
        // Overwrites default key if custom name is added.
        cubeKey = _customName;
    }

    // Enable this for instance like behaviour
    //// Checks if default cube exists in cache,if it does use that
    //// rather than calculating an entire new cube to save loading time.
    //auto it = mCache.find(cubeKey);
    //if (it != mCache.end())
    //{
    //   // LOG("%s Loaded", cubeKey.c_str());
    //    return it->second;
    //}

    // otherwise gen vertices and indices from vector
    std::vector<Vertex> vertices = {
        // Front face
        {{-0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // Bottom-left
        {{ 0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // Bottom-right
        {{ 0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // Top-right
        {{-0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // Top-left
        // Back face
        {{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
        // Left face
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        // Right face
        {{ 0.5f, -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        // Top face
        {{-0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}
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

	std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(cubeKey, std::move(vertices), std::move(indices), _material);

    // Create mesh moveing the vertices and indices into new object along with input material and add it to cache
    mCache[cubeKey] = cube;

    // return new default cube
    //LOG("%s created", cubeKey.c_str());
	return cube;
}

std::shared_ptr<Mesh> Mesh::CreateCubeByExtent(std::shared_ptr<Mesh> _extentMesh, std::shared_ptr<Material> _material, std::string _customName)
{
    // Calculate the bounding box (min and max extents) of the existing mesh
    glm::vec3 maxExtent = _extentMesh->mVertices[0].mPosition;
    glm::vec3 minExtent = _extentMesh->mVertices[0].mPosition;

    for (Vertex& vertex : _extentMesh->mVertices)
    {
        minExtent = glm::min(minExtent, vertex.mPosition);
        maxExtent = glm::max(maxExtent, vertex.mPosition);
    }

    // Slight offsett
    maxExtent += 0.001f;
    minExtent -= 0.001f;

    // Then create the collision mesh using these extents (Mesh as cube for AABB object)
    // generate a cube using extents
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

    // Generate cube indices
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

    return std::make_shared<Mesh>(_customName, std::move(vertices), std::move(indices), _material);
}

std::shared_ptr<Mesh> Mesh::CreatePlane(std::shared_ptr<Material> _material, std::string _customName)
{
    // Create default plane key
    std::string planeKey = "DefaultPlane";

    if (!_customName.empty()) {
        // Overwrites default key if custom name is added.
        planeKey = _customName;
    }

    // Checks if default plane exists in cache,if it does use that
	// rather than calculating an entire new cube to save loading time.
    auto it = mCache.find(planeKey);
    if (it != mCache.end())
    {
        //LOG("%s Loaded", planeKey.c_str());
        return it->second;
    }

    // otherwise gen vertices and indices from vector
    std::vector<Vertex> vertices = {
        // Front face
        {{-0.5f, 0.0f,  -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ -0.5f, 0.0f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.0f,  -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        {{0.5f,  0.0f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
    };
    std::vector<Index> indices = {
        // Front face
        0, 1, 2, 1, 2, 3
    };

    // Create mesh moveing the vertices and indices into new object along with input material and add it to cache
    mCache[planeKey] = std::make_shared<Mesh>(planeKey, std::move(vertices), std::move(indices), _material);

    // return new default plane
   // LOG("%s Created", planeKey.c_str());
    return mCache[planeKey];
}

std::shared_ptr<Mesh> Mesh::CreatePyramid(std::shared_ptr<Material> _material, std::string _customName)
{
    // Create default pyramid key
    std::string pyramidKey = "DefaultPyramid";

    if (!_customName.empty()) {
        // Overwrites default key if custom name is added.
        pyramidKey = _customName;
    }

    // Checks if default pyramid existin cache,if it does use that
	// rather than calculating an entire new cube to save loading time.
    auto it = mCache.find(pyramidKey);
    if (it != mCache.end())
    {
       // LOG("%s Loaded", pyramidKey.c_str());
        return it->second;
    }

    // otherwise gen vertices and indices from vector
    std::vector<Vertex> vertices = {
        // Base
       {{-0.5f, 0.0f,  -0.5f}, {0.0f,  -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ -0.5f, 0.0f,  0.5f}, {0.0f,  -1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.0f,  -0.5f}, {0.0f,  -1.0f,  0.0f}, {0.0f, 1.0f}},
		 {{0.5f,  0.0f,  0.5f}, {0.0f,  -1.0f,  0.0f}, {1.0f, 1.0f}},

        // Top
         {{0.0f,  0.5f,  0.0f}, {0.0f,  1.0f,  0.0f}, {0.5f, 0.5f}},
    };
    std::vector<Index> indices = {
        // Base
        0, 1, 2,  // Triangle 1: Base
        1, 3, 2,  // Triangle 2: Base

        // Sides
        0, 4, 1,  // Triangle 3: Side 1
        1, 4, 3,  // Triangle 4: Side 2
        3, 4, 2,  // Triangle 5: Side 3
        2, 4, 0   // Triangle 6: Side 4
    };


    // Create mesh moveing the vertices and indices into new object along with input material and add it to cache
    mCache[pyramidKey] = std::make_shared<Mesh>(pyramidKey, std::move(vertices), std::move(indices), _material);

    // return new default pyramid
    //LOG("%s Created", pyramidKey.c_str());
    return mCache[pyramidKey];
}

std::shared_ptr<Mesh> Mesh::CreateSphereByExtent(std::shared_ptr<Mesh> _extentMesh, std::shared_ptr<Material> _material, std::string _customName)
{
    // Calculate the bounding box (min and max extents) of the existing mesh
    glm::vec3 maxExtent = _extentMesh->mVertices[0].mPosition;
    glm::vec3 minExtent = _extentMesh->mVertices[0].mPosition;

    for (Vertex& vertex : _extentMesh->mVertices)
    {
        minExtent = glm::min(minExtent, vertex.mPosition);
        maxExtent = glm::max(maxExtent, vertex.mPosition);
    }

    // Slight offsett
    maxExtent += 0.001f;
    minExtent -= 0.001f;

    float radius = 0.0f;

    if(glm::length(minExtent) > glm::length(maxExtent))
    {
        radius = glm::length(minExtent);
    } else {
        radius = glm::length(maxExtent);
    }
    
    std::vector<Vertex> vertices;
    std::vector<Index> indices;
    Mesh::GenSphere(vertices, indices, 2, radius);

    return std::make_shared<Mesh>(_customName, std::move(vertices), std::move(indices), _material);
}

std::shared_ptr<Mesh> Mesh::CreateSphere(std::shared_ptr<Material> _material, const int _subdivides, std::string _customName)
{
    // Create default sphere key based on num subdivides
    std::string sphereKey = "DefaultSphere" + std::to_string(_subdivides);

    if (!_customName.empty()) {
        // Overwrites default key if custom name is added.
        sphereKey = _customName;
    }

    // Checks if default sphere exists in cache,if it does use that
     // rather than calculating an entire new cube to save loading time.
    auto it = mCache.find(sphereKey);
    if (it != mCache.end())
    {
       // LOG("%s Loaded", sphereKey.c_str());
        return it->second;
    }

    // otherwise gen vertices and indices from vector
    std::vector<Vertex> vertices;
    std::vector<Index> indices; 

    // Takes vertices and indices vectors as input and populates them with relevant geometry data.
    // Can probably be optimizing quite a bit.
    // TODO: fix textrure warping
	GenSphere(vertices,indices,_subdivides);

    // Create mesh moveing the vertices and indices into new object along with input material and add it to cache
    mCache[sphereKey] = std::make_shared<Mesh>(sphereKey, std::move(vertices), std::move(indices), _material);

    // return new default sphere
    //LOG("%s created", sphereKey.c_str());
    return mCache[sphereKey];
}

std::shared_ptr<Mesh> Mesh::CreateGraphSphere(const int _subdivides, std::string _customName)
{
    // Create default sphere key based on num subdivides
    std::string sphereKey = "DefaultGraphSphere" + std::to_string(_subdivides);

    if (!_customName.empty()) {
        // Overwrites default key if custom name is added.
        sphereKey = _customName;
    }

    // Checks if default sphere exists in cache,if it does use that
     // rather than calculating an entire new cube to save loading time.
    auto it = mCache.find(sphereKey);
    if (it != mCache.end())
    {
        // LOG("%s Loaded", sphereKey.c_str());
        return it->second;
    }

    // otherwise gen vertices and indices from vector
    std::vector<GraphVertex> vertices;
    std::vector<Index> indices;

    GenSphere(vertices, indices, _subdivides);

    // Create mesh moveing the vertices and indices into new object along with input material and add it to cache
    mCache[sphereKey] = std::make_shared<Mesh>(sphereKey, std::move(vertices), std::move(indices));

    // return new default sphere
    //LOG("%s created", sphereKey.c_str());
    return mCache[sphereKey];
}

std::shared_ptr<Mesh> Mesh::CreateDebugLine(std::vector<glm::vec3> _points)
{
    // Create default plane key
    std::string lineKey = "DebugLine";

    std::vector<DebugVertex> vertices;

    for (glm::vec3 pointLocation : _points)
    {
        DebugVertex newVertex = DebugVertex(pointLocation);
        vertices.push_back(newVertex);
    }

    // Create mesh moveing the vertices and indices into new object along with input material and add it to cache
    return std::make_shared<Mesh>(lineKey, std::move(vertices));
}

std::shared_ptr<Mesh> Mesh::Load(const std::string& _key)
{
    // Find key in cache and load that mesh
    auto it = mCache.find(_key);
    if (it != mCache.end())
        return it->second;
    
    LOG_WARNING("%s: Not found in cache for loading", _key.c_str());
    return nullptr;
}

void Mesh::Unload(const std::string& _key)
{
    // Find key in cache and unload that mesh from the cache-
    auto it = mCache.find(_key);
    if (it != mCache.end())
    {
        mCache.erase(it);
    }
    LOG_WARNING("%s: Not found in cache for unloading", _key.c_str());
}


void Mesh::ClearCache()
{
    mCache.clear();
}

void Mesh::SetupMesh()
{
    // Generates VAO
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Gen VBO and assign mesh vertices to the buffer.
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

    // Gen EBO and assign mesh indices to the buffer.
    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(Index), mIndices.data(), GL_STATIC_DRAW);

    // setts upp the vertex attributes
    Vertex::SetupAttributes();

    // unbinds the VAO once finished
    glBindVertexArray(0);
}

void Mesh::SetupGraphMesh()
{
    // Generates VAO
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Gen VBO and assign mesh vertices to the buffer.
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mGraphVertices.size() * sizeof(GraphVertex), mGraphVertices.data(), GL_STATIC_DRAW);

    // Gen EBO and assign mesh indices to the buffer.
    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(Index), mIndices.data(), GL_STATIC_DRAW);

    // setts upp the vertex attributes
    GraphVertex::SetupAttributes();

    // unbinds the VAO once finished
    glBindVertexArray(0);
}


void Mesh::SetupDebugMesh()
{
    // Generates VAO
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Gen VBO and assign mesh vertices to the buffer.
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mDebugVertices.size() * sizeof(DebugVertex), mDebugVertices.data(), GL_STATIC_DRAW);

    // setts upp the vertex attributes
    DebugVertex::SetupAttributes();

    // unbinds the VAO once finished
    glBindVertexArray(0);
}

void Mesh::GenSphere(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const int _numSubdivides, float _radius)
{
    // initial vertex position to create a base mesh
    glm::vec3 v0 = glm::vec3(0, 0, _radius);
    glm::vec3 v1 = glm::vec3(_radius, 0, 0);
    glm::vec3 v2 = glm::vec3(0, _radius, 0);
    glm::vec3 v3 = glm::vec3(-_radius, 0, 0);
    glm::vec3 v4 = glm::vec3(0, -_radius, 0);
    glm::vec3 v5 = glm::vec3(0, 0, -_radius);

    // SubDivide positions by num subdivides input
    SubDivide(_vertices, _indices, v0, v1, v2, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v2, v3, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v3, v4, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v4, v1, _numSubdivides, _radius);

    SubDivide(_vertices, _indices, v5, v2, v1, _numSubdivides,_radius);
    SubDivide(_vertices, _indices, v5, v3, v2, _numSubdivides,_radius);
    SubDivide(_vertices, _indices, v5, v4, v3, _numSubdivides,_radius);
    SubDivide(_vertices, _indices, v5, v1, v4, _numSubdivides,_radius);
}

void Mesh::GenSphere(std::vector<GraphVertex>& _vertices, std::vector<Index>& _indices, const int _numSubdivides,
	float _radius)
{
    // initial vertex position to create a base mesh
    glm::vec3 v0 = glm::vec3(0, 0, _radius);
    glm::vec3 v1 = glm::vec3(_radius, 0, 0);
    glm::vec3 v2 = glm::vec3(0, _radius, 0);
    glm::vec3 v3 = glm::vec3(-_radius, 0, 0);
    glm::vec3 v4 = glm::vec3(0, -_radius, 0);
    glm::vec3 v5 = glm::vec3(0, 0, -_radius);

    // SubDivide positions by num subdivides input
    SubDivide(_vertices, _indices, v0, v1, v2, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v2, v3, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v3, v4, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v4, v1, _numSubdivides, _radius);

    SubDivide(_vertices, _indices, v5, v2, v1, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v5, v3, v2, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v5, v4, v3, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v5, v1, v4, _numSubdivides, _radius);
}

void Mesh::SubDivide(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC, const int _numSubdivides, float _radius)
{
    // if num subdivide min is reached pass positional values into make triangle
    // otherwise process, normalize and subdivide further.
    if (_numSubdivides > 0)
    {
        glm::vec3 v1 = _vecA + _vecB;
        v1 = normalize(v1) * _radius;

        glm::vec3 v2 = _vecA + _vecC;
        v2 = normalize(v2) * _radius;

        glm::vec3 v3 = _vecC + _vecB;
        v3 = normalize(v3) * _radius;

        SubDivide(_vertices, _indices, _vecA, v1, v2, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v1, _vecB, v3, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v2, v3, _vecC, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v1, v3, v2, _numSubdivides - 1, _radius);
    }
    else
    	MakeTriangle(_vertices, _indices, _vecA, _vecB, _vecC);
    
}

void Mesh::SubDivide(std::vector<GraphVertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA,
	const glm::vec3& _vecB, const glm::vec3& _vecC, const int _numSubdivides, float _radius)
{
    // if num subdivide min is reached pass positional values into make triangle
   // otherwise process, normalize and subdivide further.
    if (_numSubdivides > 0)
    {
        glm::vec3 v1 = _vecA + _vecB;
        v1 = normalize(v1) * _radius;

        glm::vec3 v2 = _vecA + _vecC;
        v2 = normalize(v2) * _radius;

        glm::vec3 v3 = _vecC + _vecB;
        v3 = normalize(v3) * _radius;

        SubDivide(_vertices, _indices, _vecA, v1, v2, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v1, _vecB, v3, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v2, v3, _vecC, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v1, v3, v2, _numSubdivides - 1, _radius);
    }
    else
        MakeTriangle(_vertices, _indices, _vecA, _vecB, _vecC);
}

void Mesh::MakeTriangle(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC)
{
    // Calculate normal for the triangle (assuming clockwise vertex order)
    glm::vec3 normal = glm::normalize(glm::cross(_vecB - _vecA, _vecC - _vecA));

    // Calculate texture coordinates based on vertex positions
    glm::vec2 texCoordA = CalculateTexCoord(_vecA);
    glm::vec2 texCoordB = CalculateTexCoord(_vecB);
    glm::vec2 texCoordC = CalculateTexCoord(_vecC);

    // Add vertices with calculated normal and texture coordinates
    _vertices.emplace_back(Vertex{ _vecA, normal, texCoordA });
    _vertices.emplace_back(Vertex{ _vecB, normal, texCoordB });
    _vertices.emplace_back(Vertex{ _vecC, normal, texCoordC });

    // Calculate indices for the newly added vertices
    unsigned int baseIndex = static_cast<unsigned int>(_vertices.size()) - 3;
    _indices.emplace_back(baseIndex);
    _indices.emplace_back(baseIndex + 1);
    _indices.emplace_back(baseIndex + 2);
}

void Mesh::MakeTriangle(std::vector<GraphVertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC)
{
    // Add vertices with calculated normal and texture coordinates
    _vertices.emplace_back(GraphVertex{ _vecA});
    _vertices.emplace_back(GraphVertex{ _vecB});
    _vertices.emplace_back(GraphVertex{ _vecC});

    // Calculate indices for the newly added vertices
    unsigned int baseIndex = static_cast<unsigned int>(_vertices.size()) - 3;
    _indices.emplace_back(baseIndex);
    _indices.emplace_back(baseIndex + 1);
    _indices.emplace_back(baseIndex + 2);
}

glm::vec2 Mesh::CalculateTexCoord(const glm::vec3& _vec)
{
    // Convert spherical coordinates to texture coordinates
    float u = 0.5f + (float) atan2(_vec.z, _vec.x) / (2 * M_PI);
    float v = 0.5f - (float) asin(_vec.y) / M_PI;
    return glm::vec2(u, v);
}

std::pair<glm::vec3, glm::vec3> Mesh::GetMeshMinMaxExtent(std::shared_ptr<Mesh> _mesh)
{
    // Calculate the bounding box (min and max extents) of the existing mesh
    std::vector<Vertex>& collisionMeshVertices = _mesh->GetVertices();
    glm::vec3 maxExtent = collisionMeshVertices[0].mPosition;
    glm::vec3 minExtent = collisionMeshVertices[0].mPosition;

    for (Vertex& vertex : collisionMeshVertices)
    {
        minExtent = glm::min(minExtent, vertex.mPosition);
        maxExtent = glm::max(maxExtent, vertex.mPosition);
    }

    // Slight offsett
    maxExtent += 0.001f;
    minExtent -= 0.001f;

	return std::make_pair(minExtent,maxExtent);
}

