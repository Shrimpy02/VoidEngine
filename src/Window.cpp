
// Classes
#include "Window.h"
#include "Scene.h"
#include "Logger.h"

// Additional libraries
// Glad + glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


Window::Window(std::string name, int width, int height)
    :mName(name), mWidth(width), mHeight(height)
{}

Window::~Window()
{
    // Destroys viewport window
    glfwDestroyWindow(mGLFWWindow);

    // ImGui shutdown
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Delete references
    delete mScene;
}

void Window::Init()
{
    // Init GLFW-window ------------
    mGLFWWindow = glfwCreateWindow(mWidth, mHeight, mName.c_str(), NULL, NULL);
    if (mGLFWWindow == NULL)
    {
        glfwTerminate();
        LOG_ERROR("GLFW Window did not initalize");
    }
    glfwMakeContextCurrent(mGLFWWindow);
    LOG("Window Init Success");

    // Init Glad ------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Faild to initialize GLAD\n";
        LOG_ERROR("GLAD did not initalize");
    }
    LOG("GLAD Init Success");

    // Init ImGui ------------
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
    LOG("Scene loading complete");
    return true;
}

void Window::StartFrame()
{
	glfwPollEvents();

    // Tell`s ImGui this is the first frame of the render loop
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Keeps the scenes window pointer up to date
    mScene->SetWindow(this);

    // Clears color and depth buffer for OpenGL rendering
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
    // Tell`s ImGui this is the last frame of the render loop
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swaps buffers to next image frame & polls events again
    glfwSwapBuffers(mGLFWWindow);
    glfwPollEvents();
}

bool Window::IsClosed()
{
    return glfwWindowShouldClose(mGLFWWindow);
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Callback to re-size the window dynamically
    glViewport(0, 0, width, height);
}

void Window::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Stops glfw from capturing the mouse when ImGui wants it
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (mScene)
        mScene->MouseMoveCallback(this, xpos, ypos);
}

void Window::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Passes all scroll callbacks to ImGui and stops glfw from using them
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	if (ImGui::GetIO().WantCaptureMouse) return;

	if (mScene)
        mScene->MouseScrollCallback(this, xoffset, yoffset);
}

void Window::CharCallback(GLFWwindow* window, unsigned int codepoint)
{
    // Passes all character keyboard callbacks to ImGui and stops glfw from using them (a,b,c so on)
    ImGui_ImplGlfw_CharCallback(window, codepoint);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (mScene)
        mScene->CharCallback(this, codepoint);
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Passes all key-keyboard callbacks to ImGui and stops glfw from using them (backspace, enter so on)
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (mScene)
        mScene->KeyCallback(this, key, scancode, action, mods);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    // Passes all mouse button callbacks to ImGui and stops glfw from using them
   ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
   if (ImGui::GetIO().WantCaptureMouse) return;

    if (mScene)
        mScene->MouseButtonCallback(this, button, action, mods);
}