#pragma once

// Includes

// Additional Includes
#include <string>
#include <memory>

// Forward Declarations
struct GLFWwindow;
class LevelManager;
class UserInterfaceManager;

/**
 * @class Window,
 * @brief Not to be confused with GLFWWindow that acts as the viewport of the glfw application,
 * is the class that manages the GLFWWindow + all callbacks to it, and what scene is loaded and being rendered. 
 */
class Window : public std::enable_shared_from_this<Window>
{
public:
	// ---------- Global Variables --------------
private:
	// ---------- Local Variables --------------

	std::string mName;
	int mWidth, mHeight;
	GLFWwindow* mGLFWWindow{ nullptr };
	std::shared_ptr<LevelManager> mLevelManager{ nullptr };
	std::shared_ptr<UserInterfaceManager> mUserInterfaceManager{ nullptr };

public:
	// ---------- Global functions --------------

	// Constructor
	Window(std::string _name, int _width, int _height, std::shared_ptr<UserInterfaceManager> _inInterfaceManager);

	// Disable copying and deleting of this class
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = default;
	Window& operator=(Window&&) = default;

	// De-constructor for glfw and imgui shutdown
	~Window();

	// Initialize`s GLFW-Window, GLAD & ImGui
	void Init();
	// Register all callbacks for GLFW & ImGui
	void RegisterWindowCallbacks();
	// Loads the given scene
	bool LoadContent(std::shared_ptr<LevelManager> _levelManager);

	// Update ---

	void StartFrame();
	void Update(float _dt);
	void Render(float _dt);
	void EndFrame();

	bool IsClosed();

	// Callbacks
	void FramebufferSizeCallback(GLFWwindow* _window, int _width, int _height);
	void MouseMoveCallback(GLFWwindow* _window, double _xpos, double _ypos);
	void MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods);
	void MouseScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset);
	void CharCallback(GLFWwindow* _window, unsigned int _codepoint);
	void KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);

private:
	// ---------- Local functions --------------



public:
	// ---------- Getters / setters / Adders --------------

	// Getters -----------

	// Get`s the window height
	int GetHeight() const { return mHeight; }
	// Get`s the window width
	int GetWidth() const { return mWidth; }
	// Get`s the GLFWWindow reference for the viewport window
	GLFWwindow* GetGLFWWindow() { return mGLFWWindow; }

	// Setters -----------

	// Sets active scene
	void SetLevelManager(std::shared_ptr<LevelManager> _scene) { mLevelManager = _scene; }
	// Sets height of the window
	void SetWidth(int _width) { mWidth = _width; }
	// Sets width of the window
	void SetHeight(int _height) { mHeight = _height; }
};

