#pragma once

// Includes

// Additional Includes
#include <glm/glm.hpp>
#include <memory> 
#include <vector>

// Forward Declarations
class VisualActor;
class Actor;
class Mesh;

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

	// Static function that conforms an in vector of actors y values to a visual actor _surface with the sett height offset. It returns true if all actors where conformed correctly.
    static bool ConformCurveToGeometry(std::vector<std::shared_ptr<Actor>>& _points, std::shared_ptr<VisualActor> _surface, float _offsettHeight = 0.f);

    // Static function that conforms an actor _object to a visual actor _surface with the sett height offset. It returns true if _object is in contact with _surface.
    static bool ConformObjectToGeometry(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, std::vector<glm::vec3>& _debugSurfacePoints, float _offsettHeight = 0.f);

    // Static function returns true if _object is within the barycentric coordinates of _surface
    static bool IsWithinBarycentricCoordinates(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float& _height);

    // Static function returns true if _object is within the barycentric coordinates of _surface
    static bool IsWithinBarycentricCoordinates(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float& _height, std::vector<glm::vec3>& _debugSurfacePoints);

    // Curves ------
    // Static function returns a vector of vec3 locations as an interpolated parametric curve from Neville`s algorithm by the tree given points.
    static std::vector<glm::vec3> NevillParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, float _step = 0.25f);

	// Static function returns a vector of vec3 locations as an approximated parametric curve from DeCastalejau`s algorithm by the two given points.
    static std::vector<glm::vec3> DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, float _step = 0.25f);
    // Static function returns a vector of vec3 locations as an approximated parametric curve from DeCastalejau`s algorithm by the three given points.
	static std::vector<glm::vec3> DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, float _step = 0.25f);
    // Static function returns a vector of vec3 locations as an approximated parametric curve from DeCastalejau`s algorithm by the four given points.
    static std::vector<glm::vec3> DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, glm::vec3 _point4, float _step = 0.25f);

private:
    // ---------- Local functions --------------

	// Static helper function returns the barycentric coordinate between three triangle points given by a _objectPos as a vec3
    static glm::vec3 GetBarycentricCoordinates(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _objectPos);

    // Static helper function returns the height of a barycentric coordinate in a triangle of three points
    static float GetHeightFromBarycentricCoordinates(const glm::vec3& _barCoords, const  glm::vec3& _p1, const  glm::vec3& _p2, const  glm::vec3& _p3);

    // Static helper function returns true of _object is within the X and Z extents of _surface
    static bool IsWithinTerrainXZExtent(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface);


public:
    // ---------- Getters / setters / Adders --------------



};

