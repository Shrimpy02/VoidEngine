
// Includes
#include <SLifeTime.h>
#include <Actor.h>
#include <LevelActors/DebugActor.h>
#include <LevelActors/BaseActor.h>
#include <LevelActors/GraphActor.h>
#include <LevelActors/PartcleSystem/ParticleEmitter.h>
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
        for (std::shared_ptr<OctTree_Node> child : _octTreeNode->GetActiveChildren())
        {
            if (child)
                ManageOctTreeNodeLifeTime(_level, child);

        }

    }
    else if (!_octTreeNode->ContainsObjects()) {

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

void SLifeTime::ManagePhysicsPathControlPointLifeTime(std::vector<std::shared_ptr<Actor>> _lifeTimeActors)
{
    if (!_lifeTimeActors.empty())
    {
        for (std::shared_ptr<Actor> actor : _lifeTimeActors)
        {
            if (std::shared_ptr<BaseActor> lifetimeActor = std::dynamic_pointer_cast<BaseActor>(actor))
            {
                double timeSinceActorInit = difftime(time(nullptr), lifetimeActor->GetLifeTime());
                if ((timeSinceActorInit - lifetimeActor->timeOffset) >= SControlPointInterval)
                {
                    if(lifetimeActor->mGraphActor)
                    {
                        if(lifetimeActor->mGraphActor->mControlPoints.size() < 10)
                        {
                            if(lifetimeActor->GetGlobalPosition().y > -2)
                            {
                                lifetimeActor->previusControlPoints.push_back(lifetimeActor->GetGlobalPosition());
                                lifetimeActor->mGraphActor->SetControlPoints(lifetimeActor->previusControlPoints);
                            }
                        }
                    }
                		
                    lifetimeActor->timeOffset += 1.0f;
                }
            }
        }
    }
}

void SLifeTime::ManageParticleLifeTime(std::vector<std::shared_ptr<Actor>> _lifeTimeActors)
{
    if (!_lifeTimeActors.empty())
    {
        for (std::shared_ptr<Actor> actor : _lifeTimeActors)
        {
            if (std::shared_ptr<ParticleEmitter> lifetimeActor = std::dynamic_pointer_cast<ParticleEmitter>(actor))
            {
                lifetimeActor->UpdateLifeTime();
            }
        }
    }
}
