
// Includes
#include <OctTree.h>
#include <Levels/Level.h>
#include <LevelActors/BaseActor.h>
#include <LevelActors/DebugActor.h>
#include <Utilities/Defines.h>
#include <Utilities/Logger.h>

// Additional includes

// -------------------------------- Node Bounds ---------------------------------------
// -------------------------------------------------------------------------------------

NodeBounds::NodeBounds(glm::vec3 _minExtent, glm::vec3 _maxExtent)
    : mMinExtent(_minExtent), mMaxExtent(_maxExtent) {}

bool NodeBounds::Intersect(std::shared_ptr<BaseActor> _object)
{
    if (_object)
        return _object->BoundingSpherexConstrictingBoxIntersect(mMinExtent, mMaxExtent);
    return false;
}

bool NodeBounds::HardContains(std::shared_ptr<BaseActor> _object)
{
    if (_object)
        return _object->BoundingSpherexConstrictingBoxContain(mMinExtent, mMaxExtent);
    return false;
}

// -------------------------------- OctTree_Node ---------------------------------------
// -------------------------------------------------------------------------------------

// Octree todo fix:
// todo - CreateAndInsertIntoChild -> should insert if completly contained, if not should check all neibours, not only first one.
// todo - InsertContainedObject and InsertIntersectObject should also psuh all intersecting children when child is created
// todo - Fix update intersect only objects not being pushed anywhere in update
// todo -

