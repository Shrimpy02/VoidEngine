#pragma once

// Includes
#include <glm/glm.hpp>

// Additional Includes
#include <memory> 
#include <vector>

// Forward Declarations
class VisualActor;
class Actor;
class Mesh;

/**
 * @struct Points
 * @brief A small struct containing positional values
 */
struct Points
{
    glm::vec3 mPosition;

    // constructor
    Points(glm::vec3 _pos) : mPosition(_pos) { }

};

/**
 * @class SMath
 * @brief Custom static math class containing useful mathematical functions
 * for curves and geometrical calculations.
 */
class SMath
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // Static function to create a parametric curve from in num points and in resolution
    static std::vector<Points> CreateParametricCurve(const unsigned int _numPoints, const float _resolution);

	// Static function that conforms an in vector of points y values to the second in visualActors mesh 
    static bool ConformCurveToGeometry(std::vector<Points>& _points, std::shared_ptr<VisualActor> _surface);

    static bool ConformObjectToGeometry(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float _offsettHeight = 0.f);

    static bool IsWithinBarycentricCoordinates(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float& _height);

    static bool IsWithinSurfaceExtent(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float _heightExtent);

    template <typename T>
    static glm::vec3 GetActorMeshExtent(std::shared_ptr<T> _actor);

    static glm::vec3 GetMeshExtent(std::shared_ptr<Mesh> _mesh);

    template <typename T>
    static glm::vec3 GetActorMeshExtentLength(std::shared_ptr<T> _actor);

    static float GetMeshExtentLength(std::shared_ptr<Mesh> _mesh);

    // Static function returns the barycentric coordinate between three triangle points as a vec3
    static glm::vec3 GetBarycentricCoordinates(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _actorPos);

    // Static function returns the height of a barycentric coordinate in a tringle of three points
    static float GetHightFromBarycentricCoordinates(const glm::vec3& _barCoords, const  glm::vec3& _p1, const  glm::vec3& _p2, const  glm::vec3& _p3);


private:
    // ---------- Local functions --------------

    // A Generic 2d spiral parametric function 
    static glm::vec3 Generic2DParametricFunction(const double _t);


public:
    // ---------- Getters / setters / Adders --------------



};

