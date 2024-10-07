
// Includes
#include <Controllers/ActorController.h>
#include <Core/WindowManager.h>
#include <UserInterface/UserInterfaceManager.h>
#include <Levels/LevelManager.h>
#include <Components/PhysicsComponent.h>
#include <LevelActors/CameraActor.h>

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

    // Get the current window position and size of the viewport
    ImVec2 windowPos = mUserInterfaceManager->GetViewportPosition();
    int viewportWidth = (int)mUserInterfaceManager->GetViewportWidth();
    int viewportHeight = (int)mUserInterfaceManager->GetViewportHeight();

    // Calculate the center of the viewport
    float centerX = windowPos.x + viewportWidth / 2.0f;
    float centerY = windowPos.y + viewportHeight / 2.0f;

    // checks with last x and y for consistent mouse movement
    float xOffset = mLastX - static_cast<float>(_xPos);
    float yOffset = mLastY - static_cast<float>(_yPos);

    // applies mouse sensitivity 
    xOffset *= mMouseSensitivity;
    yOffset *= mMouseSensitivity;

    mLastX = static_cast<float>(_xPos);
    mLastY = static_cast<float>(_yPos);

    if(std::shared_ptr<CameraActor> camActor = std::dynamic_pointer_cast<CameraActor>(mControlledActor))
    {
        // gets teh angular speed for a smooth camera rotation based on mouse movement
        auto angularSpeed = camActor->GetAngularAccelerationSpeed();

        // Adds the acceleration to the camera for it to rotate
        camActor->AddAngularAcceleration({ xOffset * angularSpeed, yOffset * angularSpeed});

    } else if(mCameraForSnap) {

        // gets teh angular speed for a smooth camera rotation based on mouse movement
        auto angularSpeed = mCameraForSnap->GetAngularAccelerationSpeed();

        // Adds the acceleration to the camera for it to rotate
        mCameraForSnap->AddAngularAcceleration({ xOffset * angularSpeed, yOffset * angularSpeed});
    }

    // Reposition the cursor to the center of the viewport if it reaches an edge
    bool resetCursor = false;
    if (_xPos <= windowPos.x || _xPos >= windowPos.x + viewportWidth)
    {
        resetCursor = true;
        mLastX = centerX;
    }
    if (_yPos <= windowPos.y + 20 || _yPos >= windowPos.y + viewportHeight)
    {
        resetCursor = true;
        mLastY = centerY;
    }

    if (resetCursor)
    {
        glfwSetCursorPos(mWindowManager->GetGLFWWindow(), centerX, centerY);
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
                mLastX = mUserInterfaceManager->GetViewportPosition().x + mUserInterfaceManager->GetViewportWidth() / 2;
                mLastY = mUserInterfaceManager->GetViewportPosition().y + mUserInterfaceManager->GetViewportHeight() / 2;
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
                mLastX = mUserInterfaceManager->GetViewportPosition().x + mUserInterfaceManager->GetViewportWidth() / 2;
                mLastY = mUserInterfaceManager->GetViewportPosition().y + mUserInterfaceManager->GetViewportHeight() / 2;
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

void ActorController::HandleViewportClick(int _button, int _action, int _mods, double _cursorPosX, double _cursorPosY)
{
    std::shared_ptr<CameraActor> camera = std::dynamic_pointer_cast<CameraActor>(mControlledActor);
    if (!camera) return;

    static bool doOnce = true;

    if (_button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (_action == GLFW_PRESS)
        {
            if (mUserInterfaceManager && mLevelManager)
            {
               if(doOnce)
               {
                   // Get the viewport and cursor dimensions 
                   int viewportWidth = mUserInterfaceManager->GetViewportWidth();
                   int viewportHeight = mUserInterfaceManager->GetViewportHeight();
                   ImVec2 viewportPos = mUserInterfaceManager->GetViewportPosition();
                   ImVec2 cursorPos = mUserInterfaceManager->GetCursoPosition();
                   ImVec2 relativeMousePos = ImVec2(cursorPos.x - viewportPos.x, cursorPos.y - viewportPos.y);

               		// Convert mouse position from screen space to NDC
                   float x = (2.0f * relativeMousePos.x) / viewportWidth - 1.0f;
                   float y = 1.0f - (2.0f * relativeMousePos.y) / viewportHeight;
                   glm::vec2 ndc = glm::vec2(x, y);

                   // Convert NDC to clip space
                   glm::vec4 clipSpacePos = glm::vec4(ndc, -1.0f, 1.0f);

                   // Convert clip space to view space
                   glm::mat4 invProjection = glm::inverse(camera->GetProjectionMatrix());
                   glm::vec4 viewSpacePos = invProjection * clipSpacePos;
                   viewSpacePos = glm::vec4(viewSpacePos.x, viewSpacePos.y, -1.0f, 0.0f);

                   // Convert view space to world space (ray direction)
                   glm::mat4 invView = glm::inverse(camera->GetViewMatrix());
                   glm::vec3 rayDirection = glm::normalize(glm::vec3(invView * viewSpacePos));
                   glm::vec3 rayOrigin = camera->GetGlobalPosition();

					// Project ray
                   if (std::shared_ptr<Actor> hitActor = mLevelManager->LineTrace(
                       rayOrigin,
                       rayDirection,
                       500,
                       100,
                       true))
                   {
                       if (mLeftClickOptions == LeftClickOptions::Select)
                           mUserInterfaceManager->SetContentSelectedActor(hitActor);

                       else if (mLeftClickOptions == LeftClickOptions::AddForce)
                           if (hitActor->GetPhysicsComponent())
                               hitActor->GetPhysicsComponent()->AddVelocity(mClickVelocity * rayDirection);

                   }
                   doOnce = false;
               }
            }
        }
        else if (_action == GLFW_RELEASE)
        {
            if (mUserInterfaceManager && mLevelManager)
            {
                doOnce = true;
            }
        }
    }
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
		const float movementSpeed = camActor->GetMovementSpeed();
    
        // Update camera acceleration
        if (mKeyStates[GLFW_KEY_W]) camActor->SetGlobalPosition(camActor->GetGlobalPosition() + (camActor->GetFront() * movementSpeed * _dt));
        if (mKeyStates[GLFW_KEY_S]) camActor->SetGlobalPosition(camActor->GetGlobalPosition() - (camActor->GetFront() * movementSpeed * _dt));
        if (mKeyStates[GLFW_KEY_D]) camActor->SetGlobalPosition(camActor->GetGlobalPosition() + (camActor->GetRight() * movementSpeed * _dt));
        if (mKeyStates[GLFW_KEY_A]) camActor->SetGlobalPosition(camActor->GetGlobalPosition() - (camActor->GetRight() * movementSpeed * _dt));
        if (mKeyStates[GLFW_KEY_E]) camActor->SetGlobalPosition(camActor->GetGlobalPosition() + (camActor->GetUp() * movementSpeed * _dt));
        if (mKeyStates[GLFW_KEY_Q]) camActor->SetGlobalPosition(camActor->GetGlobalPosition() - (camActor->GetUp() * movementSpeed * _dt));
    }
}

