#pragma once

// Includes
// Additional Includes
#include <string>
#include <memory>

// Forward Declarations
struct GLFWwindow;
class LevelManager;
class UserInterfaceManager;

enum WindowMode
{
	WM_Windowed,
	WM_BoarderLess,
	WM_FullScreen
};

/**
 * @class WindowManager,
 * @brief Not to be confused with GLFWWindow that acts as the viewport of the glfw application.
 * This class WindowManager manages the GLFWWindow + all callbacks to it and how they are passed to the interface manager.
 */
class WindowManager : public std::enable_shared_from_this<WindowManager>
{
public:
	// ---------- Global Variables --------------
private:
	// ---------- Local Variables --------------

	std::string mWindowName;
	int mWidth = 1980, mHeight = 1020;
	const int mWindowTopBarOffset = 30;
	int mWindowedWidth = 1980;
	int mWindowedHeight = 1020;
	int mWindowedPosX = 0;
	int mWindowedPosY = 0;
	GLFWwindow* mGLFWWindow{ nullptr };
	std::shared_ptr<UserInterfaceManager> mUserInterfaceManager{ nullptr };

	// For Window Toggling ---
	WindowMode mWindowMode = WindowMode::WM_Windowed;

public:
	// ---------- Global functions --------------

	explicit WindowManager(std::string _name);

	WindowManager(const WindowManager&) = delete;
	WindowManager& operator=(const WindowManager&) = delete;
	WindowManager(WindowManager&&) = default;
	WindowManager& operator=(WindowManager&&) = default;

	~WindowManager();

	// Init -----------

	// Initialize`s GLFW-Window, GLAD & ImGui
	void Init();

	// Register all callbacks for GLFW & ImGui
	void RegisterWindowCallbacks();

	// Loads the given scene
	void LoadContent();

	// Update -----------

	// Apps start frame of the render/update loop
	void StartFrame();

	// Apps update frame of the render/update loop
	void Update(float _dt);

	// Apps render frame of the render/update loop
	void Render();

	// Apps end frame of the render/update loop
	void EndFrame();

	// Checker -----------

	// Returns true if glfwWindowShouldClose is true,
	// IE if the window should close return true
	bool IsClosed();

	void SetWindowed();

	void SetBoarderLessWindow();

	void SetFullScreen();





private:
	// ---------- Local functions --------------

	// Callback`s -----------

	void FrameBufferSizeCallback(GLFWwindow* _window,int _width, int _height);
	void MouseMoveCallback(GLFWwindow* _window, const double _xPos, const double _yPos);
	void MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods);
	void MouseScrollCallback(GLFWwindow* _window, double _xOffset, double _yOffset);
	void CharCallback(GLFWwindow* _window, unsigned int _codepoint);
	void KeyCallback(GLFWwindow* _window, int _key, int _scanCode, int _action, int _mods);

public:
	// ---------- Getters / setters / Adders --------------

	// Getters -----------

	// Get`s the GLFWWindow reference for the viewport window
	GLFWwindow* GetGLFWWindow() { return mGLFWWindow; }
	// Get`s the window height
	int GetHeight() const { return mHeight; }
	// Get`s the window width
	int GetWidth() const { return mWidth; }
	// Get`s the window mode of GLFW 
	WindowMode GetWindowMode() const { return mWindowMode; }

	// Setters -----------
	
	// Sets height of the window
	void SetWidth(int _width) { mWidth = _width; }
	// Sets width of the window
	void SetHeight(int _height) { mHeight = _height; }
};

