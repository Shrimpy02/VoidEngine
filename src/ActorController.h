#pragma once

#include <Controller.h>
#include <map>

class ActorController : public IController
{
public:
    // ---------- Global Variables --------------


private:
    // ---------- Local Variables --------------

    std::map<int, bool> mKeyStates;
    float mMovementSpeed = 1;
    class Actor* mActor;
    class Window* mWindow; 

public:
    // ---------- Global functions --------------

    ActorController(class Actor* _actor, class Window* _window) : mActor(_actor), mWindow(_window) {}

    virtual void Update(float _dt) override;
    virtual void HandleMouseMove(class Window* _window, double _xpos, double _ypos) override;
    virtual void HandleMouseScroll(class Window* _window, double _xoffset, double _yoffset) override;
    virtual void HandleMouseButton(class Window* _window, int _button, int _action, int _mods) override;
    virtual void HandleKeyboard(class Window* _window, int _key, int _scancode, int _action, int _mods) override;

private:
    // ---------- Local functions --------------

    void UpdateActor(float _dt);

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters
    void SetNewActorToControll(class Actor* _newActor) { mActor = _newActor; }
    void SetMovementSpeed(float _inMovementSpeed) { mMovementSpeed = _inMovementSpeed; }
    // Getters
    float GetMovementSpeed() { return mMovementSpeed; }
};

