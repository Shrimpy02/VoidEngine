#pragma once

// Includes 
#include <Utilities/Types.h>

// Other includes
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <string>

// Forward Declarations
struct Vertex;
struct GraphVertex;
struct DebugVertex;
class Material;
class Shader;

// TODO: make new inherit classes for each mesh type.

/**
 * @class Mesh
 * @brief Contains all logic for creating and maintaining geometry in openGL.
 * Uses a static cache to remember geometry optimizing loading times.
 */
class Mesh
{
public:
    // ---------- Global Variables --------------

    // Static cache of meshes
    static std::unordered_map<std::string, std::shared_ptr<Mesh>> mCache;

private:
    // ---------- Local Variables --------------

    // Mesh vertexes
    std::vector<Vertex> mVertices;
    // Mesh vertexes graph type
    std::vector<GraphVertex> mGraphVertices;
    // Mesh vertexes debug type
    std::vector<DebugVertex> mDebugVertices;

    // Mesh indices
    std::vector<Index> mIndices;
    // mesh material
    std::shared_ptr<Material> mMaterial{ nullptr };
    // Mesh name
    const std::string mName;

    // Reguarding BSpline Surface mesh type
    float mUResolution{2};
    float mVResolution{ 2 };
    int mUDimension{ 2 };
    int mVDimension{ 2 };
    std::vector<float> mUKnot;
    std::vector<float> mVKnot;
    std::vector<std::vector<glm::vec3>> mControlPoints;
    bool mIsBSpline = false;
    bool mIsPointCloud = false;


    bool mVisible = true;

    // gl buffer objects
    VAO mVAO{ 0U };
    VBO mVBO{ 0U };
    EBO mEBO{ 0U };

public:
    // ---------- Global functions --------------
    // Explicit constructor, calls settup mesh
    explicit Mesh(const std::string _name, std::vector<Vertex>&& _vertices, std::vector<Index>&& _indices, std::shared_ptr<Material> _material);

    explicit Mesh(const std::string _name, std::vector<Vertex>&& _vertices, std::vector<Index>&& _indices, std::shared_ptr<Material> _material,float _uRes, float _vRes, int _uDim, int _vDim, const std::vector<float>& _uKnot, const std::vector<float>& _vKnot, const std::vector<std::vector<glm::vec3>>& _controlPoints);

    explicit Mesh(const std::string _name, std::vector<GraphVertex>&& _vertices, std::vector<Index>&& _indices);
                                         
    explicit Mesh(const std::string _name, std::vector<DebugVertex>&& _vertices, std::vector<Index>&& _indices);

    // De-constructor removes class references and gl-buffer objects.
	virtual ~Mesh();
    // Inherited from IRender, binds material and draws its self
    void Draw(const std::shared_ptr<Shader> _shader) const;

    void DrawDebugLines(const std::shared_ptr<Shader> _shader) const;

    // Creates a default cube using a cube key
    static  std::shared_ptr<Mesh> CreateCube(std::shared_ptr<Material> _material, const bool _instance = false, std::string _customName = std::string());

    // Creates a default cube using a cube key
    static  std::shared_ptr<Mesh> CreateCubeByExtent(std::shared_ptr<Mesh> _extentMesh, std::shared_ptr<Material> _material, std::string _customName = std::string());

    // Creates a default plane using a plane key
    static std::shared_ptr<Mesh> CreatePlane(std::shared_ptr<Material> _material, const bool _instance = false, std::string _customName = std::string());

    // Creates a default pyramid using a pyramid key
    static std::shared_ptr<Mesh> CreatePyramid(std::shared_ptr<Material> _material, const bool _instance = false, std::string _customName = std::string());

    // Creates a default cube using a cube key
    static std::shared_ptr<Mesh> CreateSphereByExtent(std::shared_ptr<Mesh> _extentMesh, std::shared_ptr<Material> _material, std::string _customName = std::string());

    // Creates a default sphere using a sphere key
    static std::shared_ptr<Mesh> CreateSphere(std::shared_ptr<Material> _material, const int _subdivides = 2, const bool _instance = false, std::string _customName = std::string());

