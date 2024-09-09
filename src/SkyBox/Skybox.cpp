//
//// Includes
//#include <SkyBox/Skybox.h>
//#include <Core/Shader.h>
//#include <Utilities/Logger.h>
//#include <Utilities/Defines.h>
//#include <RenderElements/Mesh.h>
//#include <Camera.h>
//
//#include <glad/glad.h>
//#include <RenderElements/Texture.h>
//
//Skybox::Skybox(std::initializer_list<std::string> texturePaths)
//{
//    // Create shader and mesh for skybox
//    mSkyboxShader = std::make_shared<Shader>(SOURCE_DIRECTORY("shaderSrc/skybox.vs"), SOURCE_DIRECTORY("shaderSrc/skybox.fs"));
//    mMesh = Mesh::CreateCube(nullptr);
//
//    mTextureId = Texture::GenSkybox(texturePaths);
//}
//
//Skybox::~Skybox()
//{
//    // Mesh deletes itself
//    glDeleteTextures(1, &mTextureId);
//}
//
//void Skybox::Render(std::shared_ptr<CameraActor> renderCamera)
//{
//    if (!renderCamera) return;
//
//    // assign the correct shader to use
//    mSkyboxShader->use();
//
//    // Set view and projection matrix`es
//    mSkyboxShader->setMat4("view", glm::mat3(renderCamera->GetViewMatrix()));
//    mSkyboxShader->setMat4("projection", renderCamera->GetProjectionMatrix());
//
//    // Bind texture and draw mesh
//    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);
//    mMesh->Draw(mSkyboxShader);
//}
