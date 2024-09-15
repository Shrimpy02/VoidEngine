
// Includes
#include <SLifeTime.h>
#include <Actor.h>
#include <LevelActors/DebugActor.h>
#include <Levels/Level.h>
#include <OctTree.h>

// Additional includes
#include <ctime>

void SLifeTime::ManageDebugActorLifeTime(std::shared_ptr<Level> _level, std::vector<std::shared_ptr<Actor>> _lifeTimeActors)
{
    if (!_lifeTimeActors.empty())
    {
        for (std::shared_ptr<Actor> actor : _lifeTimeActors)
        {
            if (std::shared_ptr<DebugActor> lifetimeActor = std::dynamic_pointer_cast<DebugActor>(actor))
            {
                if (lifetimeActor->IsLifetimeEnabled())
                {
                    double timeSinceActorInit = difftime(time(nullptr), lifetimeActor->GetLifeTime());

                    if ((timeSinceActorInit) >= SActorLifeTime)
                    {
                        _level->RemoveActorFromSceneGraph(lifetimeActor);
                    }
                }
            }
        }
    }
}

void SLifeTime::ManageOctTreeNodeLifeTime(std::shared_ptr<Level> _level, std::shared_ptr<OctTree_Node> _octTreeNode)
{
    if (_octTreeNode->HasChildren())
    {
        for (std::shared_ptr<OctTree_Node> child : _octTreeNode->GetChildren())
        {
            if (child)
                ManageOctTreeNodeLifeTime(_level, child);

        }

    }
    else if (_octTreeNode->ContainsObjects()) {

        if(_octTreeNode->mLifeTimeEnabled)
        {

            if (std::shared_ptr<ILifeTime> lifetime = std::dynamic_pointer_cast<ILifeTime>(_octTreeNode))
            {
                double timeSinceNodeEmpty = difftime(time(nullptr), lifetime->GetLifeTime());

                if ((timeSinceNodeEmpty) >= SNodeLifeTime)
                {
                    if (std::shared_ptr<OctTree_Node> parent = _octTreeNode->GetParent())
                        parent->RemoveChild(_octTreeNode);
                }

            }
        }
    }
}
