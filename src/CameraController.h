#pragma once
#include <Controller.h>
#include <map>

class CameraController : public IController
{
public:
    // ---------- Global Variables --------------


private:
    // ---------- Local Variables --------------

    float mLastX = 0.0, mLastY = 0.0;
    bool mRightMousePressed = false;
    float mMouseSensitivity = 0.8f;

    std::map<int, bool> mKeyStates;
    class CameraActor* mCamera = nullptr;
    class Window* mWindow = nullptr;

public:
    // ---------- Global functions --------------

    CameraController(class CameraActor* camera, Window* _window) : mCamera(camera), mWindow(_window) {}

    virtual void Update(float _dt) override;
    virtual void HandleMouseMove(class Window* _window, double _xpos, double _ypos) override;
    virtual void HandleMouseScroll(class Window* _window, double _xoffset, double _yoffset) override;
    virtual void HandleMouseButton(class Window* _window, int _button, int _action, int _mods) override;
    virtual void HandleKeyboard(class Window* _window, int _key, int _scancode, int _action, int _mods) override;

private:
    // ---------- Local functions --------------

    void UpdateCameraAcceleration();

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters
 
};

