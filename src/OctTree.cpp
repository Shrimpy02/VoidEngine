
// Includes
#include <OctTree.h>
#include <Levels/Level.h>
#include <Utilities/Defines.h>
#include <Utilities/Logger.h>
#include <LevelActors/DebugActor.h>

// Additional includes

OctTree_Node::OctTree_Node(NodeBounds _nodeBounds, int _depth, std::shared_ptr<Level> _activeLevel)
: mNodeBounds(_nodeBounds), mCurrentDepth(_depth), mActiveLevel(_activeLevel)
{
    mChildren.fill(nullptr);

#ifdef DEBUG_ENABLED
    std::shared_ptr<DebugActor> debugActor = std::make_shared<DebugActor>("OctTree_debugBounds",glm::vec3((1.f/(mCurrentDepth+1)),0.f,0.f));
    std::pair<glm::vec3, glm::vec3> temp = GetMinMaxExtent();
    temp.first += 0.001*(mCurrentDepth+1);
    temp.second -= 0.001*(mCurrentDepth+1);
	debugActor->SetVisualMesh(temp);
    mDebugActor = debugActor;
    if(_activeLevel)
		_activeLevel->AddActorToSceneGraph(debugActor);
#endif
}

OctTree_Node::~OctTree_Node()
{
#ifdef DEBUG_ENABLED 
    if (mActiveLevel)
        if (mDebugActor)
            mActiveLevel->RemoveActorFromSceneGraph(mDebugActor);
    mDebugActor = nullptr;
#endif
}

bool OctTree_Node::InsertObject(std::shared_ptr<BaseActor> _object)
{
    // 1. Check if the object is within this node, return false if it is not within
    if (!mNodeBounds.Contains(_object)) return false;


    // 2. If num objects is less than max num objects or
    //      if depth is less than max depth, add object to this node and return true
    //      otherwise continue. 
    if (mNodeActors.size() < sMaxObjectsInNode && !mSubdivided)
    {
        mNodeActors.push_back(_object);
        if (mDebugActor)
            mDebugActor->SetColor(glm::vec3(0, 1, 0));
        return true;
        
    }

    // 3. If come here the object is not yet inserted and there is no more room, so
    //      if not already subdivided, subdivide this node
    if (!mSubdivided)
        if (mCurrentDepth < sMaxDepth)
        {

            // 3.5 If subdividing sector, move each stored object in this sector to its children,
        	//        Then Delete all objects in this sector since they have been moved.
            if (Subdivide())
            {
                for(std::shared_ptr<BaseActor> object : mNodeActors)
                {
                    for (std::shared_ptr<OctTree_Node> childNode : mChildren)
                    {
                        if (childNode->InsertObject(object))
                        {
                            break;
                        }
                    }
                }
                mNodeActors.clear();
                if (mDebugActor)
                    mDebugActor->SetColor(glm::vec3((1.f / (mCurrentDepth + 1)), 0.f, 0.f));
            }
        }
        else
        {
            LOG_ERROR("No more depth room for object in oct tree!");
            return false;
        }

    // 4. If here this node is subdivided and there is no more space, itterate through
    //      each child of this node and try inserting it into each one, return true if success
    for (std::shared_ptr<OctTree_Node> childNode : mChildren)
    {
        if(childNode->InsertObject(_object))
			return true;
    }

    // 5. Insert fail, return false
    return false;
}

bool OctTree_Node::Subdivide()
{
    // 1. Find the middle of the oct tree node
    glm::vec3 centre = glm::vec3{
        (mNodeBounds.mMinExtent.x + mNodeBounds.mMaxExtent.x) / 2.0f,
		(mNodeBounds.mMinExtent.y + mNodeBounds.mMaxExtent.y) / 2.0f,
		(mNodeBounds.mMinExtent.z + mNodeBounds.mMaxExtent.z) / 2.0f
    };
  
    // 2. Create and assign each child their sector
    mChildren[0] = std::make_shared<OctTree_Node>(NodeBounds(mNodeBounds.mMinExtent, centre), mCurrentDepth + 1, mActiveLevel);
    mChildren[1] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(centre.x, mNodeBounds.mMinExtent.y, mNodeBounds.mMinExtent.z), glm::vec3(mNodeBounds.mMaxExtent.x, centre.y, centre.z)), mCurrentDepth + 1, mActiveLevel);
    mChildren[2] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, centre.y, mNodeBounds.mMinExtent.z), glm::vec3(centre.x, mNodeBounds.mMaxExtent.y, centre.z)), mCurrentDepth + 1, mActiveLevel);
    mChildren[3] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(centre.x, centre.y, mNodeBounds.mMinExtent.z), glm::vec3(mNodeBounds.mMaxExtent.x, mNodeBounds.mMaxExtent.y, centre.z)), mCurrentDepth + 1, mActiveLevel);
    mChildren[4] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, mNodeBounds.mMinExtent.y, centre.z), glm::vec3(centre.x, centre.y, mNodeBounds.mMaxExtent.z)), mCurrentDepth + 1, mActiveLevel);
    mChildren[5] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(centre.x, mNodeBounds.mMinExtent.y, centre.z), glm::vec3(mNodeBounds.mMaxExtent.x, centre.y, mNodeBounds.mMaxExtent.z)), mCurrentDepth + 1, mActiveLevel);
    mChildren[6] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, centre.y, centre.z), glm::vec3(centre.x, mNodeBounds.mMaxExtent.y, mNodeBounds.mMaxExtent.z)), mCurrentDepth + 1, mActiveLevel);
    mChildren[7] = std::make_shared<OctTree_Node>(NodeBounds(centre, mNodeBounds.mMaxExtent), mCurrentDepth + 1, mActiveLevel);

    LOG_WARNING("SubDibide");

	mSubdivided = true;
    // 3. return true for successful subdivide
    return true;
}

std::pair<glm::vec3, glm::vec3> OctTree_Node::GetMinMaxExtent()
{
    return std::make_pair(mNodeBounds.mMinExtent, mNodeBounds.mMaxExtent);
}
