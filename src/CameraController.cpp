#include "CameraController.h"
#include <Camera.h>
#include <Window.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Window.h>

#include "Scene.h"

void CameraController::Update(float _dt)
{
    UpdateCameraAcceleration();
}

void CameraController::HandleMouseMove(Window* _window, double _xpos, double _ypos)
{
    if (!mRightMousePressed) return;

    float xoffset = mLastX - (float)_xpos;
    float yoffset = mLastY - (float)_ypos;

    mLastX = (float)_xpos;
    mLastY = (float)_ypos;

    xoffset *= mMouseSensitivity;
    yoffset *= mMouseSensitivity;

    auto angularSpeed = mCamera->GetAngularAccelerationSpeed();

    mCamera->AddAngularAcceleration({ xoffset * angularSpeed, yoffset * angularSpeed });
}

void CameraController::HandleMouseScroll(Window* _window, double _xoffset, double _yoffset)
{
}

void CameraController::HandleMouseButton(Window* _window, int _button, int _action, int _mods)
{
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

void CameraController::HandleKeyboard(Window* _window, int _key, int _scancode, int _action, int _mods)
{
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
    auto acceleration = mCamera->GetAcceleration();
    float movementSpeed = mCamera->GetAccelerationSpeed();

    acceleration = glm::vec3(0.f);

    if (mKeyStates[GLFW_KEY_W]) acceleration.z += movementSpeed;
    if (mKeyStates[GLFW_KEY_S]) acceleration.z -= movementSpeed;
    if (mKeyStates[GLFW_KEY_A]) acceleration.x -= movementSpeed;
    if (mKeyStates[GLFW_KEY_D]) acceleration.x += movementSpeed;
    if (mKeyStates[GLFW_KEY_Q]) acceleration.y -= movementSpeed;
    if (mKeyStates[GLFW_KEY_E]) acceleration.y += movementSpeed;

    if (mKeyStates[GLFW_KEY_ESCAPE] && mWindow)
        glfwSetWindowShouldClose(mWindow->GetGLFWWindow(), true);

    mCamera->SetAcceleration(acceleration);
}

