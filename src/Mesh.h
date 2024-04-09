#pragma once

// Includes 
#include <Vertex.h>
#include <Material.h>
#include <Utilities/Types.h>

// Other includes
#include <vector>
#include <unordered_map>

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
    static std::unordered_map<std::string, Mesh*> mCache;

private:
    // ---------- Local Variables --------------

    // Mesh vertexes
    std::vector<Vertex> mVertices{};
    // Mesh indices
    std::vector<Index> mIndices{};
    // mesh material
    Material* mMaterial{ nullptr };
    // Mesh name
    const std::string mName;

    // gl buffer objects
    VAO mVAO{ 0U };
    VBO mVBO{ 0U };
    EBO mEBO{ 0U };

public:
    // ---------- Global functions --------------
    // Explicit constructor, calls settup mesh
    explicit Mesh(const std::string _name, std::vector<Vertex>&& _vertices, std::vector<Index>&& _indices, Material* _material);

    // De-constructor removes class references and gl-buffer objects.
	virtual ~Mesh();

    // Inherited from IRender, binds material and draws its self
    void Draw(const Shader* _shader) const;

    // Creates a default cube using a cube key
    static Mesh* CreateCube(Material* _material, std::string _customName = std::string());

    // Creates a default plane using a plane key
    static Mesh* CreatePlane(Material* _material, std::string _customName = std::string());

    // Creates a default pyramid using a pyramid key
    static Mesh* CreatePyramid(Material* _material, std::string _customName = std::string());

    // Creates a default sphere using a sphere key
    static Mesh* CreateSphere(Material* _material, const int _subdivides = 2, std::string _customName = std::string());

    // Loads mesh from cache by key
    static Mesh* Load(const std::string& _key);

    // Unloads item from cache by key
    static void Unload(const std::string& _key);

    // erases all elements of cache
    static void ClearCache();

    // Takes in vector addresses for vertices and indices and populates them with generated
	// geometry information based on the number of subdivides the sphere should have.
    static void GenSphere(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const int _numSubdivides = 2, float _radius = 0.5f );

private:
    // ---------- Local functions --------------

    // gl generation of buffers and attributes, called when class is constructed
    void SetupMesh();

    // Subdivides existing base geometry recursively by number subdivides
    // Passes vector address to Make Triangle function
    static void SubDivide(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC, const int _numSubdivides, float _radius);

    // Takes the 3 positions at a time and populates each vector with appropriate geometry.
    static void MakeTriangle(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC);

    // Helper function to calculate texture location on a sphere.
    static glm::vec2 CalculateTexCoord(const glm::vec3& _vec);

public:
    // ---------- Getters / setters / Adders --------------

      // Getters

    // Gets the material this mesh is currently using
    const Material* GetMaterial() const { return mMaterial; }

    // gets the name of this mesh
    const std::string* GetName() { return &mName; }

    // Gets this meshes vertices as address to vector
    std::vector<Vertex>& GetVertices() { return mVertices; }

    // Returns this meshes indices as vector of index struct
    std::vector<Index>& GetIndices() { return mIndices; }

    // Setters

    // Sets the material this mesh should use
    void SetMaterial(Material* _material) { mMaterial = _material; }
   
  
};




