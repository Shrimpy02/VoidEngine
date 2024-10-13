#pragma once

// Includes 
#include <RenderElements/Mesh.h>

// Other includes

// Forward Declarations

// Enums

/**
 * @class DefaultMesh
 * @brief Contains all logic for creating and maintaining geometry in openGL.
 * Uses a static cache to remember geometry optimizing loading times.
 */
class DefaultMesh : public Mesh
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

    std::vector<Vertex> mVertices;
    std::vector<Index> mIndices;
    std::shared_ptr<Material> mMaterial{ nullptr };

    // Regarding BSpline Surface mesh type
    float mUResolution{ 2 };
    float mVResolution{ 2 };
    int mUDimension{ 2 };
    int mVDimension{ 2 };
    std::vector<float> mUKnot;
    std::vector<float> mVKnot;
    std::vector<std::vector<glm::vec3>> mControlPoints;
    bool mIsBSpline = false;

    // gl buffer objects
    VAO mVAO{ 0U };
    VBO mVBO{ 0U };
    EBO mEBO{ 0U };

public:
    // ---------- Global functions --------------
    // Explicit constructor, calls settup mesh
    explicit DefaultMesh(const std::string& _name, std::vector<Vertex>&& _vertices, std::vector<Index>&& _indices, std::shared_ptr<Material> _mat);

    explicit DefaultMesh(const std::string& _name, std::vector<Vertex>&& _vertices, std::vector<Index>&& _indices, std::shared_ptr<Material> _mat, float _URes, float _VRes, int _du, int _dv, std::vector<float> _uKnot, std::vector<float> _vKnot, std::vector<std::vector<glm::vec3>> _ctrlPoints);


    // De-constructor removes class references and gl-buffer objects.
    virtual ~DefaultMesh() override;

    // Inherited from IRender, binds material and draws its self
    void Draw(const std::shared_ptr<Shader> _shader) const override;


private:
    // ---------- Local functions --------------

    void SetupMesh();

public:
    // ---------- Getters / setters / Adders / Checkers --------------

    std::pair<glm::vec3, glm::vec3> GetMeshMinMaxExtent() override;

	// Gets this meshes vertices as address to vector
    std::vector<Vertex>& GetVertices();
    // Returns this meshes indices as vector of index struct
    std::vector<Index>& GetIndices() override;

    // Gets the material this mesh is currently using
    const std::shared_ptr<Material> GetMaterial() const override { return mMaterial; }

    float* GetBSplineUResolution() { return &mUResolution; }

    float* GetBSplineVResolution() { return &mVResolution; }

    int* GetBSplineUDimension() { return &mUDimension; }

    int* GetBSplineVDimension() { return &mVDimension; }

    std::vector<float> GetBSplineUKnot() { return mUKnot; }

    std::vector<float> GetBSplineVKnot() { return mVKnot; }

    std::vector<std::vector<glm::vec3>> GetBSplineControlPoints() { return mControlPoints; }


    // Sets the material this mesh is currently using
    void SetMaterial(std::shared_ptr<Material> _mat) override { mMaterial = _mat; }

    void SetIsBSpline(bool _b) { mIsBSpline = _b; }

    bool IsBSpline() { return mIsBSpline; }

};


