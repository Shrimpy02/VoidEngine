#pragma once

// Includes
#include <Controllers/Controller.h>

// Additional Includes
#include <memory> 
#include <map>

#include "LevelActors/CameraActor.h"

// Forward Declarations
class Actor;
class WindowManager;
class LevelManager;
class CameraActor;
class UserInterfaceManager;

enum class LeftClickOptions
{
	Select,
    AddForce,
    ShootBall
};

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
    float mMovementSpeed = 5.f;
    float mClickVelocity = 5.f;

    // assigned actor
    std::shared_ptr<Actor> mControlledActor;
    // Window reference
    std::shared_ptr<WindowManager> mWindowManager;
    std::shared_ptr<LevelManager> mLevelManager;
    std::shared_ptr<UserInterfaceManager> mUserInterfaceManager;
    LeftClickOptions mLeftClickOptions = LeftClickOptions::Select;

    // Control settings camera
    float mLastX = 0.0, mLastY = 0.0;
    bool mRightMousePressed = false;
    float mMouseSensitivity = 0.8f;

    glm::vec3 mCameraSnapOffset{ 0.f, 2.f, 7.f };

public:
    // ---------- Global functions --------------

    // Constructor
    ActorController(std::shared_ptr<WindowManager> _windowManager, std::shared_ptr<UserInterfaceManager> _userInterfaceManager);

    // Inherited update from IController, updates all local function each tick. 
    void ProcessInput(float _dt) override;

    // Inherited from IController, empty function
    void HandleMouseMove(double _xPos, double _yPos) override;
    // Inherited from IController, empty function
    void HandleMouseScroll(double _xOffset, double _yOffset) override;
    // Inherited from IController, empty function
    void HandleMouseButton(int _button, int _action, int _mods) override;
    // Inherited from IController, asigness key states to the keystates maps
    void HandleKeyboard(int _key, int _scanCode, int _action, int _mods) override;
    // Inherited from IController, empty function
    void HandleChar(unsigned codePoint) override;
    void HandleViewportClick(int _button, int _action, int _mods, double _cursorPosX, double _cursorPosY);

    void SnapCameraToControlledActor(std::shared_ptr<CameraActor> _camRef);

    void UnSnapCameraToControlledActor();

    void ExitApplication();

private:
    // ---------- Local functions --------------

    // Updates the assigned actor by what key has been pressed
    void UpdateControls(float _dt);

    void UIInput(float _dt);

    void CameraInput(float _dt);

    void ActorInput(float _dt);

    // temp
	void ShootBullet(glm::vec3 shootingPos, glm::vec3 shootingDirection);

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

	// Sets a new actor to control
    void SetActorToControl(std::shared_ptr<Actor> _newActor) { mControlledActor = _newActor; }
    // sets the actor movement speed
    void SetMovementSpeed(float _inMovementSpeed) { mMovementSpeed = _inMovementSpeed; }

    void SetLeftClickFocus(LeftClickOptions _inOption) { mLeftClickOptions = _inOption; }

    void SetWindowManager(std::shared_ptr<WindowManager> _inWindowManager) { mWindowManager = _inWindowManager; };

    void SetClickVelocity(float _inVelocity) { mClickVelocity = _inVelocity; }

    // Getters
    void GetLevelManager(std::shared_ptr<LevelManager> _inManager) { mLevelManager = _inManager; }
    std::shared_ptr<Actor> GetRefToControlledActor() { return mControlledActor; }

    // gets the current movement speed
    float GetMovementSpeed() { return mMovementSpeed; }

    float GetClickVelocity() { return mClickVelocity; }
    


};

