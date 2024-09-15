
// Includes
#include <OctTree.h>
#include <Levels/Level.h>
#include <LevelActors/BaseActor.h>
#include <Utilities/Defines.h>
#include <Utilities/Logger.h>
#include <LevelActors/DebugActor.h>

// Additional includes

NodeBounds::NodeBounds(glm::vec3 _minExtent, glm::vec3 _maxExtent)
    : mMinExtent(_minExtent), mMaxExtent(_maxExtent)
{
}

bool NodeBounds::Intersect(std::shared_ptr<BaseActor> _object)
{
    return _object->AABBxBoundingSphere(std::make_pair(mMinExtent, mMaxExtent));
}

bool NodeBounds::Overlaps(std::shared_ptr<BaseActor> _object)
{
  //  if (_object)
  //      return _object->BoundingSpherexConstrictingBoxSoft(mMinExtent, mMaxExtent);
    return false;
}

bool NodeBounds::Contains(std::shared_ptr<BaseActor> _object)
{
    if (_object)
        return _object->BoundingSpherexConstrictingBox(mMinExtent, mMaxExtent);

    return false;
}




OctTree_Node::OctTree_Node(NodeBounds _nodeBounds, int _depth, std::shared_ptr<OctTree_Node> _parent, std::shared_ptr<Level> _activeLevel)
: mNodeBounds(_nodeBounds), mCurrentDepth(_depth), mParentNode(_parent), mActiveLevel(_activeLevel)
{
    mChildren.fill(nullptr);

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
    if (!mNodeActors.empty())
    {
        //for (std::shared_ptr<BaseActor> object : mNodeActors)
        //    InsertObjectIntoRoot(object);
        //mNodeActors.clear();

        LOG_WARNING("DeletingNode With Objects in it, moved to root");
    }

#ifdef DEBUG_ENABLED 
    if (mActiveLevel)
        if (mDebugActor)
            mActiveLevel->RemoveActorFromSceneGraph(mDebugActor);
    mDebugActor = nullptr;
#endif
}

void OctTree_Node::OctTreeUpdate()
{
	// Recursively update all nodes of an oct tree
    if(mHasChildren)
    {
	    for(std::shared_ptr<OctTree_Node> childNode : mChildren)
	    {
            if(childNode)
				childNode->OctTreeUpdate();
	    }
        // if not subdivided it contains objects, run updates on them
    } else {
   
        for(std::shared_ptr<BaseActor> object : mNodeActors)
        {
	        if(object)
	        {
		        if(!mNodeBounds.Contains(object))
		        {
                    InsertObjectIntoRoot(object);
                    RemoveObject(object);

                    if (mNodeActors.empty())
                        ReInitalizeLifeTime();
		        }
	        }
        }
    }
}

//bool OctTree_Node::InsertObject(std::shared_ptr<BaseActor> _object)
//{
//    // 1. Check if the object is within this node, return false if it is not within
//    //if (!mNodeBounds.Contains(_object)) return false;
//
//
//    // 2. If num objects is less than max num objects or
//    //      if depth is less than max depth, add object to this node and return true
//    //      otherwise continue. 
//    if (!mSubdivided && mNodeActors.size() < sMaxObjectsInNode)
//    {
//        mNodeActors.push_back(_object);
//        if (mDebugActor)
//            mDebugActor->SetColor(glm::vec3(0, 1, 0));
//        return true;
//    }
//
//
//    // 1. Find the middle of the oct tree node
//    glm::vec3 centre = glm::vec3{
//        (mNodeBounds.mMinExtent.x + mNodeBounds.mMaxExtent.x) / 2.0f,
//        (mNodeBounds.mMinExtent.y + mNodeBounds.mMaxExtent.y) / 2.0f,
//        (mNodeBounds.mMinExtent.z + mNodeBounds.mMaxExtent.z) / 2.0f
//    };
//
//	// 3. If come here the object is not yet inserted and there is no more room, so
//    //      if not already subdivided, subdivide this node
//    if (!mSubdivided)
//        if (mCurrentDepth < sMaxDepth)
//        {
//
//            // 3.5 If subdividing sector, move each stored object in this sector to its children,
//        	//        Then Delete all objects in this sector since they have been moved.
//            if (Subdivide())
//            {
//                for(std::shared_ptr<BaseActor> object : mNodeActors)
//                {
//                    for (std::shared_ptr<OctTree_Node> childNode : mChildren)
//                    {
//                        if(childNode)
//                            if (childNode->InsertObject(object))
//                            {
//                                break;
//                            }
//                    }
//                }
//                mNodeActors.clear();
//                if (mDebugActor)
//                    mDebugActor->SetColor(GetDepthRedColor());
//            }
//        } else {
//            LOG_ERROR("No more depth room for object in oct tree!");
//            return false;
//        }
//
//    // 4. If here this node is subdivided and there is no more space, itterate through
//    //      each child of this node and try inserting it into each one, return true if success
//    for (std::shared_ptr<OctTree_Node> childNode : mChildren)
//    {
//        if(childNode)
//            if(childNode->InsertObject(_object))
//		    	return true;
//    }
//
//    // 5. Insert fail, return false
//    return false;
//}


