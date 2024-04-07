
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

Window::Window(std::string _name, int _width, int _height)
    :mName(_name), mWidth(_width), mHeight(_height)
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
    glfwSetFramebufferSizeCallback(mGLFWWindow, [](GLFWwindow* _window, int _width, int _height) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
        if (app)
        {
            app->FramebufferSizeCallback(_window, _width, _height);
            app->SetWidth(_width);
            app->SetHeight(_height);
        }
        });

    glfwSetCursorPosCallback(mGLFWWindow, [](GLFWwindow* _window, double _xpos, double _ypos) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
        if (app) app->MouseMoveCallback(_window, _xpos, _ypos);
        });
    glfwSetMouseButtonCallback(mGLFWWindow, [](GLFWwindow* _window, int _button, int _action, int _mods) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
        if (app) app->MouseButtonCallback(_window, _button, _action, _mods);
        });

    glfwSetScrollCallback(mGLFWWindow, [](GLFWwindow* _window, double _xoffset, double _yoffset) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
        if (app) app->MouseScrollCallback(_window, _xoffset, _yoffset);
        });

    glfwSetCharCallback(mGLFWWindow, [](GLFWwindow* _window, unsigned int _codepoint) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
        if (app) app->CharCallback(_window, _codepoint);
        });

	glfwSetKeyCallback(mGLFWWindow, [](GLFWwindow* _window, int _key, int _scancode, int _action, int _mods) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
        if (app) app->KeyCallback(_window, _key, _scancode, _action, _mods);
        });

    // Store pointer so it can be accessed in callbacks
    glfwSetWindowUserPointer(mGLFWWindow, this);
}

bool Window::LoadContent(Scene* _scene)
{
    // Loads scene and scene content
    LOG_INFO("----- Scene loading started ----- ");
    mScene = _scene;
    mScene->LoadContent();
    LOG_INFO("----- Scene loading complete ----- ");
    return true;
}

void Window::StartFrame()
{
    // Gets poll evenets
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

void Window::Update(float _dt)
{
    if (mScene)
        mScene->Update(_dt);
}

void Window::Render(float _dt)
{
    if (mScene)
        mScene->Render(_dt);
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

void Window::FramebufferSizeCallback(GLFWwindow* _window, int _width, int _height)
{
    // Callback to re-size the window dynamically
    glViewport(0, 0, _width, _height);
}

void Window::MouseMoveCallback(GLFWwindow* _window, double _xpos, double _ypos)
{
    // Stops glfw from capturing the mouse when ImGui wants it
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (mScene)
        mScene->MouseMoveCallback(this, _xpos, _ypos);
}

void Window::MouseScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset)
{
    // Passes all scroll callbacks to ImGui and stops glfw from using them
    ImGui_ImplGlfw_ScrollCallback(_window, _xoffset, _yoffset);
	if (ImGui::GetIO().WantCaptureMouse) return;

	if (mScene)
        mScene->MouseScrollCallback(this, _xoffset, _yoffset);
}

void Window::CharCallback(GLFWwindow* _window, unsigned int _codepoint)
{
    // Passes all character keyboard callbacks to ImGui and stops glfw from using them (a,b,c so on)
    ImGui_ImplGlfw_CharCallback(_window, _codepoint);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (mScene)
        mScene->CharCallback(this, _codepoint);
}

void Window::KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
    // Passes all key-keyboard callbacks to ImGui and stops glfw from using them (backspace, enter so on)
	ImGui_ImplGlfw_KeyCallback(_window, _key, _scancode, _action, _mods);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (mScene)
        mScene->KeyCallback(this, _key, _scancode, _action, _mods);
}

void Window::MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods)
{
    // Passes all mouse button callbacks to ImGui and stops glfw from using them
   ImGui_ImplGlfw_MouseButtonCallback(_window, _button, _action, _mods);
   if (ImGui::GetIO().WantCaptureMouse) return;

    if (mScene)
        mScene->MouseButtonCallback(this, _button, _action, _mods);
}