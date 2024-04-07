
// Class Include
#include <Controllers/ActorController.h>
#include <Actor.h>
#include <Core/Window.h>
#include <GLFW/glfw3.h>

void ActorController::Update(float _dt)
{
    // updates the actor each tick with values from this class
	UpdateActor(_dt);
}

void ActorController::HandleMouseMove(Window* _window, double _xpos, double _ypos)
{
}

void ActorController::HandleMouseScroll(Window* _window, double _xoffset, double _yoffset)
{
}

void ActorController::HandleMouseButton(Window* _window, int _button, int _action, int _mods)
{
}

void ActorController::HandleKeyboard(Window* _window, int _key, int _scancode, int _action, int _mods)
{
    // if there is no actor being controlled end function
	if (!mActor) return;

    // otherwise assign keys in the keymap to be updated if they are pressed
	if (_action == GLFW_PRESS || _action == GLFW_REPEAT)
		mKeyStates[_key] = true;
	
	else if (_action == GLFW_RELEASE)
		mKeyStates[_key] = false;
	
}

void ActorController::UpdateActor(float _dt)
{
    // Updates actor position if any of these keys are pressed
    if (mKeyStates[GLFW_KEY_D])
        mActor->SetPosition(mActor->GetPosition(Actor::TransformSpace::Global) + glm::vec3(1.0f, 0.f, 0.f) * _dt * mMovementSpeed);
    
    if (mKeyStates[GLFW_KEY_A])
        mActor->SetPosition(mActor->GetPosition(Actor::TransformSpace::Global) - glm::vec3(1.0f, 0.f, 0.f) * _dt * mMovementSpeed);
    
    if (mKeyStates[GLFW_KEY_W])
        mActor->SetPosition(mActor->GetPosition(Actor::TransformSpace::Global) - glm::vec3(0.f, 0.f, 1.0f) * _dt * mMovementSpeed);
    
    if (mKeyStates[GLFW_KEY_S])
        mActor->SetPosition(mActor->GetPosition(Actor::TransformSpace::Global) + glm::vec3(0.0f, 0.f, 1.0f) * _dt * mMovementSpeed);

    if (mKeyStates[GLFW_KEY_Q])
        mActor->SetPosition(mActor->GetPosition(Actor::TransformSpace::Global) + glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed);

    if (mKeyStates[GLFW_KEY_E])
        mActor->SetPosition(mActor->GetPosition(Actor::TransformSpace::Global) - glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed);

    // if escape pressed end program
    if (mKeyStates[GLFW_KEY_ESCAPE] && mWindow)
        glfwSetWindowShouldClose(mWindow->GetGLFWWindow(), true);
}
