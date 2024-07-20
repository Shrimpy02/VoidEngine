#pragma once

// Includes
#include <Controllers/Controller.h>

// Additional Includes
#include <memory> 
#include <map>

// Forward Declarations
class Actor;
class Window;
class CameraActor;

/**
 * @class ActorController,
 * @brief ActorController inherits from IController giving it key input control.
 * It lets the player manage the assign actor by input. For example movement. 
 */
class ActorController : public IController
{
public:
    // ---------- Global Variables --------------

    std::shared_ptr<CameraActor> mCameraForSnap{ nullptr };

private:
    // ---------- Local Variables --------------

    // Key map for input handeling
    std::map<int, bool> mKeyStates;
    // actor movement speed
    float mMovementSpeed = 5;
    // assigned actor
    std::shared_ptr<Actor> mActor;
    // Window reference
    std::shared_ptr<Window> mWindow;


    // Control settings camera
    float mLastX = 0.0, mLastY = 0.0;
    bool mRightMousePressed = false;
    float mMouseSensitivity = 0.8f;

public:
    // ---------- Global functions --------------

    // Constructor
    ActorController(std::shared_ptr<Actor> _actor, std::shared_ptr<Window> _window) : mActor(_actor), mWindow(_window) {}

    // Inherited update from IController, updates all local function each tick. 
    void Update(float _dt) override;

    // Inherited from IController, empty function
    void HandleMouseMove(std::shared_ptr<Window> _window, double _xpos, double _ypos) override;
    // Inherited from IController, empty function
    void HandleMouseScroll(std::shared_ptr<Window> _window, double _xoffset, double _yoffset) override;
    // Inherited from IController, empty function
    void HandleMouseButton(std::shared_ptr<Window> _window, int _button, int _action, int _mods) override;
    // Inherited from IController, asigness key states to the keystates maps
    void HandleKeyboard(std::shared_ptr<Window> _window, int _key, int _scancode, int _action, int _mods) override;

private:
    // ---------- Local functions --------------

    // Updates the assigned actor by what key has been pressed
    void UpdateActor(float _dt);

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

	// Sets a new actor to control
    void SetNewActorToControl(std::shared_ptr<Actor> _newActor) override { mActor = _newActor; }
    // sets the actor movement speed
    void SetMovementSpeed(float _inMovementSpeed) override { mMovementSpeed = _inMovementSpeed; }

    void SetCameraForSnap(std::shared_ptr<CameraActor> _camRef) override { mCameraForSnap = _camRef; }

    // Getters

    std::shared_ptr<Actor> GetRefToControlledActor() { return mActor; }

    // gets the current movement speed
    float GetMovementSpeed() override { return mMovementSpeed; }
};

