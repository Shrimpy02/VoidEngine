#pragma once
// Includes


// Additional Includes
#include <memory> 


// Forward Declarations
class Window;

/**
 * @class IController,
 * @brief Interface for the controller object.
 * Contains base functions for handling mouse movement, scroll, click and keyboard action.
 */
class IController
{
public:
    virtual ~IController() = default;

    virtual void Update(float dt) {}
    virtual void HandleMouseMove(std::shared_ptr<Window> _window, double _xpos, double _ypos) = 0;
    virtual void HandleMouseScroll(std::shared_ptr<Window> _window, double _xoffset, double _yoffset) = 0;
    virtual void HandleMouseButton(std::shared_ptr<Window> _window, int _button, int _action, int _mods) = 0;
    virtual void HandleKeyboard(std::shared_ptr<Window> _window, int _key, int _scancode, int _action, int _mods) = 0;
};