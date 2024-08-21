#pragma once
// Includes
#include <Transform.h>

// Additional Includes
#include <memory> 
#include <vector>

// Forward Declarations
struct Level;
class WindowManager;
class Shader;
class ActorController;
class IController;
class Actor;
struct Transform;
class UserInterfaceManager;
class DebugActor;
class CameraActor;


/**
 * @class LevelManager
 * @brief 
 */
class LevelManager : public std::enable_shared_from_this<LevelManager>

{
public:
    // ---------- Global Variables --------------


private:
    // ---------- Local Variables --------------

    std::vector<std::shared_ptr<Level>> mAllLevels;
    std::shared_ptr<Level> mActiveLevel{ nullptr }; 

    std::shared_ptr<ActorController> mController{ nullptr };

    std::shared_ptr<Shader> mDefaultShader{ nullptr };
    std::shared_ptr<Shader> mGraphShader{ nullptr };
    std::shared_ptr<Shader> mDebugShader{ nullptr };
    std::shared_ptr<Shader> mSkyboxShader{ nullptr };

    time_t mApplicationStartTime = 0;

public:
    // ---------- Global functions --------------
    explicit LevelManager(std::shared_ptr<ActorController> _inController);

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
    void Render();

    // Scene collision handler function for all scene objects that inherit from "IBounded" (called each frame) 
    void ProcessCollision();

	// Updates the scene graph and all children if they inherit from "Actor" (called each frame) 
	void UpdateLevelSceneGraph(std::shared_ptr<Actor> _actor, float _dt, Transform _globalTransform = Transform{});
    // Renders the scene grapg and all chidlren if they inherit from "IRender" (called each frame) 
    void RenderLevelSceneGraph(std::shared_ptr<Actor> _actor, Transform _globalTransform = Transform{});

    // Callbacks for camera to process movement or other locally. 
    void FrameBufferSizeCallback(int _width, int _height);

    void AddActorToLevel(std::shared_ptr<Actor> _inActor);

    void RemoveActorFromLevel(std::shared_ptr<Actor> _inActor);

    // Makes All Shaders draw in wire-frame mode
    void ShadersDrawWireFrame(bool _b);

    std::shared_ptr<Actor> LineTrace(glm::vec3 _startPos, glm::vec3 _endPosition, float _resolution, bool _createDebugActor = false);
    std::shared_ptr<Actor> LineTrace(glm::vec3 _startPos, glm::vec3 _direction,float _resolution, float _length, bool _createDebugActor = false);
    std::shared_ptr<Actor> LineTraceCollision(std::vector<std::shared_ptr<Actor>>& _collisionActors, glm::vec3 _point);
    void CreateLineTraceDebugActor(std::vector<glm::vec3> _points); 

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
    void SetController(const std::shared_ptr<ActorController> _controller) { mController = _controller; }

    // Getters
    std::shared_ptr<ActorController> GetActorController() const { return mController;  }

    std::shared_ptr<Level> GetActiveLevel() { return mActiveLevel; }

    std::shared_ptr<Shader> GetDefaultShader() { return mDefaultShader; }

    std::shared_ptr<CameraActor> GetActiveCamera();
};

