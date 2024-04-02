#pragma once
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AssimpLoader
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

    static std::string msBasePath;
    static long long msNameIndex;
    static long long msFlags;
  
public:
    // ---------- Global functions --------------
    static void Load(const std::string& _path, class Actor* _staticMeshActor, unsigned _flags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords);

private:
    // ---------- Local functions --------------
    static void ProcessLights(const struct aiScene* _scene, class Actor* _parentActor);
    static void ProcessNode(const struct aiScene* _scene, struct aiNode* _node, class Actor* parentActor);
    static class Mesh* ProcessMesh(struct aiMesh* _mesh);
    static class Material* ProcessMaterial(struct aiMaterial* _material);
    static struct AABBActor* ProcessCollisionAABB(struct aiMesh* _mesh, std::string _name);

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters

};

