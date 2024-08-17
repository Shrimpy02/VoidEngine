//#pragma once
//
//// Includes
//#include <Controllers/Controller.h>
//
//// Additional Includes
//#include <memory> 
//#include <map>
//
//// Forward Declarations
//class Actor;
//class WindowManager;
//class CameraActor;
//
///**
// * @class UIController,
// * @brief UIController inherits from IController giving it key input control.
// * It lets the user manage hot-keys and input for ui.
// */
//class UIController : public IController
//{
//public:
//    // ---------- Global Variables --------------
//
//private:
//    // ---------- Local Variables --------------
//
//    // Key map for input handeling
//    std::map<int, bool> mKeyStates;
//
//    // Window reference
//    std::shared_ptr<WindowManager> mWindowManager;
//
//public:
//    // ---------- Global functions --------------
//
//    // Constructor
//    UIController(std::shared_ptr<WindowManager> _window);
//
//    // Inherited update from IController, updates all local function each tick. 
//    void ProcessInput(float _dt) override;
//
//    // Inherited from IController, empty function
//    void HandleMouseMove(double _xPos, double _yPos) override;
//    // Inherited from IController, empty function
//    void HandleMouseScroll(double _xOffset, double _yOffset) override;
//    // Inherited from IController, empty function
//    void HandleMouseButton(int _button, int _action, int _mods) override;
//    // Inherited from IController, asigness key states to the keystates maps
//    void HandleKeyboard(int _key, int _scanCode, int _action, int _mods) override;
//    // Inherited from IController, empty function
//    void HandleChar(unsigned codePoint) override;
//
//private:
//    // ---------- Local functions --------------
//
//public:
//    // ---------- Getters / setters / Adders --------------
//
//    // Adders
//
//    // Setters
//    
//    // Getters
//
//};
//
