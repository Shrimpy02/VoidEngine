#pragma once

// Includes
#include <Controller.h>
#include <map>

/**
 * @class CameraController,
 * @brief ActorController inherits from IController giving it key input control.
 * It lets the player manage the assign camera by input. For example movement. 
 */
class CameraController : public IController
{
public:
    // ---------- Global Variables --------------


private:
    // ---------- Local Variables --------------

    // Control settings
    float mLastX = 0.0, mLastY = 0.0;
    bool mRightMousePressed = false;
    float mMouseSensitivity = 0.8f;

    // A map of key states to keys, for manipulation outside of this class.
    std::map<int, bool> mKeyStates;
    // the camera actor this controller is controlling. 
    class CameraActor* mCamera = nullptr;
    // A window reference
    class Window* mWindow = nullptr;

public:
    // ---------- Global functions --------------

    // Constructor
    CameraController(class CameraActor* camera, Window* _window) : mCamera(camera), mWindow(_window) {}

    ~CameraController() {};

    // Inherited from IController, Updates this controller each frame. 
    void Update(float _dt) override;

    // Inherited from IController, rotates the camera object based on mouse movement
    void HandleMouseMove(class Window* _window, double _xpos, double _ypos) override;
    // Inherited from IController, empty function
    void HandleMouseScroll(class Window* _window, double _xoffset, double _yoffset) override;
    // Inherited from IController, makes glfw capture mouse movement
    void HandleMouseButton(class Window* _window, int _button, int _action, int _mods) override;
    // Inherited from IController, asigness key states to the keystates maps
    void HandleKeyboard(class Window* _window, int _key, int _scancode, int _action, int _mods) override;

private:
    // ---------- Local functions --------------

    // Updates the camera acceleration for smooth movement.
    void UpdateCameraAcceleration();

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters
 
};

