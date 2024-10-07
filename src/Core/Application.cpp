
// Include
#include <Core/Application.h>
#include <Core/WindowManager.h>
#include <Core/SMath.h>
#include <Utilities/Logger.h>

// Additional include
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Application* Application::Get()
{
	static Application* app = new Application;
	return app;
}

int Application::Run()
{
    mWindowManager = std::make_shared<WindowManager>("VoidEngine");

    InitializeGLFW();
    InitializeWindow();
    SMath::SeedRandTime();
    //glEnable(GL_CULL_FACE);
    LOG_WARNING("Faceculling turened off");
    //glCullFace(GL_FRONT);

    // Render/Update loop -----------
    float lastFrame = 0.f;
    while (!mWindowManager->IsClosed())
    {
        // Calc delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        mWindowManager->StartFrame();
        mWindowManager->Update(deltaTime);
        mWindowManager->Render();
        mWindowManager->EndFrame();
    }

    glfwTerminate();

    return 0;
}

void Application::InitializeGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Application::InitializeWindow()
{
    mWindowManager->Init();
    mWindowManager->RegisterWindowCallbacks();
    mWindowManager->LoadContent();
}