bool OctTree_Node::InsertObject(std::shared_ptr<BaseActor> _object)
{

    if (mNodeActors.size() < sMaxObjectsInNode && !mHasChildren)
    {
        mNodeActors.push_back(_object);
    	SetDebugMeshGreen();
        return true;
    }

    if (mCurrentDepth < sMaxDepth)
    {
        if(CreateAndInsertIntoChildren(_object))
        {
            for (std::shared_ptr<BaseActor> object : mNodeActors)
            {
                CreateAndInsertIntoChildren(object);
            }
            mNodeActors.clear();
            mHasChildren = true;
            SetDebugMeshRed();
        } else {
            LOG_ERROR("No insertion room in node!");
        }
    }

    return false;
}



//void OctTree_Node::TransferObjectToNewNode(std::shared_ptr<BaseActor> _object, std::shared_ptr<OctTree_Node> _insertNode)
//{
//    _insertNode->InsertObject(_object);
//    RemoveObject(_object);
//}

void OctTree_Node::RemoveObject(std::shared_ptr<BaseActor> _object)
{
    auto it = std::find(mNodeActors.begin(), mNodeActors.end(), _object);
    if (it != mNodeActors.end())
    {
        mNodeActors.erase(it);
        if (mNodeActors.empty())
            SetDebugMeshRed();
    }

}

void OctTree_Node::RemoveChild(std::shared_ptr<OctTree_Node> _child)
{
    for(int i = 0; i < 8; i++)
    {
        if (mChildren[i].get() == _child.get())
        {
            mChildren[i] = nullptr;
            break;  // Early exit once the child is removed
        }
    }
}

bool OctTree_Node::CreateAndInsertIntoChildren(std::shared_ptr<BaseActor> _object)
{
    // Find the middle of the oct tree node
    glm::vec3 centre = glm::vec3{
        (mNodeBounds.mMinExtent.x + mNodeBounds.mMaxExtent.x) / 2.0f,
        (mNodeBounds.mMinExtent.y + mNodeBounds.mMaxExtent.y) / 2.0f,
        (mNodeBounds.mMinExtent.z + mNodeBounds.mMaxExtent.z) / 2.0f
    };

    // Bottom Left Back
    if (InsertIntoChild(_object, NodeBounds(mNodeBounds.mMinExtent, centre), 0))
    {
        mChildren[0]->InsertObject(_object);
        return true;
    }

    // Bottom Right Back
    else if (InsertIntoChild(_object, NodeBounds(glm::vec3(centre.x, mNodeBounds.mMinExtent.y, mNodeBounds.mMinExtent.z), glm::vec3(mNodeBounds.mMaxExtent.x, centre.y, centre.z)), 1))
    {
        mChildren[1]->InsertObject(_object);
        return true;
    }

    // Top Left Back
    else if (InsertIntoChild(_object, NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, centre.y, mNodeBounds.mMinExtent.z), glm::vec3(centre.x, mNodeBounds.mMaxExtent.y, centre.z)), 2))
    {
        mChildren[2]->InsertObject(_object);
        return true;
    }

    // Top Right Back
    else if (InsertIntoChild(_object, NodeBounds(glm::vec3(centre.x, centre.y, mNodeBounds.mMinExtent.z), glm::vec3(mNodeBounds.mMaxExtent.x, mNodeBounds.mMaxExtent.y, centre.z)), 3))
    {
        mChildren[3]->InsertObject(_object);
        return true;
    }

    // Bottom Left Front
    else if (InsertIntoChild(_object, NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, mNodeBounds.mMinExtent.y, centre.z), glm::vec3(centre.x, centre.y, mNodeBounds.mMaxExtent.z)), 4))
    {
        mChildren[4]->InsertObject(_object);
        return true;
    }

    // Bottom Right Front
    else if (InsertIntoChild(_object, NodeBounds(glm::vec3(centre.x, mNodeBounds.mMinExtent.y, centre.z), glm::vec3(mNodeBounds.mMaxExtent.x, centre.y, mNodeBounds.mMaxExtent.z)), 5))
    {
        mChildren[5]->InsertObject(_object);
        return true;
    }

    // Top Left Front
    else if (InsertIntoChild(_object, NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, centre.y, centre.z), glm::vec3(centre.x, mNodeBounds.mMaxExtent.y, mNodeBounds.mMaxExtent.z)), 6))
    {
        mChildren[6]->InsertObject(_object);
        return true;
    }

    // Top Right Front
    else if (InsertIntoChild(_object, NodeBounds(centre, mNodeBounds.mMaxExtent), 7))
    {
        mChildren[7]->InsertObject(_object);
        return true;
    }

    return false;
}

