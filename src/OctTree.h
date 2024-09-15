#pragma once

// Includes

// Additional Includes
#include <memory> 
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <LifeTime.h>

// Forward Declarations
class BaseActor;
class DebugActor;
struct Level;

struct NodeBounds
{
    glm::vec3 mMinExtent;
	glm::vec3 mMaxExtent;
    NodeBounds(glm::vec3 _minExtent, glm::vec3 _maxExtent);


    bool Intersect(std::shared_ptr<BaseActor> _object);

    bool Overlaps(std::shared_ptr<BaseActor> _object);
    bool Contains(std::shared_ptr<BaseActor> _object);
};

/**
 * @class OctTree_Node
 * @brief Custom static class designed to handle object spawning.
 */
class OctTree_Node : public std::enable_shared_from_this<OctTree_Node>, public ILifeTime
{
public:
    // ---------- Global Variables --------------
    
private:
    // ---------- Local Variables --------------

	// Constant static values
    static constexpr int sMaxDepth = 4;
    static constexpr int sMaxObjectsInNode = 1;

    // Node Data 
    NodeBounds mNodeBounds;
    std::shared_ptr<OctTree_Node> mParentNode;
    std::array<std::shared_ptr<OctTree_Node>, 8> mChildren;
    std::vector<std::shared_ptr<BaseActor>> mNodeActors;
    std::shared_ptr<Level> mActiveLevel{ nullptr };
    std::shared_ptr<DebugActor> mDebugActor{ nullptr };
    // Dynamic values
    int mCurrentDepth = 0;
    bool mHasChildren = false;

public:
    // ---------- Global functions --------------
    OctTree_Node(NodeBounds _nodeBounds, int _depth, std::shared_ptr<OctTree_Node> _parent, std::shared_ptr<Level> _activeLevel);

    ~OctTree_Node() override;

    void OctTreeUpdate();

    // Inserts an object into the oct tre, returns true if success or false if fail
    bool InsertObject(std::shared_ptr<BaseActor> _object);

   void InsertObjectIntoRoot(std::shared_ptr<BaseActor> _object);

    //void TransferObjectToNewNode(std::shared_ptr<BaseActor> _actor, std::shared_ptr<OctTree_Node> _insertNode);

    void RemoveObject(std::shared_ptr<BaseActor> _object);

    void RemoveChild(std::shared_ptr<OctTree_Node> _child);

private:
    // ---------- Local functions --------------

    bool CreateAndInsertIntoChildren(std::shared_ptr<BaseActor> _object);

    bool InsertIntoChild(std::shared_ptr<BaseActor> _object, NodeBounds _childBounds, const int _childIt);

    //bool Subdivide();
    //
    //bool Subdivide(std::shared_ptr<BaseActor> _object);

public:
    // ---------- Getters / setters / Adders --------------

    void SetDebugMeshRed();
    void SetDebugMeshGreen();

    constexpr glm::vec3 GetDepthRedColor() { return glm::vec3(1.f / ((float)mCurrentDepth + 1.f), 0.f, 0.f); }
    constexpr glm::vec3 GetDepthGreenColor() { return glm::vec3(0.f, 1.f / ((float)mCurrentDepth + 1.f), 0.f); }
    std::shared_ptr<OctTree_Node> GetParent() { return mParentNode; }
	const bool HasChildren() { return mHasChildren; }
 
    const bool ContainsObjects() { return mNodeActors.empty(); }

	std::vector<std::shared_ptr<OctTree_Node>> GetChildren();
    std::pair<glm::vec3, glm::vec3> GetMinMaxExtent();


};

