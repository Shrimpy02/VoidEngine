
#include <RenderElements/MeshTypes/DebugMesh.h>
#include <RenderElements/VertexTypes/DebugVertex.h>
#include <Utilities/Logger.h>
#include <glad/glad.h>

/* ---------------------------------- Constructors ---------------------------------- */

DebugMesh::DebugMesh(const std::string& _name, std::vector<DebugVertex>&& _vertices, std::vector<Index>&& _indices)
    : Mesh(_name), mVertices(_vertices), mIndices(_indices)
{
    SetupMesh();
}

DebugMesh::~DebugMesh()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}

/* ---------------------------------- Global Functions ---------------------------------- */

void DebugMesh::Draw(const std::shared_ptr<Shader> _shader) const
{
    if (!mVisible) return;

    glLineWidth(8.0f);
    glBindVertexArray(mVAO);
    glDrawElements(GL_LINE_STRIP, static_cast<GLsizei>(mIndices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glLineWidth(1.0f);
}

/* ----------------------------------- Local Functions ---------------------------------- */

void DebugMesh::SetupMesh()
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Gen VBO and assign mesh vertices to the buffer.
    if (!mVertices.empty())
    {
        glGenBuffers(1, &mVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(mVertices.size() * sizeof(DebugVertex)), mVertices.data(), GL_STATIC_DRAW);
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
    DebugVertex::SetupAttributes();

    glBindVertexArray(0);
}

std::pair<glm::vec3, glm::vec3> DebugMesh::GetMeshMinMaxExtent()
{
    // Calculate the bounding box (min and max extents) of the existing mesh
    glm::vec3 maxExtent = mVertices[0].mPosition;
    glm::vec3 minExtent = mVertices[0].mPosition;

    for (DebugVertex& vertex : mVertices)
    {
        minExtent = glm::min(minExtent, vertex.mPosition);
        maxExtent = glm::max(maxExtent, vertex.mPosition);
    }

    // Slight offset
    maxExtent += 0.001f;
    minExtent -= 0.001f;

    return std::make_pair(minExtent, maxExtent);
}

std::vector<DebugVertex>& DebugMesh::GetVertices()
{
    return mVertices;
}

std::vector<Index>& DebugMesh::GetIndices()
{
    return mIndices;
}
