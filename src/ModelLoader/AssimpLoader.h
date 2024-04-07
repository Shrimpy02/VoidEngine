#pragma once

// Includes 
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/**
 * @class AssimpLoader
 * @brief The AssimpLoader class handles all interaction with the Assimp library for this application.
 * It contains the appropriate processing of nodes and converting of Assimp objects into seb engine objects. 
 */
class AssimpLoader
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

    // Static base path to loaded file.
    static std::string msBasePath;

    // Static naming index for given names to multiple created scene objects.
    static long long msNameIndex;

    // Static flags, for assimp import options.
	static long long msFlags;
  
public:
    // ---------- Global functions --------------

    // Default constructor/de-constructor as the class only holds static functionality  

    // This is the only global function as it loads the FBX the file path refers to as a child of input static mesh with the given flags(default)
	static void Load(const std::string& _path, class Actor* _staticMeshActor, unsigned _flags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords);

private:
    // ---------- Local functions --------------

    // Processes all lights in the FBX into point or directional light actors for the scene 
    static void ProcessLights(const struct aiScene* _scene, class Actor* _parentActor);

    // Processes all scene nodes and creates the appropriate actor for the node type.
    static void ProcessNode(const struct aiScene* _scene, struct aiNode* _node, class Actor* parentActor);

    // Converts an aiMesh from assimp to a seb Mesh and returns it 
    static class Mesh* ProcessMesh(struct aiMesh* _mesh);

    // Converts an aiMatrial to a seb material and returns it
    static class Material* ProcessMaterial(struct aiMaterial* _material);

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters

};