    static std::shared_ptr<Mesh> CreateBSplineSurface(std::shared_ptr<Material> _material, int _UResolution, int _VResolution,  int _du, int _dv, const std::vector<float>& _uKnot, const std::vector<float>& _vKnot, const std::vector<std::vector<glm::vec3>>& _controlPoints, std::string _customName = std::string());

    static std::shared_ptr<Mesh> CreatePointCloudFromLASFileSurface(const char* _fileDirectory);

    static std::shared_ptr<Mesh> CreateGraphSphere(const int _subdivides = 1, const bool _instance = true, std::string _customName = std::string());

    static std::shared_ptr<Mesh> CreateDebugLine(std::vector<glm::vec3> _points);

	static std::shared_ptr<Mesh> CreateDebugLine(std::shared_ptr<Mesh> _mesh);

    static std::shared_ptr<Mesh> CreateDebugLine(std::pair<glm::vec3, glm::vec3> _extents);

    // Loads mesh from cache by key
    static std::shared_ptr<Mesh> Load(const std::string& _key);

    // Unloads item from cache by key
    static void Unload(const std::string& _key);

    // erases all elements of cache
    static void ClearCache();

    // Takes in vector addresses for vertices and indices and populates them with generated
	// geometry information based on the number of subdivides the sphere should have.
    static void GenSphere(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const int _numSubdivides = 2, float _radius = 0.5f );

    static void GenSphere(std::vector<GraphVertex>& _vertices, std::vector<Index>& _indices, const int _numSubdivides = 1, float _radius = 0.5f);


private:
    // ---------- Local functions --------------

    // gl generation of buffers and attributes, called when class is constructed
    void SetupMesh();

    void SetupGraphMesh();

    void SetupDebugMesh();

    // Subdivides existing base geometry recursively by number subdivides
    // Passes vector address to Make Triangle function
    static void SubDivide(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC, const int _numSubdivides, float _radius);

    static void SubDivide(std::vector<GraphVertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC, const int _numSubdivides, float _radius);

	// Takes the 3 positions at a time and populates each vector with appropriate geometry.
    static void MakeTriangle(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC);

    static void MakeTriangle(std::vector<GraphVertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC);

    // Helper function to calculate texture location on a sphere.
    static glm::vec2 CalculateTexCoord(const glm::vec3& _vec);

public:
    // ---------- Getters / setters / Adders --------------

      // Getters
    // Returns the min max extent of the un transformed mesh
    static std::pair<glm::vec3, glm::vec3> GetMeshMinMaxExtent(std::shared_ptr<Mesh> _mesh);

    // Gets the material this mesh is currently using
    const std::shared_ptr<Material> GetMaterial() const { return mMaterial; }

    // gets the name of this mesh
    const std::string GetName() { return mName; }

    // Gets this meshes vertices as address to vector
    std::vector<Vertex>& GetVertices() { return mVertices; }

    // Returns this meshes indices as vector of index struct
    std::vector<Index>& GetIndices() { return mIndices; }

    // Returns this meshes visibility as a bool
    bool GetIsVisible() { return mVisible; }

    float* GetBSplineUResolution() { return &mUResolution; }
    float* GetBSplineVResolution() { return &mVResolution; }
    int* GetBSplineUDimension() { return &mUDimension; }
    int* GetBSplineVDimension() { return &mVDimension; }
    std::vector<float> GetBSplineUKnot() { return mUKnot; }
    std::vector<float> GetBSplineVKnot() { return mVKnot; }
    std::vector<std::vector<glm::vec3>> GetBSplineControlPoints() { return mControlPoints; }
    void SetIsBSpline(bool _b) { mIsBSpline = _b; }
    bool IsBSpline() { return mIsBSpline; }
    void SetIsPointCloud(bool _b) { mIsPointCloud = _b; }
    bool IsPointCloud() { return mIsPointCloud; }
    // Setters

    // Sets the material this mesh should use
    void SetMaterial(std::shared_ptr<Material> _material) { mMaterial = _material; }
    void SetIsVisible(bool _b) { mVisible = _b; }

};




