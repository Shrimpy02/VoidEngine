#pragma once

// Includes

// Additional Includes
#include <glm/vec3.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <filesystem>

// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

struct FileItem
{
    std::string mName;
    bool mIsDirectory;
};

// Forward Declarations
class WindowManager;
class ActorController;
class Texture;
class Mesh;

class Actor;
class Shader;
struct Level;
class IBounded;
class Light;

class CameraActor;
class DebugActor;
class LevelManager;

/**
 * @class UserInterfaceManager
 * @brief Class that manages all UI Logic
 */
class UserInterfaceManager : public std::enable_shared_from_this<UserInterfaceManager>
{
public:
    // ---------- Global Variables --------------


private:
    // ---------- Local Variables --------------

    // world
    std::shared_ptr<WindowManager> mWindowManager{ nullptr };
    std::shared_ptr<LevelManager> mLevelManager{ nullptr };
    std::shared_ptr<ActorController> mController{ nullptr };

    // Menu Bar --------
    std::shared_ptr<Texture> mLogoIcon{ nullptr };
    ImVec2 mIconSize{ 16,16 };

    // Create Actors ----------
    bool mIsCreateActorWindowOpen = true;
    const char* mActorNames[6] = { "Cube", "Pyramid", "Plane", "Sphere","Directional-Light", "Point-Light"};
    const int mNumActorItems = (int)std::size(mActorNames);

    // File Explorer ------
    bool mIsFileExplorerWindowOpen = true;
    std::string mCurrentDirectoryPath = GetFilePath();
    int mMaxDisplayCharacters = 16;
    const float mItemIconSize = 130.0f;
    const float mItemIconSpacing = 5.0f;
    std::shared_ptr<Texture> mFolderIcon{ nullptr };
    std::shared_ptr<Texture> mJPGIcon{ nullptr };
    std::shared_ptr<Texture> mPNGIcon{ nullptr };
    std::shared_ptr<Texture> mFBXIcon{ nullptr };
    std::shared_ptr<Texture> mLVLIcon{ nullptr };
    std::shared_ptr<Texture> mErrorIcon{ nullptr };
    // Image uv coords
    // This is for Speilvent: ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)
    // This is for upside down: ImVec2(1.0f, 1.0f), ImVec2(0.0f, 0.0f)

    // Viewport ----------
    bool mIsViewportWindowOpen = true;
    GLuint mViewportTexture = 0;
    GLuint mFrameBuffer = 0;
    GLuint mDepthBuffer = 0;
    ImVec2 mViewportPos = ImVec2(0,0);
    ImVec2 mMouseCursorPosition = ImVec2(0, 0);
    int mViewportWidth = 960;
    int mViewportHeight = 540;
    bool mViewportHasFocusAndHover = false;
    bool mViewportHasHover = false;

    // World Properties ------
    bool mIsWorldPropertiesWindowOpen = true;
    std::shared_ptr<Shader> mDefaultShader{ nullptr };
    bool mWorldGravityEnabled = false;

    // Debug Properties ------
    bool mIsDebugPropertiesWindowOpen = true;

    // ContentBrowser ----------
    bool mIsContentBrowserWindowOpen = true;
    std::shared_ptr<Actor> mContentSelectedActor{ nullptr };
    std::shared_ptr<Actor> mOldContentSelectedActor{ nullptr };
    const float mItemWidth = 60.0f;
    bool mCanControlActor = false;
    bool mOpenRenamePopup = false;

    // Content properties ------
    bool mIsContentPropertiesWindowOpen = true;
    bool mSnapCameraToActor = false;

    // Sub world properties -----
    std::shared_ptr<DebugActor> mBarycentricDebugActor{ nullptr };
    bool mIsUniformScale = false;
    float mCurrentUniformScale = 1;
    float mOldUniformScale = 1;
    glm::vec3 mActorOriginalScale = glm::vec3(0);
    bool mShowBarycentricLocation = false;

    // Sub Mesh properties -----
    bool mShowMesh = true;

    // Sub component properties ----
    int mComponentSelectionIndex = 0;
    bool mEnableGravity = false;

