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
class IController;
class Actor;
struct Transform;
class UserInterfaceManager;

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
    std::shared_ptr<Level> mActiveLevel{ nullptr }; 

    std::shared_ptr<Window> mWindow{ nullptr };
    std::shared_ptr<UserInterfaceManager> mUserInterfaceManager{ nullptr };

    std::shared_ptr<IController> mController{ nullptr };

    std::shared_ptr<Shader> mDefaultShader{ nullptr };
    std::shared_ptr<Shader> mGraphShader{ nullptr };
    std::shared_ptr<Shader> mDebugShader{ nullptr };
    std::shared_ptr<Shader> mSkyboxShader{ nullptr };

public:
    // ---------- Global functions --------------
    explicit LevelManager(std::shared_ptr<Window> _window, std::shared_ptr<UserInterfaceManager> _interfaceManager);

    // Removes the ability to:    
    LevelManager(const LevelManager&) = delete;           // Copy
    LevelManager& operator=(const LevelManager&) = delete;// Copy ref
    LevelManager(LevelManager&&) = delete;                // Move
    LevelManager& operator=(LevelManager&&) = delete;     // Move ref
    // Because this class is explicit.
    ~LevelManager() = default;

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

    // Scene collision handler function for all scene objects that inherit from "IBounded" (called each frame) 
    void ProcessCollision();

	// Updates the scene graph and all children if they inherit from "Actor" (called each frame) 
	void UpdateLevelSceneGraph(std::shared_ptr<Actor> _actor, float _dt, Transform _globalTransform = Transform{});
    // Renders the scene grapg and all chidlren if they inherit from "IRender" (called each frame) 
    void RenderLevelSceneGraph(std::shared_ptr<Actor> _actor, float _dt, Transform _globalTransform = Transform{});

    // Updates Input for the active controller
    void UpdateInputControler(float _dt);

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
    void BindDirectionalLights(std::shared_ptr<Shader> _bindShader);
    // Binds all point light objects to the shader for light processing
    void BindPointLights(std::shared_ptr<Shader> _bindShader);
    // Binds the camera objects to the shader for light processing
    void BindCamera(std::shared_ptr<Shader> _bindShader);

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Set`s a new controller to the active controller
    void SetController(const std::shared_ptr<IController> _controller) { mController = _controller; }

    // Setts a new window 
    void SetWindow(std::shared_ptr<Window> _window) { mWindow = _window; }

    // Getters

    // Get`s the current active controller
    std::shared_ptr<IController> GetController() const { return mController; }
    

};

