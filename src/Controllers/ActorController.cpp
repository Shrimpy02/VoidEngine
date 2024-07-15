
// Class Include
#include <Controllers/ActorController.h>
#include <Actor.h>
#include <Core/Window.h>
#include <GLFW/glfw3.h>
#include <Components/PhysicsComponent.h>
#include <Utilities/Logger.h>
#include <Camera.h>

void ActorController::Update(float _dt)
{
    // updates the actor each tick with values from this class
	UpdateActor(_dt);
}

void ActorController::HandleMouseMove(std::shared_ptr<Window> _window, double _xpos, double _ypos)
{
    if(std::shared_ptr<CameraActor> camActor = std::dynamic_pointer_cast<CameraActor>(mActor))
    {
        // only handle mouse movement if right mouse button is pressed
        if (!mRightMousePressed) return;

        // checks with last x and y for consistent mouse movement
        float xoffset = mLastX - (float)_xpos;
        float yoffset = mLastY - (float)_ypos;

        mLastX = (float)_xpos;
        mLastY = (float)_ypos;

        // applies mouse sensitivity 
        xoffset *= mMouseSensitivity;
        yoffset *= mMouseSensitivity;

        // gets teh angular speed for a smooth camera rotation based on mouse movement
        auto angularSpeed = camActor->GetAngularAccelerationSpeed();

        // Adds the acceleration to the camera for it to rotate
        camActor->AddAngularAcceleration({ xoffset * angularSpeed, yoffset * angularSpeed });
        
    }
}

void ActorController::HandleMouseScroll(std::shared_ptr<Window> _window, double _xoffset, double _yoffset)
{
}

void ActorController::HandleMouseButton(std::shared_ptr<Window> _window, int _button, int _action, int _mods)
{
    // if right mouse button is pressed, enable the local bool and remember pose positions for consistency,
    // then update glfw input mode and cursor location
    if (_button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (_action == GLFW_PRESS)
        {
            mRightMousePressed = true;
            mLastX = static_cast<float>(_window->GetWidth() / 2);
            mLastY = static_cast<float>(_window->GetHeight() / 2);
            glfwSetCursorPos(_window->GetGLFWWindow(), _window->GetWidth() / 2, _window->GetHeight() / 2);
            glfwSetInputMode(_window->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (_action == GLFW_RELEASE)
        {
            mRightMousePressed = false;
            glfwSetInputMode(_window->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mLastX = static_cast<float>(_window->GetWidth() / 2);
            mLastY = static_cast<float>(_window->GetHeight() / 2);
            glfwSetCursorPos(_window->GetGLFWWindow(), _window->GetWidth() / 2, _window->GetHeight() / 2);
        }
    }
}

void ActorController::HandleKeyboard(std::shared_ptr<Window> _window, int _key, int _scancode, int _action, int _mods)
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
	if (!mActor) return;


    if (std::shared_ptr<CameraActor> camActor = std::dynamic_pointer_cast<CameraActor>(mActor))
    {
        // Gets the acceleration for update
        glm::vec3 acceleration = camActor->GetAcceleration();
        float movementSpeed = camActor->GetAccelerationSpeed();

        acceleration = glm::vec3(0.f);
        // Checks key state and updates acceleration
        if (mKeyStates[GLFW_KEY_W]) acceleration.z += movementSpeed;
        if (mKeyStates[GLFW_KEY_S]) acceleration.z -= movementSpeed;
        if (mKeyStates[GLFW_KEY_A]) acceleration.x -= movementSpeed;
        if (mKeyStates[GLFW_KEY_D]) acceleration.x += movementSpeed;
        if (mKeyStates[GLFW_KEY_Q]) acceleration.y -= movementSpeed;
        if (mKeyStates[GLFW_KEY_E]) acceleration.y += movementSpeed;

        // shuts down glfw if escape is pressed
        if (mKeyStates[GLFW_KEY_ESCAPE] && mWindow)
            glfwSetWindowShouldClose(mWindow->GetGLFWWindow(), true);

        // Sets the camera acceleration.
        camActor->SetAcceleration(acceleration);

    } else {

        // Updates actor position if any of these keys are pressed
        if (mKeyStates[GLFW_KEY_D])
            mActor->SetGlobalPosition(mActor->GetGlobalPosition() + glm::vec3(1.0f, 0.f, 0.f) * _dt * mMovementSpeed);

        if (mKeyStates[GLFW_KEY_A])
            mActor->SetGlobalPosition(mActor->GetGlobalPosition() - glm::vec3(1.0f, 0.f, 0.f) * _dt * mMovementSpeed);

        if (mKeyStates[GLFW_KEY_W])
            mActor->SetGlobalPosition(mActor->GetGlobalPosition() - glm::vec3(0.f, 0.f, 1.0f) * _dt * mMovementSpeed);

        if (mKeyStates[GLFW_KEY_S])
            mActor->SetGlobalPosition(mActor->GetGlobalPosition() + glm::vec3(0.0f, 0.f, 1.0f) * _dt * mMovementSpeed);

        if (mKeyStates[GLFW_KEY_Q])
            mActor->SetGlobalPosition(mActor->GetGlobalPosition() + glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed);

        if (mKeyStates[GLFW_KEY_E])
            mActor->SetGlobalPosition(mActor->GetGlobalPosition() - glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed);

        // if escape pressed end program
        if (mKeyStates[GLFW_KEY_ESCAPE] && mWindow)
            glfwSetWindowShouldClose(mWindow->GetGLFWWindow(), true);

        if (mKeyStates[GLFW_KEY_SPACE])
        {
            // std::vector<std::shared_ptr<PhysicsComponent>> physicsComponents;
            // mActor->QueryPhysicsComponents<std::shared_ptr<PhysicsComponent>(physicsComponents);
            // if (physicsComponents[0])
            //     physicsComponents[0]->Jump();
        }
    }

}