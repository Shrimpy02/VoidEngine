#pragma once
// Includes
#include <Core/Tag.h>


// Additional Includes
#include <memory>


// Forward Declarations
struct TagUnique;
struct LifeTimeManager;
class CameraActor;
class SceneGraph;
class Actor;
class DebugActor;
class OctTree_Node;

/**
 * @struct Level
 * @brief The level struct contains all items in it and some helper functions. 
 */
struct Level : public std::enable_shared_from_this<Level>
{
private:
    // ----------- Local Variables ---------------

    TagUnique mTag;

public:
    // ----------- Global Variables ---------------

    std::shared_ptr<SceneGraph> mSceneGraph{nullptr};
    std::shared_ptr<CameraActor> mActiveCamera{nullptr}; 
    std::shared_ptr<OctTree_Node> mOctTreeRootNode{ nullptr };
    //Skybox mSkybox{ nullptr };

	// ----------- Global Functions ---------------

    // Constructor
    Level(const std::string& _name);

    // Functions
    void AddActorToSceneGraph(std::shared_ptr<Actor> _inActor);

    void RemoveActorFromSceneGraph(std::shared_ptr<Actor> _inActor);

    void RemoveActorFromSceneGraphRecursive(const std::shared_ptr<Actor>& parent, const std::shared_ptr<Actor>& _inActor);

    void LifeTimeUpdate();

    void ClearLevel();

    // Setters ---------------

    // Getters ---------------

    std::string GetLevelName();
    
};

