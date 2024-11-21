#pragma once

// Includes

// Additional Includes
#include <memory> 
#include <vector>

// Forward Declarations
struct Level;
class Actor;
class OctTree_Node;

/**
 * @class SLifeTime
 * @brief Custom static class designed to handle object lifetime
 */
class SLifeTime
{
public:
    // ---------- Global Functions --------------

    inline static float SActorLifeTime = 2.0f;
    static void ManageDebugActorLifeTime(std::shared_ptr<Level> _level, std::vector<std::shared_ptr<Actor>> _lifeTimeActors);

    inline static float SNodeLifeTime = 0.5f;
    static void ManageOctTreeNodeLifeTime(std::shared_ptr<Level> _level, std::shared_ptr<OctTree_Node> _octTreeNode);

    inline static float SControlPointInterval = 2.f;
    static void ManagePhysicsPathControlPointLifeTime(std::vector<std::shared_ptr<Actor>> _lifeTimeActors);

private:
    // ---------- Local functions --------------

};