bool OctTree_Node::InsertIntoChild(std::shared_ptr<BaseActor> _object, NodeBounds _childBounds, const int _childIt)
{
    if (_childBounds.Contains(std::move(_object)))
    {
        if(!mChildren[_childIt])
            mChildren[_childIt] = std::make_shared<OctTree_Node>(_childBounds, mCurrentDepth + 1, shared_from_this(), mActiveLevel);
        return true;
    } 
    return false;
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

void OctTree_Node::InsertObjectIntoRoot(std::shared_ptr<BaseActor> _object)
{
    // Recursively insert _object into root node
    if (mParentNode)
        mParentNode->InsertObjectIntoRoot(_object);
    else
        InsertObject(_object);
}
//
//bool OctTree_Node::Subdivide()
//{
//    // 1. Find the middle of the oct tree node
//    glm::vec3 centre = glm::vec3{
//        (mNodeBounds.mMinExtent.x + mNodeBounds.mMaxExtent.x) / 2.0f,
//		(mNodeBounds.mMinExtent.y + mNodeBounds.mMaxExtent.y) / 2.0f,
//		(mNodeBounds.mMinExtent.z + mNodeBounds.mMaxExtent.z) / 2.0f
//    };
//  
//    // 2. Create and assign each child their sector
//    mChildren[0] = std::make_shared<OctTree_Node>(NodeBounds(mNodeBounds.mMinExtent, centre), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[1] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(centre.x, mNodeBounds.mMinExtent.y, mNodeBounds.mMinExtent.z), glm::vec3(mNodeBounds.mMaxExtent.x, centre.y, centre.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[2] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, centre.y, mNodeBounds.mMinExtent.z), glm::vec3(centre.x, mNodeBounds.mMaxExtent.y, centre.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[3] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(centre.x, centre.y, mNodeBounds.mMinExtent.z), glm::vec3(mNodeBounds.mMaxExtent.x, mNodeBounds.mMaxExtent.y, centre.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[4] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, mNodeBounds.mMinExtent.y, centre.z), glm::vec3(centre.x, centre.y, mNodeBounds.mMaxExtent.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[5] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(centre.x, mNodeBounds.mMinExtent.y, centre.z), glm::vec3(mNodeBounds.mMaxExtent.x, centre.y, mNodeBounds.mMaxExtent.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[6] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, centre.y, centre.z), glm::vec3(centre.x, mNodeBounds.mMaxExtent.y, mNodeBounds.mMaxExtent.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[7] = std::make_shared<OctTree_Node>(NodeBounds(centre, mNodeBounds.mMaxExtent), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//
//    LOG_WARNING("SubDibide");
//
//	mSubdivided = true;
//    // 3. return true for successful subdivide
//    return true;
//}
//
//bool OctTree_Node::Subdivide(std::shared_ptr<BaseActor> _object)
//{
//    // 1. Find the middle of the node
//    glm::vec3 centre = glm::vec3{
//        (mNodeBounds.mMinExtent.x + mNodeBounds.mMaxExtent.x) / 2.0f,
//        (mNodeBounds.mMinExtent.y + mNodeBounds.mMaxExtent.y) / 2.0f,
//        (mNodeBounds.mMinExtent.z + mNodeBounds.mMaxExtent.z) / 2.0f
//    };
//
//
//    NodeBounds childBounds0 = NodeBounds(mNodeBounds.mMinExtent, centre);
//    if(childBounds0.Contains(_object))
//    {
//        mChildren[0] = std::make_shared<OctTree_Node>(childBounds0, mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//        mSubdivided = true;
//    	return true;
//    }
//		 
//    NodeBounds childBounds0 = NodeBounds(mNodeBounds.mMinExtent, centre);
//    else if (childBounds0.Contains(_object))
//        mChildren[0] = std::make_shared<OctTree_Node>(childBounds0, mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//
//    NodeBounds childBounds0 = NodeBounds(mNodeBounds.mMinExtent, centre);
//    if (childBounds0.Contains(_object))
//        mChildren[0] = std::make_shared<OctTree_Node>(childBounds0, mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//
//    NodeBounds childBounds0 = NodeBounds(mNodeBounds.mMinExtent, centre);
//    if (childBounds0.Contains(_object))
//        mChildren[0] = std::make_shared<OctTree_Node>(childBounds0, mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//
//    NodeBounds childBounds0 = NodeBounds(mNodeBounds.mMinExtent, centre);
//    if (childBounds0.Contains(_object))
//        mChildren[0] = std::make_shared<OctTree_Node>(childBounds0, mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//
//    NodeBounds childBounds0 = NodeBounds(mNodeBounds.mMinExtent, centre);
//    if (childBounds0.Contains(_object))
//        mChildren[0] = std::make_shared<OctTree_Node>(childBounds0, mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//
//    NodeBounds childBounds0 = NodeBounds(mNodeBounds.mMinExtent, centre);
//    if (childBounds0.Contains(_object))
//        mChildren[0] = std::make_shared<OctTree_Node>(childBounds0, mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//
//    NodeBounds childBounds0 = NodeBounds(mNodeBounds.mMinExtent, centre);
//    if (childBounds0.Contains(_object))
//        mChildren[0] = std::make_shared<OctTree_Node>(childBounds0, mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//
//    NodeBounds childBounds0 = NodeBounds(mNodeBounds.mMinExtent, centre);
//    if (childBounds0.Contains(_object))
//        mChildren[0] = std::make_shared<OctTree_Node>(childBounds0, mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//
//
//	mChildren[1] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(centre.x, mNodeBounds.mMinExtent.y, mNodeBounds.mMinExtent.z), glm::vec3(mNodeBounds.mMaxExtent.x, centre.y, centre.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[2] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, centre.y, mNodeBounds.mMinExtent.z), glm::vec3(centre.x, mNodeBounds.mMaxExtent.y, centre.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[3] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(centre.x, centre.y, mNodeBounds.mMinExtent.z), glm::vec3(mNodeBounds.mMaxExtent.x, mNodeBounds.mMaxExtent.y, centre.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[4] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, mNodeBounds.mMinExtent.y, centre.z), glm::vec3(centre.x, centre.y, mNodeBounds.mMaxExtent.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[5] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(centre.x, mNodeBounds.mMinExtent.y, centre.z), glm::vec3(mNodeBounds.mMaxExtent.x, centre.y, mNodeBounds.mMaxExtent.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[6] = std::make_shared<OctTree_Node>(NodeBounds(glm::vec3(mNodeBounds.mMinExtent.x, centre.y, centre.z), glm::vec3(centre.x, mNodeBounds.mMaxExtent.y, mNodeBounds.mMaxExtent.z)), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//    mChildren[7] = std::make_shared<OctTree_Node>(NodeBounds(centre, mNodeBounds.mMaxExtent), mCurrentDepth + 1, shared_from_this(), mActiveLevel);
//
//    LOG_WARNING("SubDibide");
//
//    mSubdivided = true;
//    // 3. return true for successful subdivide
//    return true;
//}

std::vector<std::shared_ptr<OctTree_Node>> OctTree_Node::GetChildren()
{
    std::vector<std::shared_ptr<OctTree_Node>> tempR;
    for(std::shared_ptr<OctTree_Node> child : mChildren)
    {
        if (child)
            tempR.push_back(child);
    }
    return tempR;
}

std::pair<glm::vec3, glm::vec3> OctTree_Node::GetMinMaxExtent()
{
    return std::make_pair(mNodeBounds.mMinExtent, mNodeBounds.mMaxExtent);
}
