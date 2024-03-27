#pragma once
#include <string>



class Window
{
public:
	// ---------- Global Variables --------------

private:
	// ---------- Local Variables --------------

	std::string mName;
	int mWidth, mHeight;

	class Scene* mScene{ nullptr };
	struct GLFWwindow* mGLFWWindow{ nullptr };

public:
	// ---------- Global functions --------------

	Window(std::string name, int width, int height);
	~Window();

	// Init
	void Init();
	void RegisterWindowCallbacks();
	bool LoadContent(Scene* _scene);

	// Update
	void StartFrame();
	void Update(float dt);
	void Render(float dt);
	void EndFrame();

	bool IsClosed();

	// Callbacks
	void FramebufferSizeCallback(struct GLFWwindow* window, int width, int height);
	void MouseMoveCallback(struct GLFWwindow* window, double xpos, double ypos);
	void MouseButtonCallback(struct GLFWwindow* window, int button, int action, int mods);
	void MouseScrollCallback(struct GLFWwindow* window, double xoffset, double yoffset);
	void CharCallback(GLFWwindow* window, unsigned int codepoint);
	void KeyCallback(struct GLFWwindow* window, int key, int scancode, int action, int mods);


private:
	// ---------- Local functions --------------



public:
	// ---------- Getters / setters / Adders --------------

	// Setters
	void SetScene(Scene* _scene) { mScene = _scene; }
	void SetWidth(int _width) { mWidth = _width; }
	void SetHeight(int _height) { mHeight = _height; }

	// Getters
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	struct GLFWwindow* GetGLFWWindow() { return mGLFWWindow; }
};

