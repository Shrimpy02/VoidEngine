#pragma once

// Includes

// Additional Includes
#include <memory> 
#include <vector>
#include <glm/vec3.hpp>

// Forward Declarations
class VisualActor;
class BaseActor;
class Actor;

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
    static void SetObjectLocationWithinBoundsRandomlyIgnoreY(std::shared_ptr<BaseActor> _object, std::shared_ptr<VisualActor> _confineObject);
    static void SetObjectLocationWithinInSphere(std::shared_ptr<Actor> _object, glm::vec3 _centre, float _radius);
private:
    // ---------- Local functions --------------

};

