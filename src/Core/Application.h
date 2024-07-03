#pragma once

#include <Core/Window.h>
#include <Levels/LevelManager.h>

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

    Window mWindow{ "LearnOpenGL", 1980, 1020 };
    std::shared_ptr<LevelManager> mLevelManager = std::make_shared<LevelManager>(&mWindow);

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