    // Debug properties -----
    double mOldTime = 0;
    int numFrames = 0;
    bool mShouldDrawCollisionDebugMesh = false;

    // World properties ---
	bool mShouldShowWireFrame = false;

    // Log ------
    bool mIsLogWindowOpen = true;

    // Console ------
    bool mIsConsoleWindowOpen = true;
 

public:
    // ---------- Global functions --------------
    explicit UserInterfaceManager(std::shared_ptr<WindowManager> _inWindowManager);

    UserInterfaceManager(const UserInterfaceManager&) = delete;           
    UserInterfaceManager& operator=(const UserInterfaceManager&) = delete;
    UserInterfaceManager(UserInterfaceManager&&) = delete;                
    UserInterfaceManager& operator=(UserInterfaceManager&&) = delete;     

    ~UserInterfaceManager();

    // Init -----------

    // Initialize userInterface and levelManager
    void Init(GLFWwindow* _glfwWindow);


    // Loads UI and default level content
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

    // Callback`s ------------

    void FrameBufferCallback(GLFWwindow* _window, int _width, int _height);
    void MouseMoveCallback(GLFWwindow* _window, double _xpos, double _ypos);
    void MouseScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset);
    void CharCallback(GLFWwindow* _window, unsigned int _codepoint);
    void KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);
    void MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods, double _cursorPosX, double _cursorPosY);

    // Checkers --------------

    bool IsViewportFocusedAndHovered();

private:
    // ---------- Local functions --------------

	// Initialize ImGui settings, flags and style
    void ImGuiInit(GLFWwindow* _glfwWindow);

    void InitImages();

    void CreateViewportTexture();

    // Local scene UIRender function for distribution (called each frame) 
    void RenderUI();

    void RenderLevelToTexture();

    void ResizeFrameBuffer(int newWidth, int newHeight);

    ImVec2 CalculateAspectRatioSize(ImVec2 availableSize, float aspectRatio);


    // UI Windows

    void ui_DockSpace();

    void ui_Sub_MainMenuBar();

    void ui_CreateActors();

    void ui_FileExplorer();

    void ui_ViewPort();

    void ui_LevelSelect();

    void ui_WorldProperties();

    void ui_DebugProperties();

    void ui_ContentBrowser();

    void ui_ContentProperties();

    void uiSub_WorldProperties(std::shared_ptr<Actor> _inActor);

    void uiSub_MeshProperties(std::shared_ptr<Mesh> _inMesh);

	void uiSub_ComponentProperties(std::shared_ptr<Actor> _inActor);

    void uiSub_CameraProperties(std::shared_ptr<CameraActor> _inCamera);

    void uiSub_CollisionProperties(std::shared_ptr<IBounded> _inCollision);

    void uiSub_LightProperties(std::shared_ptr<Light> _inLight);

    void ui_Log();

    void ui_Console();

    // ----

    void ui_Element_Image(std::shared_ptr<Texture> _image, ImVec2 _imageSize, ImVec2 _uv0 = ImVec2(1.0f, 1.0f), ImVec2 _uv1 = ImVec2(0.0f, 0.0f),bool _centreVertically = true);

    bool HasSuffix(const std::string& _fileName, const std::string& _suffix);

    void DrawBoarderAroundImage(const ImVec4 _color = ImVec4(0.5f, 0.0f, 0.5f, 1.0f));



    void GetSceneActors(std::shared_ptr<Actor> _sceneGraph, std::vector<std::shared_ptr<Actor>>& _actorHolder);

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    void SetContentSelectedActor(std::shared_ptr<Actor> _inActor) { mContentSelectedActor = _inActor; }

    // Getters
    std::string GetFilePath();
    std::string GetCoreFilePath();
    int GetViewportWidth() { return mViewportWidth; }
    int GetViewportHeight() { return mViewportHeight; }
    ImVec2 GetViewportPosition() { return mViewportPos;  }
    ImVec2 GetCursoPosition() { return mMouseCursorPosition; }
    std::vector<FileItem> GetDirectoryContents(const std::string& _path);
};

