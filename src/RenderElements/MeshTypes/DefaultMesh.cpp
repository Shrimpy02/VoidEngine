
#include <RenderElements/MeshTypes/DefaultMesh.h>
#include <RenderElements/VertexTypes/DefaultVertex.h>
#include <RenderElements/Material.h>
#include <RenderElements/Texture.h>
#include <Utilities/Logger.h>
#include <glad/glad.h>

/* ---------------------------------- Constructors ---------------------------------- */

DefaultMesh::DefaultMesh(const std::string& _name, std::vector<Vertex>&& _vertices, std::vector<Index>&& _indices, std::shared_ptr<Material> _mat)
    : Mesh(_name), mVertices(_vertices), mIndices(_indices), mMaterial(_mat)
{
    SetupMesh();

	if (!mMaterial)
        mMaterial = Material::Load(mName, { Texture::LoadWhiteTexture() }, {});
}

DefaultMesh::DefaultMesh(const std::string& _name, std::vector<Vertex>&& _vertices, std::vector<Index>&& _indices, std::shared_ptr<Material> _mat, float _URes, float _VRes, int _du, int _dv, std::vector<float> _uKnot, std::vector<float> _vKnot, std::vector<std::vector<glm::vec3>> _ctrlPoints)
    : Mesh(_name), mVertices(_vertices), mIndices(_indices), mMaterial(_mat), mUResolution(_URes), mVResolution(_VRes), mUDimension(_du), mVDimension(_dv), mUKnot(_uKnot), mVKnot(_vKnot), mControlPoints(_ctrlPoints)
{
    SetupMesh();

	if (!mMaterial)
        mMaterial = Material::Load(mName, { Texture::LoadWhiteTexture() }, {});
}

DefaultMesh::~DefaultMesh(){
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}

/* ---------------------------------- Global Functions ---------------------------------- */

void DefaultMesh::Draw(const std::shared_ptr<Shader> _shader) const
{
    if (!mVisible || !mMaterial) return;

    mMaterial->Bind(_shader);
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mIndices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

/* ----------------------------------- Local Functions ---------------------------------- */

void DefaultMesh::SetupMesh()
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Gen VBO and assign mesh vertices to the buffer.
    if (!mVertices.empty())
    {
        glGenBuffers(1, &mVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(mVertices.size() * sizeof(Vertex)), mVertices.data(), GL_STATIC_DRAW);
    }
    else {
        LOG_ERROR("MESH_SETTUP::NO_VERTICEIES_IN_MESH");
    }

    // Gen EBO and assign mesh indices to the buffer.
    if (!mIndices.empty())
    {
        glGenBuffers(1, &mEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(mIndices.size() * sizeof(Index)), mIndices.data(), GL_STATIC_DRAW);
    }
    else {
        LOG_ERROR("MESH_SETTUP::NO_INDICES_IN_MESH");
    }

    // setts up the vertex attributes
    Vertex::SetupAttributes();

    glBindVertexArray(0);
}


std::pair<glm::vec3, glm::vec3> DefaultMesh::GetMeshMinMaxExtent()
{
    // Calculate the bounding box (min and max extents) of the existing mesh
    glm::vec3 maxExtent = mVertices[0].mPosition;
    glm::vec3 minExtent = mVertices[0].mPosition;

    for (Vertex& vertex : mVertices)
    {
        minExtent = glm::min(minExtent, vertex.mPosition);
        maxExtent = glm::max(maxExtent, vertex.mPosition);
    }

    // Slight offset
    maxExtent += 0.001f;
    minExtent -= 0.001f;

    return std::make_pair(minExtent, maxExtent);
}

std::vector<Vertex>& DefaultMesh::GetVertices()
{
    return mVertices;
}

std::vector<Index>& DefaultMesh::GetIndices()
{
    return mIndices;
}
