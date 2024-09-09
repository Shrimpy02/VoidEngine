//
//// Includes
//#include <Controllers/UIController.h>
//#include <Core/WindowManager.h>
//
//// Additional Includes
//#include <GLFW/glfw3.h>
//
//UIController::UIController(std::shared_ptr<WindowManager> _window)
//    : mWindowManager(_window)
//{
//}
//
//void UIController::ProcessInput(float _dt)
//{
//    // shuts down glfw if escape is pressed
//    if (mKeyStates[GLFW_KEY_ESCAPE] && mWindowManager)
//        glfwSetWindowShouldClose(mWindowManager->GetGLFWWindow(), true);
//}
//
//void UIController::HandleMouseMove(double _xPos, double _yPos)
//{
// 
//}
//
//void UIController::HandleMouseScroll(double _xOffset, double _yOffset)
//{
//}
//
//void UIController::HandleMouseButton(int _button, int _action, int _mods)
//{
//
//}
//
//void UIController::HandleKeyboard(int _key, int _scanCode, int _action, int _mods)
//{
//    // Otherwise assign keys in the keymap to be updated if they are pressed
//    if (_action == GLFW_PRESS || _action == GLFW_REPEAT)
//        mKeyStates[_key] = true;
//
//    else if (_action == GLFW_RELEASE)
//        mKeyStates[_key] = false;
//}
//
//void UIController::HandleChar(unsigned codePoint)
//{
//}
