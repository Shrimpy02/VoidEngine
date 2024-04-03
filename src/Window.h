#pragma once
#include <string>

/**
 * @class Window,
 * @brief Not to be confused with GLFWWindow that acts as the viewport of the glfw application,
 * is the class that manages the GLFWWindow + all callbacks to it, and what scene is loaded and being rendered. 
 */
class Window
{
public:
	// ---------- Global Variables --------------

private:
	// ---------- Local Variables --------------

	std::string mName;
	int mWidth, mHeight;
	struct GLFWwindow* mGLFWWindow{ nullptr };
	class Scene* mScene{ nullptr };

public:
	// ---------- Global functions --------------

	// Constructor
	Window(std::string _name, int _width, int _height);

	// Disable copying and deleting of this class
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = default;
	Window& operator=(Window&&) = default;

	// De-constructor
	~Window();


	// Initialize`s GLFW-Window, GLAD & ImGui
	void Init();
	// Register all callbacks for GLFW & ImGui
	void RegisterWindowCallbacks();
	// Loads the given scene
	bool LoadContent(Scene* _scene);

	// Update
	// First frame of the render loop
	void StartFrame();
	// Update logic per frame
	void Update(float _dt);
	// Render logic per frame
	void Render(float _dt);
	// Last frame of the render loop
	void EndFrame();

	// Checks if the current window is closed
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

	// Setters -----------
	// Sets active scene
	void SetScene(Scene* _scene) { mScene = _scene; }
	// Sets height of the window
	void SetWidth(int _width) { mWidth = _width; }
	// Sets width of the window
	void SetHeight(int _height) { mHeight = _height; }

	// Getters
	// Get`s the window height
	int GetHeight() const { return mHeight; }
	// Get`s the window width
	int GetWidth() const { return mWidth; }

	// Get`s the GLFWWindow reference for the viewport window
	GLFWwindow* GetGLFWWindow() { return mGLFWWindow; }
};

