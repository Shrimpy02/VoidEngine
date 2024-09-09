
// Includes
#include <Core/WindowManager.h>
#include <UserInterface/UserInterfaceManager.h>
#include <Utilities/Logger.h>

// Additional includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

WindowManager::WindowManager(std::string _name)
    :mWindowName(_name)
{
   
}

WindowManager::~WindowManager()
{
    // Destroys viewport window and it`s place in memory
    glfwDestroyWindow(mGLFWWindow);
}

void WindowManager::Init()
{
    // Init GLFW-window ---------------------
    // Functions: https://www.glfw.org/docs/3.3/group__window.html
    // Documentation: https://www.glfw.org/docs/3.3/window_guide.html#window_hints

    // Get the primary monitor resolution for window creation
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    glfwGetMonitorPos(primaryMonitor, &mWindowedPosX, &mWindowedPosY);
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
    mWindowedWidth = videoMode->width;
    mWindowedHeight = videoMode->height;

	mGLFWWindow = glfwCreateWindow(mWindowedWidth, mWindowedHeight, mWindowName.c_str(), NULL, NULL);
    if (mGLFWWindow == NULL)
    {
        glfwTerminate();
        LOG_ERROR("GLFW Window did not initalize");
    }
    glfwMakeContextCurrent(mGLFWWindow);
    glfwSetWindowMonitor(mGLFWWindow, nullptr, mWindowedPosX, mWindowedPosY + mWindowTopBarOffset, mWindowedWidth, mWindowedHeight, 0);

    LOG("Window Init Success");

    // Init Glad ---------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Faild to initialize GLAD\n";
        LOG_ERROR("GLAD did not initalize");
    }
    LOG("GLAD Init Success");

    // Init ImGui ---------------------
    mUserInterfaceManager = std::make_shared<UserInterfaceManager>(shared_from_this());
    mUserInterfaceManager->Init(mGLFWWindow);
}

void WindowManager::RegisterWindowCallbacks()
{
    glfwSetFramebufferSizeCallback(mGLFWWindow, [](GLFWwindow* _window, int _width, int _height) {
        auto app = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(_window));
        if (app)
        {
            app->FrameBufferSizeCallback(_window, _width, _height);
            app->SetWidth(_width);
            app->SetHeight(_height);
        }
        });

    glfwSetCursorPosCallback(mGLFWWindow, [](GLFWwindow* _window, double _xpos, double _ypos) {
        auto app = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(_window));
        if (app) app->MouseMoveCallback(_window, _xpos, _ypos);
        });
    glfwSetMouseButtonCallback(mGLFWWindow, [](GLFWwindow* _window, int _button, int _action, int _mods) {
        auto app = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(_window));
        if (app) app->MouseButtonCallback(_window, _button, _action, _mods);
        });

    glfwSetScrollCallback(mGLFWWindow, [](GLFWwindow* _window, double _xoffset, double _yoffset) {
        auto app = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(_window));
        if (app) app->MouseScrollCallback(_window, _xoffset, _yoffset);
        });

    glfwSetCharCallback(mGLFWWindow, [](GLFWwindow* _window, unsigned int _codepoint) {
        auto app = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(_window));
        if (app) app->CharCallback(_window, _codepoint);
        });

	glfwSetKeyCallback(mGLFWWindow, [](GLFWwindow* _window, int _key, int _scancode, int _action, int _mods) {
        auto app = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(_window));
        if (app) app->KeyCallback(_window, _key, _scancode, _action, _mods);
        });

    // Store pointer so it can be accessed in callbacks
    glfwSetWindowUserPointer(mGLFWWindow, this);
}

void WindowManager::LoadContent()
{
    // Loads scene and scene content
    LOG_INFO("----- Default level & UI loading started ----- ");
    mUserInterfaceManager->LoadContent();
    LOG_INFO("----- Default level & UI loading complete ----- ");
}

