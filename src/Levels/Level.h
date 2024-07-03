#pragma once
// Includes
#include <Core/Tag.h>
#include <SceneGraph.h>

// Additional Includes

// Forward Declarations
struct CameraActor;
class SceneGraph;

/**
 * @struct Level
 * @brief The level struct contains all items in it and some helper functions. 
 */
struct Level
{
private:
    // ----------- Local Variables ---------------

    TagUnique mTag;

public:
    // ----------- Global Variables ---------------


    std::shared_ptr<SceneGraph> mSceneGraph{nullptr};
    std::shared_ptr<CameraActor> mActiveCamera{nullptr}; 

    //Skybox mSkybox{ nullptr };

	// ----------- Global Functions ---------------

    // Constructor
    Level(const std::string& _name) : mTag(_name)
    {
        mSceneGraph = std::make_shared<SceneGraph>("SceneGraph");
    }

    // Functions
    void AddActorToSceneGraph(std::shared_ptr<Actor> _inActor)
    {
        mSceneGraph->AddChild(_inActor);
    }

    void RemoveActorToLevel()
	{

	}

    void ClearLevel()
	{

	}

    // Setters ---------------

    // Getters ---------------

	void GetLevelName()
    {

    }
};