OctTree_Node::OctTree_Node(NodeBounds _nodeBounds, int _depth, std::shared_ptr<OctTree_Node> _parent, std::shared_ptr<Level> _activeLevel)
: mNodeBounds(_nodeBounds), mCurrentDepth(_depth), mParentNode(_parent), mActiveLevel(_activeLevel)
{
    mChildren.fill(nullptr);

    // For auto deletion when node is empty
    if (mParentNode)
        InitializeLifeTime();

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

// -------------------------------- Global Functions ------------------------------------------

void OctTree_Node::OctTreeUpdate()
{
    // Recursively move down to update all leaf nodes
    if (mHasChildren)
    {
        for (std::shared_ptr<OctTree_Node> childNode : mChildren)
        {
            if (childNode)
                childNode->OctTreeUpdate();
        }

        // If it does not have children its a leaf-node,
        // and if exits ig has either contained or intersect objects so update them in turn.
    } else {

        // Updates Contained objects first
        for (std::shared_ptr<BaseActor> object : mContainedNodeActors)
        {
            if (object)
            {
                // If the object is no longer completely contained.
                if (!mNodeBounds.HardContains(object))
                {
                    // then remove it from the contained object vector
                    RemoveContainedObject(object);

                    // check if it intersects the node
                    if(mNodeBounds.Intersect(object))
                    {
                        // if it does insert it into intersect vector 
                        InsertIntersectObject(object);
                    }
                	else
                	{
                        // if it does not intersect the node its somewhere unknown,
                        // insert into root to figure out where it went.
                        InsertObjectIntoRoot(object);
                	}
                }
            }
        }

        // Updates Intersected objects second
        for (std::shared_ptr<BaseActor> object : mIntersectingActors)
        {
            if (object)
            {
                // If it does not intersect 
                if (!mNodeBounds.Intersect(object))
                {
                    // then remove if from intersect vector
                    RemoveIntersectObject(object);
                    //InsertObjectIntoRoot(object);

                    // If it intersects and contains
                } else if (mNodeBounds.HardContains(object))
                {
                    // Remove from intersect, add to contain
                    RemoveIntersectObject(object);
                    InsertContainedObject(object);
                } 
            }
        }
    }
}

bool OctTree_Node::InsertObject(std::shared_ptr<BaseActor> _object)
{
    // Checks if hard contains first
    if (mNodeBounds.HardContains(_object))
        return InsertContainedObject(_object);

    // Then Intersect since both are true if hard contain is true.
    else if (mNodeBounds.Intersect(_object))
        return InsertIntersectObject(_object);

    return false;
}

void OctTree_Node::InsertObjectIntoRoot(std::shared_ptr<BaseActor> _object)
{
    // Recursively insert _object up the tree into the root node
    if (mParentNode)
        mParentNode->InsertObjectIntoRoot(_object);
    else
        InsertObject(_object);
}

void OctTree_Node::RemoveContainedObject(std::shared_ptr<BaseActor> _object)
{
    // Finds the iterator to input _object in object contain vector and removes it, also updates colors of debug
    auto it = std::find(mContainedNodeActors.begin(), mContainedNodeActors.end(), _object);
    if (it != mContainedNodeActors.end())
    {
        mContainedNodeActors.erase(it);
        if (mContainedNodeActors.empty() && !mIntersectingActors.empty())
            SetDebugMeshYellow();
        else if (!ContainsObjects())
        {
            SetDebugMeshRed();
            ReInitializeLifeTime();
        }
    }
}

void OctTree_Node::RemoveIntersectObject(std::shared_ptr<BaseActor> _object)
{
    // Finds the iterator to input _object in object intersect vector and removes it, also updates colors of debug
    auto it = std::find(mIntersectingActors.begin(), mIntersectingActors.end(), _object);
    if (it != mIntersectingActors.end())
    {
        mIntersectingActors.erase(it);

        if (mIntersectingActors.empty() && !mContainedNodeActors.empty())
            SetDebugMeshGreen();
        else if (!ContainsObjects())
        {
            SetDebugMeshRed();
            ReInitializeLifeTime();
        }
    }
}

void OctTree_Node::RemoveChild(std::shared_ptr<OctTree_Node> _child)
{
    for (int i = 0; i < 8; i++)
    {
        if (mChildren[i].get() == _child.get())
        {
            mChildren[i] = nullptr;
            break;  // Early exit once the child is removed
        }
    }
}

void OctTree_Node::Resize(NodeBounds _nodeBounds)
{
}

// -------------------------------- Local Functions ------------------------------------------

bool OctTree_Node::InsertContainedObject(std::shared_ptr<BaseActor> _object)
{
    // Add object to contain vector if this is leaf and max num objects not reached.
    if (mContainedNodeActors.size() < sMaxObjectsInNode && !mHasChildren)
    {
        mContainedNodeActors.push_back(_object);
    	SetDebugMeshGreen();
        return true;
    }

    // if cant add, make child if not at max depth
    if (mCurrentDepth < sMaxDepth)
    {
        if(CreateAndInsertIntoChildren(_object))
        {
            // if children created move all current contained objects to children. 
            for (std::shared_ptr<BaseActor> object : mContainedNodeActors)
            {
                CreateAndInsertIntoChildren(object);
            }
            mContainedNodeActors.clear();
            mHasChildren = true;
            SetDebugMeshRed();
        } else {
            LOG_ERROR("No insertion room in node!");
        }
    }

    return false;
}

bool OctTree_Node::InsertIntersectObject(std::shared_ptr<BaseActor> _object)
{
    // Add object to intersect vector if this is leaf -> does not have children
    if (!mHasChildren)
    {
        // Check if already intersected and processed (might not be necessary )
        if (std::find(mIntersectingActors.begin(), mIntersectingActors.end(), _object) != mIntersectingActors.end())
            return false;

        // Add object to neighbours excluding this node 
        if(mParentNode)
			mParentNode->InsertIntersectIntoNeighbors(_object, shared_from_this());

        mIntersectingActors.push_back(_object);
        SetDebugMeshYellow();

        return true;
    }

    // if cant add, make child if not at max depth
    if (mCurrentDepth < sMaxDepth)
    {
        if (CreateAndInsertIntoChildren(_object))
        {
            // if children created move all current intersect objects to children. 
            for (std::shared_ptr<BaseActor> object : mContainedNodeActors)
            {
                CreateAndInsertIntoChildren(object);
            }
            mContainedNodeActors.clear();
            mHasChildren = true;
            SetDebugMeshRed();
        }
        else {
            LOG_ERROR("No insertion room in node!");
        }
    }

    return false;
}

bool OctTree_Node::InsertIntersectIntoNeighbors(std::shared_ptr<BaseActor> _object, std::shared_ptr<OctTree_Node> _excludeNode)
{
    // this is called a node higher than initial insert
    bool ShouldPropagateHigher = true;

    for (int i = 0; i < 8; i++)
    {
        // checks each child, excluding the child node that called this function
        NodeBounds childBounds = GetChildBounds(mNodeBounds, i);
        if (_excludeNode)
            if (mChildren[i])
                if (mChildren[i].get() == _excludeNode.get())
                    continue;

        // if any children intersects, add object as overlap, and stop propagating up
        if (childBounds.Intersect(_object))
        {
            if (!mChildren[i])
                mChildren[i] = std::make_shared<OctTree_Node>(childBounds, mCurrentDepth + 1, shared_from_this(), mActiveLevel);

            if (mChildren[i]->AddOverlapObject(_object))
                ShouldPropagateHigher = false;
        }
    }
    // if this has a parent and no overlap was added, propagate to this parent excluding this node. 
    if (mParentNode && ShouldPropagateHigher)
        return mParentNode->InsertIntersectIntoNeighbors(_object, mParentNode);

    return true;
}

bool OctTree_Node::AddOverlapObject(std::shared_ptr<BaseActor> _object)
{
    // Add object to intersect vector if this is leaf -> does not have children
    if (!mHasChildren)
    {
        // Check if already intersected and processed
        if (std::find(mIntersectingActors.begin(), mIntersectingActors.end(), _object) != mIntersectingActors.end())
            return false;

        //Add it without checking neighbours since this is a helper function
        mIntersectingActors.push_back(_object);
        SetDebugMeshYellow();
        return true;
    } else {

        // If this has children check each child and add it there is an intersect
        for (int i = 0; i < 8; i++)
        {
            NodeBounds childBounds = GetChildBounds(mNodeBounds, i);

            if (childBounds.Intersect(_object))
            {
                if (!mChildren[i])
                    mChildren[i] = std::make_shared<OctTree_Node>(childBounds, mCurrentDepth + 1, shared_from_this(), mActiveLevel);

                mChildren[i]->AddOverlapObject(_object);
            }
        }
        return true;
    }
}

bool OctTree_Node::CreateAndInsertIntoChildren(std::shared_ptr<BaseActor> _object)
{
    // checks if _object is contained within any children if it is then insert and return true.
    for(int i = 0; i < 8 ; i++)
    {
        NodeBounds childBounds = GetChildBounds(mNodeBounds, i);

        if(childBounds.HardContains(_object))
        {
            if (!mChildren[i])
                mChildren[i] = std::make_shared<OctTree_Node>(childBounds, mCurrentDepth + 1, shared_from_this(), mActiveLevel);

            mChildren[i]->InsertContainedObject(_object);
            return true;
        }
    }

    // if not it checks intersects with all children and returns true if intersects
    for (int i = 0; i < 8; i++)
    {
        NodeBounds childBounds = GetChildBounds(mNodeBounds, i);

        if (childBounds.Intersect(_object))
        {
            if (!mChildren[i])
                mChildren[i] = std::make_shared<OctTree_Node>(childBounds, mCurrentDepth + 1, shared_from_this(), mActiveLevel);

            mChildren[i]->InsertIntersectObject(_object);
            return true;
        }
    }

    return false;
}

NodeBounds OctTree_Node::GetChildBounds(NodeBounds& _parentBounds, const int _it)
{
    NodeBounds childBounds;

    glm::vec3 centre = glm::vec3{
      (_parentBounds.mMinExtent.x + _parentBounds.mMaxExtent.x) / 2.0f,
      (_parentBounds.mMinExtent.y + _parentBounds.mMaxExtent.y) / 2.0f,
      (_parentBounds.mMinExtent.z + _parentBounds.mMaxExtent.z) / 2.0f
    };

    switch (_it) {
    case 0:
        // Bottom Left Back
        childBounds = NodeBounds(mNodeBounds.mMinExtent, centre);
        return childBounds;
    	break;
    case 1:
        // Bottom Right Back
        childBounds = NodeBounds(glm::vec3(centre.x, mNodeBounds.mMinExtent.y, mNodeBounds.mMinExtent.z), glm::vec3(mNodeBounds.mMaxExtent.x, centre.y, centre.z));
        return childBounds;
        break;
    case 2:
        // Top Left Back
        childBounds = NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, centre.y, mNodeBounds.mMinExtent.z), glm::vec3(centre.x, mNodeBounds.mMaxExtent.y, centre.z));
        return childBounds;
        break;
    case 3:
        // Top Right Back
        childBounds = NodeBounds(glm::vec3(centre.x, centre.y, mNodeBounds.mMinExtent.z), glm::vec3(mNodeBounds.mMaxExtent.x, mNodeBounds.mMaxExtent.y, centre.z));
        return childBounds;
        break;
    case 4:
        // Bottom Left Front
        childBounds = NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, mNodeBounds.mMinExtent.y, centre.z), glm::vec3(centre.x, centre.y, mNodeBounds.mMaxExtent.z));
        return childBounds;
		break;
    case 5:
        // Bottom Right Front
        childBounds = NodeBounds(glm::vec3(centre.x, mNodeBounds.mMinExtent.y, centre.z), glm::vec3(mNodeBounds.mMaxExtent.x, centre.y, mNodeBounds.mMaxExtent.z));
        return childBounds;
        break;
    case 6:
        // Top Left Front
        childBounds = NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, centre.y, centre.z), glm::vec3(centre.x, mNodeBounds.mMaxExtent.y, mNodeBounds.mMaxExtent.z));
        return childBounds;
        break;
    case 7:
        // Top Right Front
        childBounds = NodeBounds(centre, mNodeBounds.mMaxExtent);
        return childBounds;
        break;
    default:
        LOG_ERROR("No Childbounds returned");
        return childBounds;
        break;
    }
}

