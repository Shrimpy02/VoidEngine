#pragma once
// Includes
#include <Actor.h>

// Additional Includes
#include <string>

// Forward Declarations


/**
 * @class SceneGraph
 * @brief Represents a minimal actor in world context, is the top object in scene for render and update reference.
 * Has no functionality other han containing all scene objects as children.
 */
class SceneGraph : public Actor
{

public:
    // basic constructor
    SceneGraph(const std::string& _name) : Actor(_name) { };
};