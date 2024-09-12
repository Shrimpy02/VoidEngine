#pragma once

// Includes

// Additional Includes
#include <memory> 
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <LevelActors/BaseActor.h>

// Forward Declarations
class BaseActor;
struct Level;
class DebugActor;

struct NodeBounds
{
    glm::vec3 mMinExtent;
	glm::vec3 mMaxExtent;
    NodeBounds(glm::vec3 _minExtent, glm::vec3 _maxExtent)
	: mMinExtent(_minExtent), mMaxExtent(_maxExtent) {}

    bool Intersect(std::shared_ptr<BaseActor> _object)
    {
        return _object->AABBxBoundingSphere(std::make_pair(mMinExtent,mMaxExtent));
    }
    
    bool Contains(std::shared_ptr<BaseActor> _object)
    {
        if (_object)
            return _object->BoundingSpherexConstrictingBox(mMinExtent, mMaxExtent);
        
        return false;
    }

    bool ContainsPos(std::shared_ptr<BaseActor> _object)
    {
        return _object->BoundingSpherePosxConstrictingBox(mMinExtent, mMaxExtent);
        return false;
    }
};


/**
 * @class OctTree_Node
 * @brief Custom static class designed to handle object spawning.
 */
class OctTree_Node
{
public:
    // ---------- Global Variables --------------
    
private:
    // ---------- Local Variables --------------

	// Constant static values
    static constexpr int sMaxDepth = 5;
    static constexpr int sMaxObjectsInNode = 1;

    // Node Data 
    NodeBounds mNodeBounds;
    std::array<std::shared_ptr<OctTree_Node>, 8> mChildren;
    std::vector<std::shared_ptr<BaseActor>> mNodeActors;
    std::shared_ptr<Level> mActiveLevel{ nullptr };
    std::shared_ptr<DebugActor> mDebugActor{ nullptr };
    // Dynamic values
    int mCurrentDepth = 0;
    bool mSubdivided = false;

public:
    // ---------- Global functions --------------
    OctTree_Node(NodeBounds _nodeBounds, int _depth, std::shared_ptr<Level> _activeLevel);

    ~OctTree_Node();

    // Inserts an object into the oct tre, returns true if success or false if fail
    bool InsertObject(std::shared_ptr<BaseActor> _object);


private:
    // ---------- Local functions --------------

    bool Subdivide();

public:
    // ---------- Getters / setters / Adders --------------

    std::pair<glm::vec3, glm::vec3> GetMinMaxExtent();
};

