#pragma once

// Includes 
#include <RenderElements/Mesh.h>

// Other includes

// Forward Declarations
struct PointCloudVertex;

// Enums

/**
 * @class PointCloudMesh
 * @brief Contains all logic for creating and maintaining geometry in openGL.
 * Uses a static cache to remember geometry optimizing loading times.
 */
class PointCloudMesh : public Mesh
{
public:
    // ---------- Global Variables --------------

    std::vector<PointCloudVertex> mVertices;

private:
    // ---------- Local Variables --------------


    std::vector<Index> mIndices;

    // gl buffer objects
    VAO mVAO{ 0U };
    VBO mVBO{ 0U };
    EBO mEBO{ 0U };

public:
    // ---------- Global functions --------------
    // Explicit constructor, calls settup mesh
    explicit PointCloudMesh(const std::string& _name, std::vector<PointCloudVertex>&& _vertices, std::vector<Index>&& _indices);

    // De-constructor removes class references and gl-buffer objects.
    virtual ~PointCloudMesh() override;

    // Inherited from IRender, binds material and draws its self
    void Draw(const std::shared_ptr<Shader> _shader) const override;

    void UpdateMesh();

private:
    // ---------- Local functions --------------

    void SetupMesh();
    
public:
    // ---------- Getters / setters / Adders --------------

    std::pair<glm::vec3, glm::vec3> GetMeshMinMaxExtent() override;

	// Gets the material this mesh is currently using
    const std::shared_ptr<Material> GetMaterial() const override { return nullptr; }

	// Gets this meshes vertices as address to vector
    std::vector<PointCloudVertex>& GetVertices();

	// Returns this meshes indices as vector of index struct
    std::vector<Index>& GetIndices() override;

    // Sets the material this mesh is currently using
    void SetMaterial(std::shared_ptr<Material> _mat) override { };


	};



