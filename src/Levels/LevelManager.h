#pragma once
// Includes
#include <Transform.h>

// Additional Includes
#include <memory> 
#include <vector>

// temp for lua
#include "lua.h"

// Forward Declarations
struct Level;
class WindowManager;
class Shader;
class ActorController;
class IController;
class Actor;
class VisualActor;
struct Transform;
class UserInterfaceManager;
class DebugActor;
class CameraActor;
class OctTree_Node;
class BaseActor;
class AISystem;
class HealthSystem;
class PhysicsSystem;
class Material;
class Mesh;
class GraphActor;
class CollisionSystem;

/**
 * @class LevelManager
 * @brief 
 */
class LevelManager : public std::enable_shared_from_this<LevelManager>


{
public:
    // ---------- Global Variables --------------

        //tmp
    std::shared_ptr<BaseActor> PLayer{ nullptr };
    std::shared_ptr<VisualActor> Surface{ nullptr };
    std::shared_ptr<Material> enemyMat{ nullptr };
    std::shared_ptr<DebugActor> mDebugSpawnerBall{ nullptr };
    std::shared_ptr<VisualActor> mBallTerrain{ nullptr };
    std::vector<std::shared_ptr<GraphActor>> mBallTerrainGraphes{ nullptr };
    
    // Component Systems
    std::shared_ptr<AISystem> mAiSystem;
    std::shared_ptr<HealthSystem> mHealthSystem;
    std::shared_ptr<PhysicsSystem> mPhysicsSystem;
    std::shared_ptr<CollisionSystem> mCollisionSystem;

    lua_State* mActiveLuaInterpreter{nullptr};

    void SpawnWave();

private:
    // ---------- Local Variables -------------


    std::shared_ptr<UserInterfaceManager> mUserInterfaceManager{nullptr};

    std::vector<std::shared_ptr<Level>> mAllLevels;
    std::shared_ptr<Level> mActiveLevel{ nullptr }; 

    std::shared_ptr<ActorController> mController{ nullptr };


    std::shared_ptr<Shader> mDefaultShader{ nullptr };
    std::shared_ptr<Shader> mGraphShader{ nullptr };
    std::shared_ptr<Shader> mDebugShader{ nullptr };
    std::shared_ptr<Shader> mPointCloudShader{ nullptr };
    std::shared_ptr<Shader> mSkyboxShader{ nullptr };

    std::shared_ptr<VisualActor> mConformBox{ nullptr };
         
    time_t mApplicationStartTime = 0;



 
public:
    // ---------- Global functions --------------
    explicit LevelManager(std::shared_ptr<ActorController> _inController, std::shared_ptr<UserInterfaceManager> _userIManager);

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

    void BaseLevelRequiredObjects();

    void LoadDefaultLevel();

    void LoadPhysicsBoxLevel();

    void LoadGraphDisplayLevel();

    void LoadTestGame();

    void LoadFolderLevel();

    void LoadFolderLevelGameEngine();

	// Deletes pointers and clears all caches for texture, mesh and material.  
    void UnloadContent();

    void SetActiveLevel(std::shared_ptr<Level> _activeLevel); 

    // Local scene update function for distribution (called each frame) 
    void Update(float _dt);
    // Local scene render function for distribution (called each frame) 
    void Render();

    // Scene collision handler function for all scene objects that inherit from "IBounded" (called each frame) 
    void CheckLevelCollision();

    void ProcessPhysicsCollision(std::shared_ptr<Actor> _colliderA, std::shared_ptr<Actor> _colliderB);

    void CheckLevelCollisionWithinBoxBounds(std::shared_ptr<VisualActor> _conformBox);

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

    void AddDebugActor(std::vector<std::shared_ptr<DebugActor>>& _debActorStorage, const std::pair<glm::vec3, glm::vec3>& _extents, const glm::vec3& _position = glm::vec3(0), const glm::vec3& _color = glm::vec3(1));

    std::shared_ptr<Mesh> CreateTriangulatedMesh(std::shared_ptr<VisualActor> _root, const glm::vec3& _min, const glm::vec3& _max, bool _enableDebug, bool _enableConsole);

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

    std::shared_ptr<VisualActor> GetConformBox() { return mConformBox; };

};

