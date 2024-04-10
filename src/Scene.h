#pragma once
// Includes
#include <Camera.h>
#include <SceneActors.h>

// Additional Includes
#include <memory> // smart pointers 

/**
 * @class Scene
 * @brief This class is in effect a world, it loads all content in a scene and initialize`s the objects that use them.
 * It indirectly manages all scene objects by using a SceneGraph as a root node, all updates and render calls are passed down through it to it`s children.
 */
class Scene
{
public:
    // ---------- Global Variables --------------

    SceneGraph mSceneGraph;
    CameraActor mSceneCamera{ "SceneCamera" };
    class Window* mWindow = nullptr;

private:
    // ---------- Local Variables --------------

	// Shader`s ----------------- 
    Shader* mShader{ nullptr };

    // Scene Items -------------------------
    // MeshActors
    BaseActor* mMACube0{ nullptr };
    BaseActor* mMACube1{ nullptr };

    VisualActor* mVAPlane0{ nullptr };
    // Other actors

	// lights
    class DirectionalLightActor* mDirectionalLightActor{ nullptr };

    // ImGui UI Variables
    int mMainSelectionIndex = 0;
    int mOldSelectionIndex;
    const float mItemWidth = 80.0f;

    float mCurrentUniformScale = 1;
    float mOldUniformScale = 1;
    glm::vec3 mActorOriginalScale;
   
	bool mShouldShowWireFrame = false;
    bool mShouldDrawCollisionDebugMesh = false;
	bool mCanControlActor = false;
    bool mIsUniformScale = false;

    double mOldTime = 0;
    int numFrames = 0;

    // Other
    std::shared_ptr<class ActorController> mActorController;
    std::shared_ptr<class CameraController> mCameraController;

protected:

    // the active controller
    std::shared_ptr<class IController> mActiveController{ nullptr };

public:
    // ---------- Global functions --------------
    explicit Scene(const std::string& _name, Window* _window);

    // Removes the ability to:    
    Scene(const Scene&) = delete;           // Copy
    Scene& operator=(const Scene&) = delete;// Copy ref
    Scene(Scene&&) = delete;                // Move
    Scene& operator=(Scene&&) = delete;     // Move ref
    // Because this class is explicit.

    // default de-constructor
    virtual ~Scene() = default;

    // Function`s
    // ------------------------------------------------------------
    
    // Loads all objects, their materials and meshes and assigns them as a child of mSceneGraph so they
    // receive update and render calls if they inherit from the correct classes.
    virtual void LoadContent();
    // Deletes pointers and clears all caches for texture, mesh and material.  
    virtual void UnloadContent();

    // Updates Input for the active controller
    void UpdateInputController(float _dt);
    // Updates the scene graph and all children if they inherit from "Actor" (called each frame) 
    void UpdateSceneGraph(Actor* _actor, float _dt, Transform _globalTransform = Transform{});
    // Renders the scene grapg and all chidlren if they inherit from "IRender" (called each frame) 
    void RenderSceneGraph(Actor* _actor, float _dt, Transform _globalTransform = Transform{});
    // Local scene update function for distribution (called each frame) 
	void Update(float _dt);
    // Local scene render function for distribution (called each frame) 
    void Render(float _dt);
    // Scene collision handler function for all scene objects that inherit from "IBounded" (called each frame) 
    void HandleCollision();
        // Template helper function for HandleCollision, takes two template arguments for either aabb
        // or BoundingSphere in addition to who it belongs and executes the appropriate logic for them.
        // Returns true if there was a collision and false otherwise.
        template <typename T, typename U>
        bool ProcessCollision(T _a,U _b,IBounded* _iA, IBounded* _iB, Actor* _AA, Actor* _AB);

	// Local scene UIRender function for distribution (called each frame) 
    void RenderUI();
    // Contains all custom logic for the world ImGui window
	void imgui_WorldObjectSettings();
		// Contains custom ImGui logic for details sub section
        void imguiSub_WorldDetails(Actor* _aptr);
        // Contains custom ImGui logic for collision sub section
        void imguiSub_Collision(class IBounded* _cptr);
        // Contains custom ImGui logic for light sub section
        void imguiSub_Light(class Light* _lptr);
	// Contains all custom logic for the DebugLogger ImGui window
	void imgui_Logger();
		 // Contains custom ImGui logic for FPS sub section
		void imguiSub_FPS();

    // Callbacks for camera or active controller to process movement or other locally. 
    void FramebufferSizeCallback(class Window* _window, int _width, int _height);
    void MouseMoveCallback(class Window* _window, double _xpos, double _ypos);
    void MouseButtonCallback(class Window* _window, int _button, int _action, int _mods);
    void MouseScrollCallback(class Window* _window, double _xoffset, double _yoffset);
    void CharCallback(class Window* _window, unsigned int _codepoint);
	void KeyCallback(class Window* _window, int _key, int _scancode, int _action, int _mods);

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
    void SetController(const std::shared_ptr<IController>& _controller) { mActiveController = _controller; }

    // Setts a new window 
	void SetWindow(class Window* _window) { mWindow = _window;  }

    // Getters

    // Get`s the current active controller
	std::shared_ptr<IController> GetController() const { return mActiveController; }

};