void OctTree_Node::SetDebugMeshRed()
{
    if (mDebugActor)
        mDebugActor->SetColor(GetDepthRedColor());
}

void OctTree_Node::SetDebugMeshGreen()
{
    if (mDebugActor)
        mDebugActor->SetColor(GetDepthGreenColor());
}

void OctTree_Node::SetDebugMeshYellow()
{
    if (mDebugActor)
        mDebugActor->SetColor(GetDepthYellowColor());
}

std::vector<std::shared_ptr<OctTree_Node>> OctTree_Node::GetActiveChildren()
{
    std::vector<std::shared_ptr<OctTree_Node>> tempR;
    for(std::shared_ptr<OctTree_Node> child : mChildren)
    {
        if (child)
            tempR.push_back(child);
    }
    return tempR;
}

std::vector<std::shared_ptr<OctTree_Node>> OctTree_Node::GetAllChildren()
{
    std::vector<std::shared_ptr<OctTree_Node>> tempR;
    for (std::shared_ptr<OctTree_Node> child : mChildren)
    {
    	tempR.push_back(child);
    }
    return tempR;
}

std::pair<glm::vec3, glm::vec3> OctTree_Node::GetMinMaxExtent()
{
    return std::make_pair(mNodeBounds.mMinExtent, mNodeBounds.mMaxExtent);
}
