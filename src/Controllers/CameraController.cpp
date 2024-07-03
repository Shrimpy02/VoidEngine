
// class Includes
#include <Controllers/CameraController.h>
#include <Camera.h>
#include <Core/Window.h>

// Other
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void CameraController::Update(float _dt)
{
    UpdateCameraAcceleration();
}

void CameraController::HandleMouseMove(std::shared_ptr<Window> _window, double _xpos, double _ypos)
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
    auto angularSpeed = mCamera->GetAngularAccelerationSpeed();

    // Adds the acceleration to the camera for it to rotate
    mCamera->AddAngularAcceleration({ xoffset * angularSpeed, yoffset * angularSpeed });
}

void CameraController::HandleMouseScroll(std::shared_ptr<Window> _window, double _xoffset, double _yoffset)
{
    // no scroll funcionality yet
}

void CameraController::HandleMouseButton(std::shared_ptr<Window> _window, int _button, int _action, int _mods)
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

void CameraController::HandleKeyboard(std::shared_ptr<Window> _window, int _key, int _scancode, int _action, int _mods)
{
    // Binds key to key map and changes the bool based on if key is down or not
    if (_action == GLFW_PRESS || _action == GLFW_REPEAT)
    {
        mKeyStates[_key] = true;
    }
    else if (_action == GLFW_RELEASE)
    {
        mKeyStates[_key] = false;
    }
}

void CameraController::UpdateCameraAcceleration()
{
    // Gets the acceleration for update
    auto acceleration = mCamera->GetAcceleration();
    float movementSpeed = mCamera->GetAccelerationSpeed();

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
    mCamera->SetAcceleration(acceleration);
}

