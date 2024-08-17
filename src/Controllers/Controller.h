#pragma once
// Includes


// Additional Includes
#include <memory> 


// Forward Declarations
class Window;
class Actor;
class CameraActor;

/**
 * @class IController,
 * @brief Interface for the controller object.
 * Contains base functions for handling mouse movement, scroll, click and keyboard action.
 */
class IController
{
public:
    virtual ~IController() = default;

    virtual void ProcessInput(float dt) {}
    virtual void HandleMouseMove(double _xPos, double _yPos) = 0;
    virtual void HandleMouseScroll(double _xOffset, double _yOffset) = 0;
    virtual void HandleMouseButton(int _button, int _action, int _mods) = 0;
    virtual void HandleKeyboard(int _key, int _scanCode, int _action, int _mods) = 0;
    virtual void HandleChar(unsigned int codePoint) = 0;
};