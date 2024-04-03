// Includes
#include <corecrt_math_defines.h>
#include <Mesh.h>
#include <Logger.h>

// static cache of meshes
std::unordered_map<std::string, Mesh*> Mesh::mCache;

Mesh::Mesh(const std::string _name, std::vector<Vertex>&& _vertices, std::vector<Index>&& _indices, Material* material)
    : mName(_name), mVertices(std::move(_vertices)), mIndices(std::move(_indices)), mMaterial(material)
{
    // generates gl specific buffers for mesh init.
    SetupMesh();
}

Mesh::~Mesh()
{
    // deletes gl mesh buffers
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}

void Mesh::Draw(const Shader* _shader) const
{
    // bind the material the shader for this object if it exists
    if(mMaterial)
        mMaterial->Bind(_shader);
    
    // binds VAO and draws all geometry by given indices and vertices
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mIndices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh* Mesh::CreateCube(Material * _material)
{
    // Create default plane key
    std::string cubeKey = "DefaultCube";

    // Checks if default cube exists in cache,if it does use that
    // rather than calculating an entire new cube to save loading time.
    auto it = mCache.find(cubeKey);
    if (it != mCache.end())
    {
        LOG("%s Loaded", cubeKey.c_str());
        return it->second;
    }

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

    // Create mesh moveing the vertices and indices into new object along with input material and add it to cache
    mCache[cubeKey] = new Mesh(cubeKey, std::move(vertices), std::move(indices), _material);

    // return new default cube
    LOG("%s created", cubeKey.c_str());
	return mCache[cubeKey];
}

Mesh* Mesh::CreatePlane(Material* _material)
{
    // Create default plane key
    std::string planeKey = "DefaultPlane";

    // Checks if default plane exists in cache,if it does use that
	// rather than calculating an entire new cube to save loading time.
    auto it = mCache.find(planeKey);
    if (it != mCache.end())
    {
        LOG("%s Loaded", planeKey.c_str());
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
    mCache[planeKey] = new Mesh(planeKey, std::move(vertices), std::move(indices), _material);

    // return new default plane
    LOG("%s Created", planeKey.c_str());
    return mCache[planeKey];
}

Mesh* Mesh::CreatePyramid(Material* _material)
{
    // Create default pyramid key
    std::string pyramidKey = "DefaultPyramid";

    // Checks if default pyramid exists in cache,if it does use that
	// rather than calculating an entire new cube to save loading time.
    auto it = mCache.find(pyramidKey);
    if (it != mCache.end())
    {
        LOG("%s Loaded", pyramidKey.c_str());
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
    mCache[pyramidKey] = new Mesh(pyramidKey, std::move(vertices), std::move(indices), _material);

    // return new default pyramid
    LOG("%s Created", pyramidKey.c_str());
    return mCache[pyramidKey];
}

Mesh* Mesh::CreateSphere(Material* _material, const int _subdivides)
{
    // Create default sphere key based on num subdivides
    std::string sphereKey = "DefaultSphere" + std::to_string(_subdivides);

    // Checks if default sphere exists in cache,if it does use that
     // rather than calculating an entire new cube to save loading time.
    auto it = mCache.find(sphereKey);
    if (it != mCache.end())
    {
        LOG("%s Loaded", sphereKey.c_str());
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
    mCache[sphereKey] = new Mesh(sphereKey, std::move(vertices), std::move(indices), _material);

    // return new default sphere
    LOG("%s created", sphereKey.c_str());
    return mCache[sphereKey];
}

Mesh* Mesh::Load(const std::string& _key)
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
        delete it->second;
        mCache.erase(it);
    }
    LOG_WARNING("%s: Not found in cache for unloading", _key.c_str());
}

void Mesh::ClearCache()
{
    // deletes all elements of the cache
    for (auto it : mCache)
        delete it.second;
    
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

void Mesh::GenSphere(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const int _numSubdivides)
{
    // initial vertex position to create a base mesh
    glm::vec3 v0 = glm::vec3(0, 0, 1);
    glm::vec3 v1 = glm::vec3(1, 0, 0);
    glm::vec3 v2 = glm::vec3(0, 1, 0);
    glm::vec3 v3 = glm::vec3(-1, 0, 0);
    glm::vec3 v4 = glm::vec3(0, -1, 0);
    glm::vec3 v5 = glm::vec3(0, 0, -1);

    // SubDivide positions by num subdivides input
    SubDivide(_vertices, _indices, v0, v1, v2, _numSubdivides);
    SubDivide(_vertices, _indices, v0, v2, v3, _numSubdivides);
    SubDivide(_vertices, _indices, v0, v3, v4, _numSubdivides);
    SubDivide(_vertices, _indices, v0, v4, v1, _numSubdivides);

    SubDivide(_vertices, _indices, v5, v2, v1, _numSubdivides);
    SubDivide(_vertices, _indices, v5, v3, v2, _numSubdivides);
    SubDivide(_vertices, _indices, v5, v4, v3, _numSubdivides);
    SubDivide(_vertices, _indices, v5, v1, v4, _numSubdivides);

}

void Mesh::SubDivide(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC, const int _numSubdivides)
{
    // if num subdivide min is reached pass positional values into make triangle
    // otherwise process, normalize and subdivide further.
    if (_numSubdivides > 0)
    {
        glm::vec3 v1 = _vecA + _vecB;
        v1 = normalize(v1);

        glm::vec3 v2 = _vecA + _vecC;
        v2 = normalize(v2);

        glm::vec3 v3 = _vecC + _vecB;
        v3 = normalize(v3);

        SubDivide(_vertices, _indices, _vecA, v1, v2, _numSubdivides - 1);
        SubDivide(_vertices, _indices, v1, _vecB, v3, _numSubdivides - 1);
        SubDivide(_vertices, _indices, v2, v3, _vecC, _numSubdivides - 1);
        SubDivide(_vertices, _indices, v1, v3, v2, _numSubdivides - 1);
    }
    else
    	MakeTriangle(_vertices, _indices, _vecA, _vecB, _vecC);
    
}

void Mesh::MakeTriangle(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC)
{
    // Scale down the vertex positions by 0.5
    glm::vec3 scaledVecA = _vecA * 0.5f;
    glm::vec3 scaledVecB = _vecB * 0.5f;
    glm::vec3 scaledVecC = _vecC * 0.5f;

    // Calculate normal for the triangle (assuming clockwise vertex order)
    glm::vec3 normal = glm::normalize(glm::cross(scaledVecB - scaledVecA, scaledVecC - scaledVecA));

    // Calculate texture coordinates based on vertex positions
    glm::vec2 texCoordA = CalculateTexCoord(scaledVecA);
    glm::vec2 texCoordB = CalculateTexCoord(scaledVecB);
    glm::vec2 texCoordC = CalculateTexCoord(scaledVecC);

    // Add vertices with calculated normal and texture coordinates
    _vertices.emplace_back(Vertex{ scaledVecA, normal, texCoordA });
    _vertices.emplace_back(Vertex{ scaledVecB, normal, texCoordB });
    _vertices.emplace_back(Vertex{ scaledVecC, normal, texCoordC });

    // Calculate indices for the newly added vertices
    unsigned int baseIndex = static_cast<unsigned int>(_vertices.size()) - 3;
    _indices.emplace_back(baseIndex);
    _indices.emplace_back(baseIndex + 1);
    _indices.emplace_back(baseIndex + 2);
}

glm::vec2 Mesh::CalculateTexCoord(const glm::vec3& _vec)
{
    // Convert spherical coordinates to texture coordinates
    float u = 0.5f + atan2(_vec.z, _vec.x) / (2 * M_PI);
    float v = 0.5f - asin(_vec.y) / M_PI;
    return glm::vec2(u, v);
}

// ---------------------------------------------------------------
// --------------------- Mesh Actor ------------------------------
// ---------------------------------------------------------------

MeshActor::MeshActor(const std::string& _name, Mesh* _mesh)
    :Actor(_name), mMesh(_mesh)
{
    //// Add debug mesh to show collision
	//auto Tex = Texture::Load(SOURCE_DIRECTORY("assets/Textures/white.jpg"));
	//auto mat = Material::Load("Collision-Debug", { Tex }, {});
	//mCollisionMeshVisualization = IBounded::CreateCollisionCube(mat, mMesh->GetVetices(), "CollisionCube");
}

void MeshActor::Draw(const Shader* _shader) const
{
	if (!mMesh) return; 

	mMesh->Draw(_shader);

    if (mCollisionMeshVisualization && mShouldDrawCollisionDebugMesh){
        // draws debug collision mesh if exists and has permission
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mCollisionMeshVisualization->Draw(_shader);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

AABB MeshActor::GetAABB() const
{
    return AABB(GetPosition(TransformSpace::Global), GetScale(TransformSpace::Global) * 0.5f);
}

CollisionProperties MeshActor::GetCollisionProperties() const
{
    return mCollisionProperties;
}

//AABB MeshActor::GenAABB() 
//{
//    if (mCollisionMeshVisualization)
//    {
//        glm::vec3 minExtent = GetMinExtent();
//        glm::vec3 maxExtent = GetMaxExtent();
//
//        // Calculate scaled min and max extents
//        glm::vec3 scaledMinExtent = minExtent * GetScale(TransformSpace::Global);
//        glm::vec3 scaledMaxExtent = maxExtent * GetScale(TransformSpace::Global);
//
//        // Apply object's position
//        glm::vec3 position = GetPosition(TransformSpace::Global);
//
//        // Calculate center of the AABB
//        glm::vec3 center = (scaledMinExtent + scaledMaxExtent) * 0.5f + position;
//
//        // Calculate extent of the AABB
//        glm::vec3 extent = (scaledMaxExtent - scaledMinExtent) * 0.5f;
//
//        // Construct and return the AABB
//        return AABB(center, extent);
//    } else {
//        return AABB(GetPosition(TransformSpace::Global), GetScale(TransformSpace::Global) * 0.5f);
//    }
//	
//}

//BoundingSphere MeshActor::GenBoundingSphere()
//{
//    return BoundingSphere(GetPosition(TransformSpace::Global), GetScale(TransformSpace::Global) * 0.5f);
//}
//
//CollisionProperties* MeshActor::GetCollisionProperties()
//{
//    return &mCollisionProperties; 
//}
