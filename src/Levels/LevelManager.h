#pragma once
// Includes
#include <Transform.h>

// Additional Includes
#include <memory> 
#include <vector>

// Forward Declarations
struct Level;
class Window;
class Shader;
class ActorController;
class CameraController;
class IController;
class Actor;
struct Transform;

/**
 * @class LevelManager
 * @brief 
 */
class LevelManager
{
public:
    // ---------- Global Variables --------------


private:
    // ---------- Local Variables --------------

    std::vector<std::shared_ptr<Level>> mAllLevels;
    std::shared_ptr<Level> mActiveLevel;

    std::shared_ptr<Window> mWindow{ nullptr };

    std::shared_ptr<ActorController> mActorController{ nullptr };
    std::shared_ptr<CameraController> mCameraController{ nullptr };
    std::shared_ptr<IController> mActiveController{ nullptr };

    std::shared_ptr<Shader> mShader{ nullptr };

   // // ImGui UI Variables
   // int mMainSelectionIndex = 0;
   // int mOldSelectionIndex;
   // const float mItemWidth = 80.0f;
   // int mComponentSelectionIndex = 0;
   // 
   // 
   // float mCurrentUniformScale = 1;
   // float mOldUniformScale = 1;
   // //glm::vec3 mActorOriginalScale;
   // 
   // bool mShouldShowWireFrame = false;
   // bool mShouldDrawCollisionDebugMesh = true;
   // bool mCanControlActor = false;
   // bool mIsUniformScale = false;
   // bool mSnapCameraToActor = false;
   // 
   // double mOldTime = 0;
   // int numFrames = 0;

public:
    // ---------- Global functions --------------
    explicit LevelManager(std::shared_ptr<Window> _window);

    // Removes the ability to:    
    LevelManager(const LevelManager&) = delete;           // Copy
    LevelManager& operator=(const LevelManager&) = delete;// Copy ref
    LevelManager(LevelManager&&) = delete;                // Move
    LevelManager& operator=(LevelManager&&) = delete;     // Move ref
    // Because this class is explicit.
    ~LevelManager();

    // Function`s
    // ------------------------------------------------------------

    // Loads all objects, their materials and meshes and assigns them as a child of mSceneGraph so they
    // receive update and render calls if they inherit from the correct classes.
    void LoadContent();

    void LoadDefaultLevel();

	// Deletes pointers and clears all caches for texture, mesh and material.  
    void UnloadContent();

    void SetActiveLevel(std::shared_ptr<Level> _activeLevel); 

    // Local scene update function for distribution (called each frame) 
    void Update(float _dt);
    // Local scene render function for distribution (called each frame) 
    void Render(float _dt);


   // // Scene collision handler function for all scene objects that inherit from "IBounded" (called each frame) 
   // void HandleCollision();
   // // Template helper function for HandleCollision, takes two template arguments for either aabb
   // // or BoundingSphere in addition to who it belongs and executes the appropriate logic for them.
   // // Returns true if there was a collision and false otherwise.
   // template <typename T, typename U>
   // bool ProcessCollision(T _a, U _b, IBounded* _iA, IBounded* _iB, Actor* _AA, Actor* _AB);

	// Updates the scene graph and all children if they inherit from "Actor" (called each frame) 
	void UpdateLevelSceneGraph(std::shared_ptr<Actor> _actor, float _dt, Transform _globalTransform = Transform{});
    // Renders the scene grapg and all chidlren if they inherit from "IRender" (called each frame) 
    void RenderLevelSceneGraph(std::shared_ptr<Actor> _actor, float _dt, Transform _globalTransform = Transform{});

    // Updates Input for the active controller
    void UpdateInputController(float _dt);
  
   

    //// Local scene UIRender function for distribution (called each frame) 
    //void RenderUI();
    //// Contains all custom logic for the world ImGui window
    //void imgui_WorldObjectSettings();
    //// Contains custom ImGui logic for details sub section
    //void imguiSub_WorldDetails(Actor* _aptr);
    //// Contains custom ImGui logic for collision sub section
    //void imguiSub_Collision(class IBounded* _cptr);
    //// Contains custom ImGui logic for light sub section
    //void imguiSub_Light(class Light* _lptr);
    //// Contains all custom logic for the DebugLogger ImGui window
    //void imgui_Logger();
    //// Contains custom ImGui logic for FPS sub section
    //void imguiSub_FPS();

    // Callbacks for camera or active controller to process movement or other locally. 
    void FramebufferSizeCallback(std::shared_ptr<Window> _window, int _width, int _height);
    void MouseMoveCallback(std::shared_ptr<Window> _window, double _xpos, double _ypos);
    void MouseButtonCallback(std::shared_ptr<Window> _window, int _button, int _action, int _mods);
    void MouseScrollCallback(std::shared_ptr<Window> _window, double _xoffset, double _yoffset);
    void CharCallback(std::shared_ptr<Window> _window, unsigned int _codepoint);
    void KeyCallback(std::shared_ptr<Window> _window, int _key, int _scancode, int _action, int _mods);

private:
    // ---------- Local functions --------------
    // Binds all directional light objects to the shader for light processing
    void BindDirectionalLights();
    // Binds all point light objects to the shader for light processing
    void BindPointLights();
    // Binds the camera objects to the shader for light processing
    void BindCamera();

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Set`s a new controller to the active controller
    void SetController(const std::shared_ptr<IController> _controller) { mActiveController = _controller; }

    // Setts a new window 
    void SetWindow(std::shared_ptr<Window> _window) { mWindow = _window; }

    // Getters

    // Get`s the current active controller
    std::shared_ptr<IController> GetController() const { return mActiveController; }

};

