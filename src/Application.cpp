#include "Application.h"
#include <exception>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


Application* Application::Get()
{
	static Application* app = new Application;
	return app;
}

void Application::InitializeGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Application::Init()
{
    InitializeGLFW();
    mWindow.Init();
    mWindow.RegisterWindowCallbacks();
}

void Application::LoadContent()
{
    mWindow.LoadContent(&mScene);
}

int Application::Run()
{
    Init();
    LoadContent();

    float lastFrame = 0.f;

    while (!mWindow.IsClosed())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        mWindow.StartFrame();
        mWindow.Update(deltaTime);
        mWindow.Render(deltaTime);
        mWindow.EndFrame();
    }

    Mesh::ClearCache();
    Material::ClearCache();
    Texture::ClearCache();

    glfwTerminate();

    return 0;
}

