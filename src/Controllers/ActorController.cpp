
// Includes
#include <Controllers/ActorController.h>
#include <Core/WindowManager.h>
#include <UserInterface/UserInterfaceManager.h>
#include <Components/PhysicsComponent.h>
#include <LevelActors/CameraActor.h>
#include <Utilities/Logger.h>

// Additional Includes
#include <GLFW/glfw3.h>

ActorController::ActorController(std::shared_ptr<WindowManager> _windowManager,
	std::shared_ptr<UserInterfaceManager> _userInterfaceManager)
		: mWindowManager(_windowManager), mUserInterfaceManager(_userInterfaceManager)
{
}

void ActorController::ProcessInput(float _dt)
{
    if (mCameraForSnap && mControlledActor)
        mCameraForSnap->SetGlobalPosition(mControlledActor->GetGlobalPosition() + mCameraSnapOffset);

    UpdateControls(_dt);
}

void ActorController::HandleMouseMove(double _xPos, double _yPos)
{
    // Only handle mouse movement if right mouse button is pressed
    if (!mRightMousePressed) return;

    // checks with last x and y for consistent mouse movement
    float xOffset = mLastX - static_cast<float>(_xPos);
    float yOffset = mLastY - static_cast<float>(_yPos);

    mLastX = static_cast<float>(_xPos);
    mLastY = static_cast<float>(_yPos);

    // applies mouse sensitivity 
    xOffset *= mMouseSensitivity;
    yOffset *= mMouseSensitivity;

    if(std::shared_ptr<CameraActor> camActor = std::dynamic_pointer_cast<CameraActor>(mControlledActor))
    {
        // gets teh angular speed for a smooth camera rotation based on mouse movement
        auto angularSpeed = camActor->GetAngularAccelerationSpeed();

        // Adds the acceleration to the camera for it to rotate
        camActor->AddAngularAcceleration({ xOffset * angularSpeed, yOffset * angularSpeed });

    } else if(mCameraForSnap) {

        // gets teh angular speed for a smooth camera rotation based on mouse movement
        auto angularSpeed = mCameraForSnap->GetAngularAccelerationSpeed();

        // Adds the acceleration to the camera for it to rotate
        mCameraForSnap->AddAngularAcceleration({ xOffset * angularSpeed, yOffset * angularSpeed });
    }
}

void ActorController::HandleMouseScroll(double _xOffset, double _yOffset)
{
}

