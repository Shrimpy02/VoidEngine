#include "Mesh.h"

#include "Actor.h"
#include "Defines.h"

std::unordered_map<std::string, Mesh*> Mesh::mCache;

Mesh::Mesh(const std::string _name, std::vector<Vertex>&& _vertices, std::vector<Index>&& _indices, Material* material)
    : mName(_name), mVertices(std::move(_vertices)), mIndices(std::move(_indices)), mMaterial(material)
{
    SetupMesh();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}

void Mesh::Draw(const Shader* _shader) const
{
    if(mMaterial)
    {
        mMaterial->Bind(_shader);
    }

    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mIndices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh* Mesh::CreateCube(Material * _material, const std::string _key)
{
    auto it = mCache.find(_key);
    if (it != mCache.end())
    {
        return it->second;
    }

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

    mCache[_key] = new Mesh(_key, std::move(vertices), std::move(indices), _material);

    return mCache[_key];
}

Mesh* Mesh::CreatePlane(Material* _material)
{
    const std::string planeKey = "Plane";

    auto it = mCache.find(planeKey);
    if (it != mCache.end())
    {
        return it->second;
    }

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

    mCache[planeKey] = new Mesh(planeKey, std::move(vertices), std::move(indices), _material);

    return mCache[planeKey];
}

Mesh* Mesh::CreatePyramid(Material* _material)
{
    const std::string PyramidKey = "Pyramid";

    auto it = mCache.find(PyramidKey);
    if (it != mCache.end())
    {
        return it->second;
    }

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

    mCache[PyramidKey] = new Mesh(PyramidKey, std::move(vertices), std::move(indices), _material);

    return mCache[PyramidKey];
}

Mesh* Mesh::Load(const std::string& _path)
{
    auto it = mCache.find(_path);
    if (it != mCache.end())
    {
        return it->second;
    }
    return nullptr;
}

void Mesh::Unload(const std::string& _path)
{
    auto it = mCache.find(_path);
    if (it != mCache.end())
    {
        delete it->second;
        mCache.erase(it);
    }
}

void Mesh::ClearCache()
{
    for (auto it : mCache)
    {
        delete it.second;
    }
    mCache.clear();
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(Index), mIndices.data(), GL_STATIC_DRAW);

    Vertex::SetupAttributes();

    glBindVertexArray(0);
}

// ---------------------------------------------------------------
// --------------------- Mesh Actor ------------------------------
// ---------------------------------------------------------------

MeshActor::MeshActor(const std::string& _name, Mesh* _mesh)
    :Actor(_name), mMesh(_mesh)
{
    // Add debug mesh to show collision
	auto Tex = Texture::Load(SOURCE_DIRECTORY("assets/Textures/white.jpg"));
	auto mat = Material::Load("Collision-Debug", { Tex }, {});
	mCollisionMeshVisualization = IBounded::CreateCollisionCube(mat, mMesh->GetVetices(), "CollisionCube");
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

AABB MeshActor::GenAABB() 
{
    if (mCollisionMeshVisualization)
    {
        glm::vec3 minExtent = GetMinExtent();
        glm::vec3 maxExtent = GetMaxExtent();

        // Calculate scaled min and max extents
        glm::vec3 scaledMinExtent = minExtent * GetScale(TransformSpace::Global);
        glm::vec3 scaledMaxExtent = maxExtent * GetScale(TransformSpace::Global);

        // Apply object's position
        glm::vec3 position = GetPosition(TransformSpace::Global);

        // Calculate center of the AABB
        glm::vec3 center = (scaledMinExtent + scaledMaxExtent) * 0.5f + position;

        // Calculate extent of the AABB
        glm::vec3 extent = (scaledMaxExtent - scaledMinExtent) * 0.5f;

        // Construct and return the AABB
        return AABB(center, extent);
    } else {
        return AABB(GetPosition(TransformSpace::Global), GetScale(TransformSpace::Global) * 0.5f);
    }
	
}

BoundingSphere MeshActor::GenBoundingSphere()
{
    return BoundingSphere(GetPosition(TransformSpace::Global), GetScale(TransformSpace::Global) * 0.5f);
}

CollisionProperties* MeshActor::GetCollisionProperties()
{
    return &mCollisionProperties; 
}
