#pragma once


// Includes


// Additional Includes
#include <memory> 


// Forward Declarations
class Window;
class LevelManager;
class UserInterfaceManager;

/**
 * @class Application,
 * @brief Manages GLFW and the application as a whole. It contains the render loop and Init space. 
 */
class Application
{
public:
    // ---------- Global Variables --------------


private:
    // ---------- Local Variables --------------

    std::shared_ptr<Window> mWindow{ nullptr };
    std::shared_ptr<UserInterfaceManager> mUserInterfaceManager{ nullptr };
    std::shared_ptr<LevelManager> mLevelManager{ nullptr };

public:
    // ---------- Global functions --------------

    // Uses default constructor

    // Disable copying and deleting of this class
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = default;
    Application& operator=(Application&&) = default;

    // Called from "Main()", create`s itself and calls run();
    static Application* Get();

    // Runs everything, init then render loop.
    int Run();
    
    // Creats Window object to manage viewport and scene focus
    void Init();

    // Init and sets window context version for GLFW
    void InitializeGLFW();

    // Let`s Window know the application has started
    // running and can load all content for the relevant scene. 
    void LoadContent();



private:
    // ---------- Local functions --------------

    Application() = default;

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters

};