void ActorController::HandleMouseButton(int _button, int _action, int _mods)
{
	if (_button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (_action == GLFW_PRESS )
        {
            if(mUserInterfaceManager)
            {
                mRightMousePressed = true;
                mLastX = mUserInterfaceManager->GetWindowPos().x + mUserInterfaceManager->GetViewportWidth() / 2;
                mLastY = mUserInterfaceManager->GetWindowPos().y + mUserInterfaceManager->GetViewportHeight() / 2;
                glfwSetCursorPos(mWindowManager->GetGLFWWindow(), mLastX, mLastY);
                glfwSetInputMode(mWindowManager->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }
        else if (_action == GLFW_RELEASE)
        {
            if(mUserInterfaceManager)
            {
                mRightMousePressed = false;
                glfwSetInputMode(mWindowManager->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                mLastX = mUserInterfaceManager->GetWindowPos().x + mUserInterfaceManager->GetViewportWidth() / 2;
                mLastY = mUserInterfaceManager->GetWindowPos().y + mUserInterfaceManager->GetViewportHeight() / 2;
                glfwSetCursorPos(mWindowManager->GetGLFWWindow(), mLastX, mLastY);
            }
        }
    }
}

void ActorController::HandleKeyboard(int _key, int _scanCode, int _action, int _mods)
{
    // if there is no actor being controlled end function
	if (!mControlledActor) return;

    // otherwise assign keys in the keymap to be updated if they are pressed
	if (_action == GLFW_PRESS || _action == GLFW_REPEAT)
		mKeyStates[_key] = true;
	
	else if (_action == GLFW_RELEASE)
		mKeyStates[_key] = false;
}

void ActorController::HandleChar(unsigned codePoint)
{

}

void ActorController::SnapCameraToControlledActor(std::shared_ptr<CameraActor> _camRef)
{
    if (!mControlledActor || !_camRef) return;

    mCameraForSnap = _camRef;
    _camRef->SetSnappedToActor(mControlledActor);
}

void ActorController::UnSnapCameraToControlledActor()
{
    if (!mCameraForSnap)
    {
    	return;
    }

    mCameraForSnap->SetSnappedToActor(nullptr);
    mCameraForSnap = nullptr;
}

void ActorController::ExitApplication()
{
	glfwSetWindowShouldClose(mWindowManager->GetGLFWWindow(), true);
}


void ActorController::UpdateControls(float _dt)
{
    UIInput(_dt);
    CameraInput(_dt);
    ActorInput(_dt);
}

void ActorController::UIInput(float _dt)
{
    // UI stuff
    if (!mWindowManager) return;

    // Exit application
    if (mKeyStates[GLFW_KEY_ESCAPE])
		ExitApplication();
	
    // Set window mode
    static bool windowedPressOnce = true;
    if (mKeyStates[GLFW_KEY_F9] && windowedPressOnce) {
        windowedPressOnce = false;
        mWindowManager->SetWindowed();
    }
    else if (!mKeyStates[GLFW_KEY_F11] && !windowedPressOnce) {
        windowedPressOnce = true;
    }

    // Set boarderLess window mode
    static bool BoarderLessPressOnce = true;
    if (mKeyStates[GLFW_KEY_F10] && BoarderLessPressOnce) {
        BoarderLessPressOnce = false;
        mWindowManager->SetBoarderLessWindow();
    }
    else if (!mKeyStates[GLFW_KEY_F10] && !BoarderLessPressOnce) {
        BoarderLessPressOnce = true;
    }

    // Set fullscreen window mode
    static bool fullscreenPressOnce = true;
    if (mKeyStates[GLFW_KEY_F11] && fullscreenPressOnce) {
        fullscreenPressOnce = false;
        mWindowManager->SetFullScreen();
    } else if (!mKeyStates[GLFW_KEY_F11] && !fullscreenPressOnce) {
        fullscreenPressOnce = true;
    }


}

void ActorController::CameraInput(float _dt)
{
	if (std::shared_ptr<CameraActor> camActor = std::dynamic_pointer_cast<CameraActor>(mControlledActor))
    {
        // Gets the acceleration for update
        glm::vec3 acceleration = camActor->GetAcceleration();
        const float movementSpeed = camActor->GetAccelerationSpeed();
        acceleration = glm::vec3(0.f);

        // Update camera acceleration
        if (mKeyStates[GLFW_KEY_W]) acceleration.z += movementSpeed;
        if (mKeyStates[GLFW_KEY_S]) acceleration.z -= movementSpeed;
        if (mKeyStates[GLFW_KEY_D]) acceleration.x += movementSpeed;
        if (mKeyStates[GLFW_KEY_A]) acceleration.x -= movementSpeed;
        if (mKeyStates[GLFW_KEY_E]) acceleration.y += movementSpeed;
        if (mKeyStates[GLFW_KEY_Q]) acceleration.y -= movementSpeed;
    
        // Sets the camera acceleration
        camActor->SetAcceleration(acceleration);
    }
}

void ActorController::ActorInput(float _dt)
{
    // Controlling actor that has a camera snapped to it
    // ----------------------------------------------------------------
    if(mControlledActor && mCameraForSnap) {
        // Updates actor position if any of these keys are pressed
        if (mKeyStates[GLFW_KEY_W]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() + (mCameraForSnap->GetFront() *_dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_S]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() - (mCameraForSnap->GetFront() * _dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_D]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() + (mCameraForSnap->GetRight() * _dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_A]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() - (mCameraForSnap->GetRight() * _dt * mMovementSpeed));

        if (mControlledActor->GetPhysicsComponent())
            if (!mControlledActor->GetPhysicsComponent()->IsGravityEnabled())
            {
                if (mKeyStates[GLFW_KEY_Q]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() + (mCameraForSnap->GetUp() * _dt * mMovementSpeed));
                if (mKeyStates[GLFW_KEY_E]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() - (mCameraForSnap->GetUp() * _dt * mMovementSpeed));

            }
            else {

                if (mKeyStates[GLFW_KEY_SPACE])
                    mControlledActor->GetPhysicsComponent()->Jump();
            }


        // Only controlling actor
        // ----------------------------------------------------------------
    } else if (mControlledActor) {
        // Updates actor position if any of these keys are pressed
        if (mKeyStates[GLFW_KEY_W]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() + (glm::vec3(0.f, 0.f, -1.0f) * _dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_S]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() - (glm::vec3(0.0f, 0.f, -1.0f) * _dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_D]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() + (glm::vec3(1.0f, 0.f, 0.f) * _dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_A]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() - (glm::vec3(1.0f, 0.f, 0.f) * _dt * mMovementSpeed));

        if (mControlledActor->GetPhysicsComponent())
            if (!mControlledActor->GetPhysicsComponent()->IsGravityEnabled())
            {
                if (mKeyStates[GLFW_KEY_Q]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() + (glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed));
                if (mKeyStates[GLFW_KEY_E]) mControlledActor->SetGlobalPosition(mControlledActor->GetGlobalPosition() - (glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed));

            }
            else {

                if (mKeyStates[GLFW_KEY_SPACE])
                    mControlledActor->GetPhysicsComponent()->Jump();
            }
    }


	
}
