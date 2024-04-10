#pragma once

// Includes
#include <vector>
#include "glm/glm.hpp"

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
    static void ConformCurveToGeometry(std::vector<Points>& _points, class VisualActor* _mesh);

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