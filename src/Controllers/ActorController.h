#pragma once

// Includes
#include <Controllers/Controller.h>

// Additional Includes
#include <memory> 
#include <map>

// Forward Declarations
class Actor;
class Window;

/**
 * @class ActorController,
 * @brief ActorController inherits from IController giving it key input control.
 * It lets the player manage the assign actor by input. For example movement. 
 */
class ActorController : public IController
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

    // Key map for input handeling
    std::map<int, bool> mKeyStates;
    // actor movement speed
    float mMovementSpeed = 1;
    // assigned actor
    std::shared_ptr<Actor> mActor;
    // Window reference
    std::shared_ptr<Window> mWindow;

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
    void SetNewActorToControll(std::shared_ptr<Actor> _newActor) { mActor = _newActor; }
    // sets the actor movement speed
    void SetMovementSpeed(float _inMovementSpeed) { mMovementSpeed = _inMovementSpeed; }

    // Getters

    // gets the current movement speed
    float GetMovementSpeed() { return mMovementSpeed; }
};

