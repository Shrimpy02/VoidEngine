#pragma once

// Includes

// Additional Includes
#include <memory> 
#include <vector>

// Forward Declarations
class VisualActor;
class BaseActor;

/**
 * @class SSpawner
 * @brief Custom static class designed to handle object spawning.
 */
class SSpawner
{
public:
    // ---------- Global functions --------------

    static void SetObjectLocationWithinBoundsRandomly(std::vector<std::shared_ptr<BaseActor>>& _objects, std::shared_ptr<VisualActor> _confineObject);
    static void SetObjectLocationWithinBoundsRandomly(std::shared_ptr<BaseActor> _object, std::shared_ptr<VisualActor> _confineObject);

private:
    // ---------- Local functions --------------

};

