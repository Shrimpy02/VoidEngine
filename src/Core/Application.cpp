// Classes
#include <Core/Application.h>
#include <Core/Window.h>
#include <Levels/LevelManager.h>

// Additional libraries
// Glad + glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "UserInterface/UserInterfaceManager.h"

Application* Application::Get()
{
	static Application* app = new Application;
	return app;
}

int Application::Run()
{
    mUserInterfaceManager = std::make_shared<UserInterfaceManager>();
    mWindow = std::make_shared<Window>("LearnOpenGL", 1980, 1020, mUserInterfaceManager);
	mLevelManager = std::make_shared<LevelManager>(mWindow, mUserInterfaceManager);

    // Creates window class and initializes it
    Init();
    // Loads scene content from window
    LoadContent();

    float lastFrame = 0.f;

    // This is the entire render loop
    while (!mWindow->IsClosed())
    {
        // Calc delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        mWindow->StartFrame();
        mWindow->Update(deltaTime);
        mWindow->Render(deltaTime);
        mWindow->EndFrame();
    }

    // Clears static cache of content
    //Mesh::ClearCache();
    //Material::ClearCache();
    //Texture::ClearCache();

    glfwTerminate();

    return 0;
}

void Application::Init()
{
    InitializeGLFW();
    mWindow->Init();
    mWindow->RegisterWindowCallbacks();
}

void Application::InitializeGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Application::LoadContent()
{
    mWindow->LoadContent(mLevelManager);
}

