#pragma once
#include <Renderer.h>
#include <Material.h>
#include <Shader.h>
#include <Vertex.h>
#include <vector>
#include <unordered_map>
#include <Actor.h>
#include <Types.h>

#include "AABB.h"
#include "Collision.h"
//#include <AABB.h>

class Mesh
{
public:
    // ---------- Global Variables --------------
    static std::unordered_map<std::string, Mesh*> mCache;

private:
    // ---------- Local Variables --------------

    std::vector<Vertex> mVertices{};
    std::vector<Index> mIndices{};
    Material* mMaterial{ nullptr };

    const std::string mName;

    VAO mVAO{ 0U };
    VBO mVBO{ 0U };
    EBO mEBO{ 0U };

public:
    // ---------- Global functions --------------

    explicit Mesh(const std::string _name, std::vector<Vertex>&& _vertices, std::vector<Index>&& _indices, Material* _material);
    virtual ~Mesh();

    // Assumes the shader is bound
    void Draw(const Shader* _shader) const;

    static Mesh* CreateCube(Material* _material, const std::string _key = "Cube");
    static Mesh* CreatePlane(Material* _material);
    static Mesh* CreatePyramid(Material* _material);
    static Mesh* Load(const std::string& _path);
    static void Unload(const std::string& _path);
    static void ClearCache();

private:
    // ---------- Local functions --------------

    void SetupMesh();


public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    void SetMaterial(Material* material) { mMaterial = material; }
   
    // Getters
    const Material* GetMaterial() const { return mMaterial; }

    const std::string* GetName() { return &mName; }
    std::vector<Vertex>& GetVetices() { return mVertices; }
};

class MeshActor : public Actor, public IRender, public IBounded
{
public:
    // ---------- Global Variables --------------

    Mesh* mMesh{ nullptr };
    Mesh* mCollisionMeshVisualization{ nullptr };
    //MeshActor* mCollisionMeshVisualization{ nullptr };
	CollisionProperties mCollisionProperties{ CollisionType::STATIC, CollisionResponse::BLOCK };

    bool mShouldDrawCollisionDebugMesh = false;

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------









    MeshActor(const std::string& _name, Mesh* _mesh);
    virtual void Draw(const class Shader* _shader = nullptr) const override;
    virtual AABB GetAABB() const override;
    virtual CollisionProperties GetCollisionProperties() const override;

private:
    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters
    void SetDrawDebugCollisionMesh(bool _state) { mShouldDrawCollisionDebugMesh = _state; }
    // Getters

};

