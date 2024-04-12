#pragma once

// Inlcuedes
#include <Utilities/Types.h>
#include <initializer_list>
#include <string>

/**
 * @class Skybox
 * @brief Is responsible for world sky-box managing and contains
 * functions to assign sky-box mesh and shader.
 */
class Skybox
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

    TextureID mTextureId{ 0 };

    class Mesh* mMesh{ nullptr };
    class Shader* mSkyboxShader{ nullptr };

public:
    // ---------- Global functions --------------
    // Constructor
    Skybox(std::initializer_list<std::string> texturePaths);

    // de-constructor
    ~Skybox();

    // Renders the skybox geometry and textures using custom skybox shader
    void Render(class CameraActor* renderCamera);

private:
    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------



};