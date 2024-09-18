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

/**
 * @struct NodeBounds 
 * @brief Struct containing information about the bounds of a node and
 * collision functions for either intersecting or containing and object.
 */
struct NodeBounds
{
    // Global variables
    glm::vec3 mMinExtent;
	glm::vec3 mMaxExtent;
	
    // Constructors 
	NodeBounds() = default;
    NodeBounds(glm::vec3 _minExtent, glm::vec3 _maxExtent);

    // Returns true if _object intersects this node
    bool Intersect(std::shared_ptr<BaseActor> _object);

    // Returns true if _object is completely contained within this node
    bool HardContains(std::shared_ptr<BaseActor> _object);
};

/**
 * @class OctTree_Node
 * @brief Class containing all logic required for a functional oct tree. This implementation
 * involves storing objects that are completely contained and overlapping in separate vectors,
 * and dealing with their logic recursively up or down the node structure. 
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
    std::vector<std::shared_ptr<BaseActor>> mContainedNodeActors;
    std::vector<std::shared_ptr<BaseActor>> mIntersectingActors;

    // Debug Data
    std::shared_ptr<Level> mActiveLevel{ nullptr };
    std::shared_ptr<DebugActor> mDebugActor{ nullptr }; 

    // Dynamic values
    int mCurrentDepth = 0;
    bool mHasChildren = false;

public:
    // ---------- Global functions --------------

    // Constructor, fills children array and initialize debug visualization
    OctTree_Node(NodeBounds _nodeBounds, int _depth, std::shared_ptr<OctTree_Node> _parent, std::shared_ptr<Level> _activeLevel);

    // De-constructor, unloads debug actor and node from scene
    ~OctTree_Node() override;

    // Updates each node recursively down the tree from the node it is called. (most applicable is the root node)
    // It first update each leaf nodes contained objects, then their overlapping objects.
    void OctTreeUpdate();

    // Checks if object is hard contained or intersects inserted node, if not leaf-node it finds the appropriate leaf-node
    // through this nodes children an inserts it there. returns true if inserted, false if not
    bool InsertObject(std::shared_ptr<BaseActor> _object);

    // Calls Insert object recursively up to the root node of the oct tree. 
    void InsertObjectIntoRoot(std::shared_ptr<BaseActor> _object);

    // Removes the input _object from the contained object vector for the current node.  
    void RemoveContainedObject(std::shared_ptr<BaseActor> _object);

    // Removes the input _object from the intersecting object vector for the current node.  
    void RemoveIntersectObject(std::shared_ptr<BaseActor> _object);

    // Removes the input child from a node. 
    void RemoveChild(std::shared_ptr<OctTree_Node> _child);

    // Adjusts and resize all nodes node bounds based on new input bounds. 
    void Resize(NodeBounds _nodeBounds);

private:
    // ---------- Local functions --------------

    // Insert object into contain vector, if not leaf find a leaf it fits in.
    // Returns true if object is inserted false if it is not.
    bool InsertContainedObject(std::shared_ptr<BaseActor> _object);

    // Insert object into intersect vector, if not leaf find a leaf it fits in.
    // Returns true if object is inserted false if it is not.
    bool InsertIntersectObject(std::shared_ptr<BaseActor> _object);

    // Helper function to InsertIntersectObject that adds the intersecting actor to the array and
   // then checks all surrounding nodes if they overlap with the object as well. returns true if object added, false if not.
    bool InsertIntersectIntoNeighbors(std::shared_ptr<BaseActor> _object, std::shared_ptr<OctTree_Node> _excludeNode = nullptr);

    // Helper function to InsertIntersectIntoNeighbors that adds the object to the intersect array if it is a leaf.
    // If it is not it finds the appropriate leaf and inserts it. Returns true if inserted false if not. 
    bool AddOverlapObject(std::shared_ptr<BaseActor> _object);

    // checks if _object is contained in any children, if it does it creates and inserts into that child and returns true.
    // if not it checks if it intersects any of the children, if it does it inserts into that child and returns true.
    // if neither are true it returns false.
    bool CreateAndInsertIntoChildren(std::shared_ptr<BaseActor> _object);

    // Returns the bounds of the child given input parent bounds and the iterator of what child this function is regarding.
    NodeBounds GetChildBounds(NodeBounds& _parentBounds, const int _it);

public:
    // ---------- Getters / setters / Adders --------------

    // Setters ----

    // Sets the debug meshes color to Red. 
    void SetDebugMeshRed();
    // Sets the debug meshes color to Green. 
    void SetDebugMeshGreen();
    // Sets the debug meshes color to Yellow. 
    void SetDebugMeshYellow();

    // Getters ----

	// Returns a smart pointer to this nodes parent. 
    std::shared_ptr<OctTree_Node> GetParent() { return mParentNode; }
    // Returns a vector of smart pointers to each active children of this node. 
    std::vector<std::shared_ptr<OctTree_Node>> GetActiveChildren();
    // Returns a vector of smart pointers to each child of this node. 
    std::vector<std::shared_ptr<OctTree_Node>> GetAllChildren();
    // Returns this nodes extent as a pair of vec3. 
    std::pair<glm::vec3, glm::vec3> GetMinMaxExtent();

    // Returns a vec3 with the color Red given the depth of the node.
    constexpr glm::vec3 GetDepthRedColor() { return glm::vec3(1.f / ((float)mCurrentDepth + 1.f), 0.f, 0.f); }
    // Returns a vec3 with the color Green given the depth of the node.
	constexpr glm::vec3 GetDepthGreenColor() { return glm::vec3(0.f, 1.f / ((float)mCurrentDepth + 1.f), 0.f); }
    // Returns a vec3 with the color Yellow given the depth of the node.
	constexpr glm::vec3 GetDepthYellowColor() { return glm::vec3(1.f / ((float)mCurrentDepth + 1.f), 1.f / ((float)mCurrentDepth + 1.f), 0.f); }

    // Checkers ----

	// Returns true if this node has any children
	constexpr bool HasChildren() { return mHasChildren; }
    // Returns true if this node contains any contained or intersecting objects.
    constexpr bool ContainsObjects() { return !(mContainedNodeActors.empty() && mIntersectingActors.empty()); }
};

