#pragma once

// Includes
// Additional Includes
#include <memory> 

// Forward Declarations
class WindowManager;
class LevelManager;
class UserInterfaceManager;

/**
 * @class Application,
 * @brief The Application as a whole.
 * It Initializes the user-interface manager, by proxy the level manager and the GLFW Window.
 * It also contains the render loop where it calculates delta-time and call the start, update,
 * render and end frame functions found in the in the window. 
 */
class Application
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

    std::shared_ptr<WindowManager> mWindowManager{ nullptr };

public:
    // ---------- Global functions --------------

    explicit Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = default;
    Application& operator=(Application&&) = default;

    // Static function that creates itself
    static Application* Get();

    // Runs everything, app initialization then render loop.
    int Run();

private:
    // ---------- Local functions --------------

	// Initialize GLFW settings
    void InitializeGLFW();

    // Initialize Window, its callbacks and its content 
    void InitializeWindow();

public:
    // ---------- Getters / setters / Adders --------------

};

