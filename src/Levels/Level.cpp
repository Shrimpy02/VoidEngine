
#include <Levels/Level.h>
#include <LevelActors/SceneGraph.h>
#include <LevelActors/DebugActor.h>
#include <Actor.h>
#include <SLifeTime.h>

Level::Level(const std::string& _name)
	: mTag(_name)
{
    mSceneGraph = std::make_shared<SceneGraph>("SceneGraph");
}

void Level::AddActorToSceneGraph(std::shared_ptr<Actor> _inActor)
{
    mSceneGraph->AddChild(_inActor);
}

void Level::RemoveActorFromSceneGraph(std::shared_ptr<Actor> _inActor)
{
    RemoveActorFromSceneGraphRecursive(mSceneGraph, _inActor);
}

void Level::RemoveActorFromSceneGraphRecursive(const std::shared_ptr<Actor>& parent, const std::shared_ptr<Actor>& _inActor)
{
 
    std::vector<std::shared_ptr<Actor>>& children = parent->GetChildren();
    for (auto it = children.begin(); it != children.end();)
    {
        if (*it == _inActor)
        {
            // Remove the tag from the tag cache
            std::string tempTag = _inActor->GetTag();
            TagUnique::RemoveTagFromCache(tempTag);

            std::vector<std::shared_ptr<Actor>>& actorChildren = _inActor->GetChildren();
            for (std::shared_ptr<Actor>& child : actorChildren)
            {
                RemoveActorFromSceneGraphRecursive(child, child);
            }

            it = children.erase(it);
        }
        else
        {
            // Recursively check the children of this child
            RemoveActorFromSceneGraphRecursive(*it, _inActor);
            ++it;
        }
    }
}

void Level::LifeTimeUpdate()
{
    // Get all IBounded Actors of the active level
    std::vector<std::shared_ptr<Actor>> lifeTimeActors;
    mSceneGraph->Query<ILifeTime>(lifeTimeActors);

    SLifeTime::ManageDebugActorLifeTime(shared_from_this(), lifeTimeActors);
    if(mOctTreeRootNode)
		SLifeTime::ManageOctTreeNodeLifeTime(shared_from_this(), mOctTreeRootNode);

}

void Level::ClearLevel()
{
}

std::string Level::GetLevelName()
{
    return mTag.GetValue().c_str();
}

