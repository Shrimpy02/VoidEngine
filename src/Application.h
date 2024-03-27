#pragma once

#include <Window.h>
#include <Scene.h>
#include <memory>

class Application
{
public:
    // ---------- Global Variables --------------


private:
    // ---------- Local Variables --------------

    Window mWindow{ "LearnOpenGL", 1280, 720 };
    Scene mScene{ "Scene", &mWindow };
    

public:
    // ---------- Global functions --------------

    Application(const Application&) = delete;
    Application(Application&&) = delete;

    static Application* Get();

    void InitializeGLFW();

    void Init();

    void LoadContent();

    int Run();

private:
    // ---------- Local functions --------------

    Application() = default;

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters

};