void ActorController::ActorInput(float _dt)
{
    if (std::shared_ptr<CameraActor> camActor = std::dynamic_pointer_cast<CameraActor>(mControlledActor)) return;

    // Controlling actor that has a camera snapped to it
    // ----------------------------------------------------------------
    if(mControlledActor && mCameraForSnap) {


    	// Updates actor position if any of these keys are pressed
        if (mKeyStates[GLFW_KEY_W]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() + (mCameraForSnap->GetFront() *_dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_S]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() - (mCameraForSnap->GetFront() * _dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_D]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() + (mCameraForSnap->GetRight() * _dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_A]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() - (mCameraForSnap->GetRight() * _dt * mMovementSpeed));

        if (mControlledActor->GetPhysicsComponent()){
            if (!mControlledActor->GetPhysicsComponent()->IsGravityEnabled())
            {
                if (mKeyStates[GLFW_KEY_Q]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() + (mCameraForSnap->GetUp() * _dt * mMovementSpeed));
                if (mKeyStates[GLFW_KEY_E]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() - (mCameraForSnap->GetUp() * _dt * mMovementSpeed));

            }
            else {

                if (mKeyStates[GLFW_KEY_SPACE])
                    mControlledActor->GetPhysicsComponent()->Jump();
            }
        } else {
            if (mKeyStates[GLFW_KEY_Q]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() + (glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed));
            if (mKeyStates[GLFW_KEY_E]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() - (glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed));
        }

        // Only controlling actor
        // ----------------------------------------------------------------
    } else if (mControlledActor) {
        // Updates actor position if any of these keys are pressed
        if (mKeyStates[GLFW_KEY_W]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() + (glm::vec3(0.f, 0.f, -1.0f) * _dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_S]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() - (glm::vec3(0.0f, 0.f, -1.0f) * _dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_D]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() + (glm::vec3(1.0f, 0.f, 0.f) * _dt * mMovementSpeed));
        if (mKeyStates[GLFW_KEY_A]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() - (glm::vec3(1.0f, 0.f, 0.f) * _dt * mMovementSpeed));

        if (mControlledActor->GetPhysicsComponent())
        {
            if (!mControlledActor->GetPhysicsComponent()->IsGravityEnabled())
            {
                if (mKeyStates[GLFW_KEY_Q]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() + (glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed));
                if (mKeyStates[GLFW_KEY_E]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() - (glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed));

            }
            else {

                if (mKeyStates[GLFW_KEY_SPACE])
                    mControlledActor->GetPhysicsComponent()->Jump();
            }
        } else {
            if (mKeyStates[GLFW_KEY_Q]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() + (glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed));
            if (mKeyStates[GLFW_KEY_E]) mControlledActor->SetLocalPosition(mControlledActor->GetLocalPosition() - (glm::vec3(0.0f, 1.f, 0.0f) * _dt * mMovementSpeed));
        }
        

    }


	
}