void WindowManager::StartFrame()
{
    // Gets poll evenets
	glfwPollEvents();

    if (mUserInterfaceManager)
        mUserInterfaceManager->StartFrame();

    // Clears color and depth buffer for OpenGL rendering
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WindowManager::Update(float _dt)
{
    if (mUserInterfaceManager)
        mUserInterfaceManager->Update(_dt);

}

void WindowManager::Render()
{
    if (mUserInterfaceManager)
        mUserInterfaceManager->Render();
}

void WindowManager::EndFrame()
{
    if (mUserInterfaceManager)
        mUserInterfaceManager->EndFrame();

    // Swaps buffers to next image frame & polls events again
    glfwSwapBuffers(mGLFWWindow);
    glfwPollEvents();
}

bool WindowManager::IsClosed()
{
    return glfwWindowShouldClose(mGLFWWindow);
}

void WindowManager::SetWindowed()
{
    if (!mGLFWWindow) return;

    // Get the primary monitor and its video mode
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

    // Restore the previous window position and size if transitioning from fullscreen or borderless mode
    glfwSetWindowAttrib(mGLFWWindow, GLFW_DECORATED, GLFW_TRUE); 

    // Set windowed mode
    glfwSetWindowMonitor(mGLFWWindow, nullptr, mWindowedPosX, mWindowedPosY + mWindowTopBarOffset, mWindowedWidth, mWindowedHeight, 0);

    // Update the state
    mWindowMode = WindowMode::WM_Windowed;

    // Reconfigure the OpenGL viewport to match the new window size
    int width, height;
    glfwGetFramebufferSize(mGLFWWindow, &width, &height);
    glViewport(0, 0, width, height);
}

void WindowManager::SetBoarderLessWindow()
{
    if (!mGLFWWindow) return;

    // Get the current monitor and video mode
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

    // Store the current window position and size
    glfwGetWindowPos(mGLFWWindow, &mWindowedPosX, &mWindowedPosY);
    glfwGetWindowSize(mGLFWWindow, &mWindowedWidth, &mWindowedHeight);

    // Switch to borderless windowed mode
    glfwSetWindowAttrib(mGLFWWindow, GLFW_DECORATED, GLFW_FALSE);
    glfwSetWindowMonitor(mGLFWWindow, nullptr, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);

    // Update state
    mWindowMode = WindowMode::WM_BoarderLess;

    // Reconfigure the OpenGL viewport to match the new window size
    int width, height;
    glfwGetFramebufferSize(mGLFWWindow, &width, &height);
    glViewport(0, 0, width, height);
}

void WindowManager::SetFullScreen()
{
    if (!mGLFWWindow) return;

    // Get the primary monitor and its video mode
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

    // Store the current window position and size
    glfwGetWindowPos(mGLFWWindow, &mWindowedPosX, &mWindowedPosY);
    glfwGetWindowSize(mGLFWWindow, &mWindowedWidth, &mWindowedHeight);

    // Switch to fullscreen mode
    glfwSetWindowMonitor(mGLFWWindow, monitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);

    // Update the states
    mWindowMode = WM_FullScreen;

    // Reconfigure the OpenGL viewport to match the new window size
    int width, height;
    glfwGetFramebufferSize(mGLFWWindow, &width, &height);
    glViewport(0, 0, width, height);
}

void WindowManager::FrameBufferSizeCallback(GLFWwindow* _window, int _width, int _height)
{
    // Callback to re-size the window dynamically
    glViewport(0, 0, _width, _height);

	if (mUserInterfaceManager)
        mUserInterfaceManager->FrameBufferCallback(_window, _width, _height);
}

void WindowManager::MouseMoveCallback(GLFWwindow* _window, const double _xPos, const double _yPos)
{
    if (mUserInterfaceManager)
        mUserInterfaceManager->MouseMoveCallback(_window, _xPos, _yPos);
}

void WindowManager::MouseScrollCallback(GLFWwindow* _window, double _xOffset, double _yOffset)
{
    if (mUserInterfaceManager)
        mUserInterfaceManager->MouseScrollCallback(_window, _xOffset, _yOffset);
}

void WindowManager::CharCallback(GLFWwindow* _window, unsigned int _codepoint)
{
    if (mUserInterfaceManager)
        mUserInterfaceManager->CharCallback(_window, _codepoint);
}

void WindowManager::KeyCallback(GLFWwindow* _window, int _key, int _scanCode, int _action, int _mods)
{
    if (mUserInterfaceManager)
        mUserInterfaceManager->KeyCallback(_window, _key, _scanCode, _action, _mods);
}

void WindowManager::MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods)
{
    double cursorX, cursorY;
    glfwGetCursorPos(_window, &cursorX, &cursorY);

   if (mUserInterfaceManager)
       mUserInterfaceManager->MouseButtonCallback(_window, _button, _action, _mods, cursorX, cursorY);
}