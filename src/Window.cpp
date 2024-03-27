#include "Window.h"
#include "Scene.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <windows.h>

Window::Window(std::string name, int width, int height)
    :mName(name), mWidth(width), mHeight(height)
{}

Window::~Window()
{
    glfwDestroyWindow(mGLFWWindow);

    // ImGui shutdown
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Window::Init()
{
    mGLFWWindow = glfwCreateWindow(mWidth, mHeight, mName.c_str(), NULL, NULL);
    if (mGLFWWindow == NULL)
    {
        glfwTerminate();
        std::cout << "Failed to create GLFW window\n";
    }
    glfwMakeContextCurrent(mGLFWWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Faild to initialize GLAD\n";
    }

	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(mGLFWWindow, false);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void Window::RegisterWindowCallbacks()
{
    glfwSetFramebufferSizeCallback(mGLFWWindow, [](GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        if (app)
        {
            app->FramebufferSizeCallback(window, width, height);
            app->SetWidth(width);
            app->SetHeight(height);
        }
        });

    glfwSetCursorPosCallback(mGLFWWindow, [](GLFWwindow* window, double xpos, double ypos) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        if (app) app->MouseMoveCallback(window, xpos, ypos);
        });
    glfwSetMouseButtonCallback(mGLFWWindow, [](GLFWwindow* window, int button, int action, int mods) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        if (app) app->MouseButtonCallback(window, button, action, mods);
        });

    glfwSetScrollCallback(mGLFWWindow, [](GLFWwindow* window, double xoffset, double yoffset) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        if (app) app->MouseScrollCallback(window, xoffset, yoffset);
        });

    glfwSetCharCallback(mGLFWWindow, [](GLFWwindow* window, unsigned int codepoint) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        if (app) app->CharCallback(window, codepoint);
        });

	glfwSetKeyCallback(mGLFWWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        if (app) app->KeyCallback(window, key, scancode, action, mods);
        });

    // Store pointer so it can be accessed in callbacks
    glfwSetWindowUserPointer(mGLFWWindow, this);
}

bool Window::LoadContent(Scene* _scene)
{
    mScene = _scene;
    mScene->LoadContent();
    return true;
}

void Window::StartFrame()
{
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  
    mScene->SetWindow(this);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Update(float dt)
{
    if (mScene)
        mScene->Update(dt);
}

void Window::Render(float dt)
{
    if (mScene)
        mScene->Render(dt);
}

void Window::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(mGLFWWindow);
    glfwPollEvents();
}

bool Window::IsClosed()
{
    return glfwWindowShouldClose(mGLFWWindow);
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Window::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (mScene)
        mScene->MouseMoveCallback(this, xpos, ypos);
}

void Window::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	if (ImGui::GetIO().WantCaptureMouse) return;

	if (mScene)
        mScene->MouseScrollCallback(this, xoffset, yoffset);
}

void Window::CharCallback(GLFWwindow* window, unsigned int codepoint)
{
    ImGui_ImplGlfw_CharCallback(window, codepoint);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (mScene)
        mScene->CharCallback(this, codepoint);
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (mScene)
        mScene->KeyCallback(this, key, scancode, action, mods);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
   ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
   if (ImGui::GetIO().WantCaptureMouse) return;

    if (mScene)
        mScene->MouseButtonCallback(this, button, action, mods);
}