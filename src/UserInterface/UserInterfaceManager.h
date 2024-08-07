#pragma once

// Includes

// Additional Includes
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

// Forward Declarations
class Actor;
class Shader;
struct Level;
class IBounded;
class Light;
class IController;
class CameraActor;
class DebugActor;
class Window;

/**
 * @class UserInterfaceManager
 * @brief Class that manages all UI Logic
 */
class UserInterfaceManager
{
public:
    // ---------- Global Variables --------------


private:
    // ---------- Local Variables --------------

    // world
    std::shared_ptr<Shader> mDefaultShader{ nullptr };
    std::shared_ptr<Level> mActiveLevel{ nullptr };
    std::shared_ptr<IController> mController{ nullptr };
    std::shared_ptr<DebugActor> mBarycentricDebugActor{ nullptr };


    // // ImGui UI Variables
     int mMainSelectionIndex = 0;
     int mOldSelectionIndex = 0;
     int mCameraSelectionIndex = 0;
     const float mItemWidth = 80.0f;
     int mComponentSelectionIndex = 0;
     
     
     float mCurrentUniformScale = 1;
     float mOldUniformScale = 1;
     glm::vec3 mActorOriginalScale = glm::vec3(0);
     
     bool mShouldShowWireFrame = false;
     bool mShouldDrawCollisionDebugMesh = true;
     bool mCanControlActor = false;
     bool mIsUniformScale = false;
     bool mSnapCameraToActor = false;
     bool mShowBarycentricLocation = false;
     
     double mOldTime = 0;
     int numFrames = 0;

public:
    // ---------- Global functions --------------
    explicit UserInterfaceManager();

    // Removes the ability to:    
    UserInterfaceManager(const UserInterfaceManager&) = delete;           // Copy
    UserInterfaceManager& operator=(const UserInterfaceManager&) = delete;// Copy ref
    UserInterfaceManager(UserInterfaceManager&&) = delete;                // Move
    UserInterfaceManager& operator=(UserInterfaceManager&&) = delete;     // Move ref
    // Because this class is explicit.
    ~UserInterfaceManager();

    // Function`s
    // ------------------------------------------------------------

    void ImguiInit(GLFWwindow* _glfwWindow);

    void ImguiStartFrame();

    void ImguiEndFrame();


	//// Local scene UIUpdate function for distribution (called each frame) 
    void UpdateUI();
    //// Local scene UIRender function for distribution (called each frame) 
    void RenderUI();

private:
    // ---------- Local functions --------------



    void ui_WorldObjects();

    void ui_ObjectProperties();

    void ui_docktest();

    //// Contains all custom logic for the world ImGui window
    void imgui_WorldObjectSettings();
    //// Contains custom ImGui logic for details sub section
    void imguiSub_WorldDetails(std::shared_ptr<Actor> _aptr);
    //// Contains custom ImGui logic for collision sub section
    void imguiSub_Collision(std::shared_ptr<IBounded> _cptr);
    //// Contains custom ImGui logic for light sub section
    void imguiSub_Light(std::shared_ptr<Light> _lptr);
    //// Contains custom ImGui logic for camera sub section
    void imguiSub_Camera(std::shared_ptr<CameraActor> _captr);
    //// Contains all custom logic for the DebugLogger ImGui window
    void imgui_Logger();
    //// Contains custom ImGui logic for FPS sub section
    void imguiSub_FPS();

    void imgui_ContentBrowser();

    void imgui_SceneItems();

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters
    void SetActiveLevel(std::shared_ptr<Level> _lvl) { mActiveLevel = _lvl; }
    void SetController(std::shared_ptr<IController> _controller) { mController = _controller; }
    void SetDefaultShader(std::shared_ptr<Shader> _inShader) { mDefaultShader = _inShader; }

    // Getters


};

