#pragma once

// Includes 
#include <Utilities/Types.h>

// Other includes
#include <any>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <string>

// Forward Declarations
class Shader;
class Material;
struct Vertex;
struct GraphVertex;

class DefaultMesh;

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

    const std::string mName;
    bool mVisible = true;

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------
    // Explicit constructor, calls settup mesh

    explicit Mesh(const std::string _name);
    
    // De-constructor removes class references and gl-buffer objects.
    virtual ~Mesh() = default;

    // Inherited from IRender, binds material and draws its self
    virtual void Draw(const std::shared_ptr<Shader> _shader) const = 0;

    // Creates a default cube using a cube key
    static std::shared_ptr<Mesh> CreateCube(std::shared_ptr<Material> _material, const bool _instance = false, std::string _customName = std::string());

    // Creates a default cube using a cube key
    static std::shared_ptr<Mesh> CreateCubeByExtent(std::shared_ptr<Mesh> _extentMesh, std::shared_ptr<Material> _material, std::string _customName = std::string());

    // Creates a default plane using a plane key
    static std::shared_ptr<Mesh> CreatePlane(std::shared_ptr<Material> _material, const bool _instance = false, std::string _customName = std::string());

    // Creates a default pyramid using a pyramid key
    static std::shared_ptr<Mesh> CreatePyramid(std::shared_ptr<Material> _material, const bool _instance = false, std::string _customName = std::string());

    // Creates a default cube using a cube key
    static std::shared_ptr<Mesh> CreateSphereByExtent(std::shared_ptr<Mesh> _extentMesh, std::shared_ptr<Material> _material, std::string _customName = std::string());

    // Creates a default sphere using a sphere key
    static std::shared_ptr<Mesh> CreateSphere(std::shared_ptr<Material> _material, const int _subdivides = 2, const bool _instance = false, std::string _customName = std::string());

    static std::shared_ptr<Mesh> CreateBSplineSurface(std::shared_ptr<Material> _material, int _UResolution, int _VResolution,  int _du, int _dv, const std::vector<float>& _uKnot, const std::vector<float>& _vKnot, const std::vector<std::vector<glm::vec3>>& _controlPoints, std::string _customName = std::string());

    static std::shared_ptr<Mesh> CreatePointCloudFromLASFileSurface(const char* _fileDirectory, float _scaleFactor);

    static std::shared_ptr<Mesh> CreateGraphSphere(const int _subdivides = 1, const bool _instance = true, std::string _customName = std::string());

    static std::shared_ptr<Mesh> CreateDebugLine(std::vector<glm::vec3> _points);

    // Template static function for creating debug lines that match an existing meshes vertices
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

    // Gets the material this mesh is currently using
    virtual const std::shared_ptr<Material> GetMaterial() const = 0;

    //template <typename T, typename K>
    //static std::vector<K>& GetVertices(std::shared_ptr<T> _mesh);

    virtual std::vector<Index>& GetIndices() = 0;

    //// Template function, gets the min extent, as first and max extent as second from input mesh.
	//// Note the input mesh must have mPosition quality in their vertex type.
    //template <typename T>
    //static std::pair<glm::vec3, glm::vec3> GetMeshMinMaxExtent(std::shared_ptr<T> _mesh);


    virtual std::pair<glm::vec3, glm::vec3> GetMeshMinMaxExtent() = 0;

    // gets the name of this mesh
    const std::string GetName() { return mName; }

    // Returns this meshes visibility as a bool
    bool GetIsVisible() { return mVisible; }

	// Setters

      // Sets the material this mesh is currently using
    virtual void SetMaterial(std::shared_ptr<Material> _mat) = 0;

    void SetIsVisible(bool _b) { mVisible = _b; }

    // Checkers


};


