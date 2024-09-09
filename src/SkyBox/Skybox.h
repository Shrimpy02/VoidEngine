//#pragma once
//
//// Inlcuedes
//#include <Utilities/Types.h>
//
//// Additional Includes
//#include <memory> 
//#include <string>
//#include <initializer_list>
//
//// Forward Declarations
//class Mesh;
//class Shader;
//class CameraActor;
//
///**
// * @class Skybox
// * @brief Is responsible for world sky-box managing and contains
// * functions to assign sky-box mesh and shader.
// */
//class Skybox
//{
//public:
//    // ---------- Global Variables --------------
//
//private:
//    // ---------- Local Variables --------------
//
//    TextureID mTextureId{ 0 };
//
//    std::shared_ptr<Mesh> mMesh{ nullptr };
//    std::shared_ptr<Shader> mSkyboxShader{ nullptr };
//
//public:
//    // ---------- Global functions --------------
//    // Constructor
//    Skybox(std::initializer_list<std::string> texturePaths);
//
//    // de-constructor
//    ~Skybox();
//
//    // Renders the skybox geometry and textures using custom skybox shader
//    void Render(std::shared_ptr<CameraActor> renderCamera);
//
//private:
//    // ---------- Local functions --------------
//
//
//public:
//    // ---------- Getters / setters / Adders --------------
//
//
//
//};