#pragma once

// Includes
#include <Utilities/Types.h>

// Additional Includes
#include <glm/glm.hpp>
#include <memory> 
#include <vector>

// Forward Declarations
class VisualActor;
class Actor;
class Mesh;
class GraphPoint;
class PointCloudMesh;
struct Vertex;

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
    static bool ConformPhysicsObjectToGeometry(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, glm::vec3& _normals, float& _faceFriction, float _offsettHeight = 0.f);

    static bool ConformObjectToGeometry(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, std::vector<glm::vec3>& _debugSurfacePoints, float _offsettHeight = 0.f);

    static bool ConformPointToGeometry(glm::vec3& _point, std::shared_ptr<VisualActor> _surface, float _offsettHeight = 0.f);

    static bool IsWithinBarycentricCoordinates(glm::vec3 _point, std::shared_ptr<VisualActor> _surface, float& _height);

    // Static function returns true if _object is within the barycentric coordinates of _surface
	static bool IsWithinBarycentricCoordinates(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float& _height);

    static bool IsWithinBarycentricCoordinates(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float& _height, glm::vec3& _normal, float& _faceFriction);

    // Static function returns true if _object is within the barycentric coordinates of _surface
    static bool IsWithinBarycentricCoordinates(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float& _height, std::vector<glm::vec3>& _debugSurfacePoints);

    // File reading

    static void LASFileReadingExample(const char* _fileNameIn, const char* _fileNameOut);

    static std::vector<glm::vec3> LASFileToPoints(const char* _fileDirectory);

    static void LASFileToCustomFileOfPoints(const char* _fileDirectoryInn, const char* _fileDirectoryOut);

    static void CenterPointCloudStructureToWorldZero(std::shared_ptr<Actor> _terrainParent);

    static void MovePointCloudPoints(std::shared_ptr<VisualActor> _terrain, glm::vec3 _moveBy);

    // Surface ---

    // The recursive basis function for B-Spline
    // Arg1: u = Parameter value for u direction.
    // Arg2: v = Parameter value for V direction.
    // Arg3: du = Degree in u direction.
    // Arg4: dv = Degree in v direction.
    // Arg5: uKnot = Knot vector in u direction.
    // Arg6: vKnot = Knot vector in v direction.
    // Arg7: controlPoints = control points. 
    static glm::vec3 EvaluateBSplineSurface(float _u, float _v, int _du, int _dv, const std::vector<float>& _uKnot, const std::vector<float>& _vKnot, const std::vector<std::vector<glm::vec3>>& _controlPoints);

    static glm::vec3 EvaluateBSplineNormal(float _u, float _v, int _du, int _dv, int _UResolution, int _VResolution, const std::vector<float>& _uKnot, const std::vector<float>& _vKnot, const std::vector<std::vector<glm::vec3>>& _controlPoints);

    static std::vector<glm::vec3> BSplineFromPoints(const std::vector<std::shared_ptr<GraphPoint>>& _controlPoints, const float& _step, const int& _dimension);

    static std::vector<float> GenerateClampedKnotVector(int _numControlPointsInDirection, int _degree);

    static std::vector<float> GenerateUniformKnotVector(int _numControlPoints, int _degree);

    // Mesh operations -----

    static void AdjustVertexCoordinates(std::shared_ptr<PointCloudMesh> _pcm, glm::vec3 _offsett);

    static void MergeVerticesByXZ(std::vector<glm::vec3>& vertices, const int _quality = 1);

    static void MergeVerticesXZ(std::vector<glm::vec3>& vertices);

    static bool PointXZOverlapsWithVertex(const glm::vec3& _pos1, const glm::vec3& _pos2, const float _epsilon = 0.001f);

    static void UpdateVerticesNormal(std::vector<Vertex>& _vertices, std::vector<Index>& _indices);

    static bool PointWithinArea(glm::vec3 _point, glm::vec3 _minExtent, glm::vec3 _maxExtent);

    static bool PointWithinArea(glm::vec3 _point, glm::vec3 _minExtent, glm::vec3 _maxExtent, float _yOveride);

    static void UpdateIndex(std::vector<glm::vec3>& vertexPointsForNewMesh, std::vector<Index>& indices, const glm::vec3& vert1, const glm::vec3& vert2, const glm::vec3& vert3, const glm::vec3& vert4);

	// Curves ------

    static std::vector<glm::vec3> NevillInterpolatedPoints(const std::vector<std::shared_ptr<GraphPoint>>& _controlPoints, const float _step = 0.25f);
    static std::vector<glm::vec3> DeCastApproximationPoints(const std::vector<std::shared_ptr<GraphPoint>>& _controlPoints, const float _step = 0.25f);

    static void SeedRandTime();

    static int GetRandomInt(const int& _offset, const int& _range);

    static float GetRandomFloat(const float& _offset, const float& _range);

    static float GetRandomFloatBetweenMinMax(const float& _min, const float& _max);

private:
    // ---------- Local functions --------------

    // Static function returns a vector of vec3 locations as an approximated parametric curve from DeCastalejau`s algorithm by the two given points.
    static std::vector<glm::vec3> DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, float _step);
    // Static function returns a vector of vec3 locations as an approximated parametric curve from DeCastalejau`s algorithm by the three given points.
    static std::vector<glm::vec3> DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, float _step);
    // Static function returns a vector of vec3 locations as an approximated parametric curve from DeCastalejau`s algorithm by the four given points.
    static std::vector<glm::vec3> DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, glm::vec3 _point4, float _step);

    // Static function returns a vector of vec3 locations as an interpolated parametric curve from Neville`s algorithm by the tree given points.
    static std::vector<glm::vec3> NevillInterpolatedParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, float _step = 0.25f);

    // The recursive basis function for B-Spline
    // Arg1: i = Control point index,
    // Arg2: d = degree of B-spline base function
    // Arg3: uv = Parameter value, between 0 and 1 for how far along the B-spline surface we are calculating
    // Arg4: KnotVector = Sequence of parameter values that define control point influence
	static float CoxDeBoorRecursive(int _i, int _d, float _uv, const std::vector<float>& _knotVector);

	// Static helper function returns the barycentric coordinate between three triangle points given by a _objectPos as a vec3
    static glm::vec3 GetBarycentricCoordinates(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _objectPos);

    static glm::vec3 GetTriangleNormalizedNormal(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3);

    // Static helper function returns the height of a barycentric coordinate in a triangle of three points
    static float GetHeightFromBarycentricCoordinates(const glm::vec3& _barCoords, const  glm::vec3& _p1, const  glm::vec3& _p2, const  glm::vec3& _p3);

    static float GetTriangleFriction(float _p1, float _p2, float _p3);

    // Static helper function returns true of _object is within the X and Z extents of _surface
    static bool IsWithinTerrainXZExtent(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface);

    static bool IsWithinTerrainXZExtent(const glm::vec3& _point, std::shared_ptr<VisualActor> _surface);

    // Mesh:

    static int FindOrAddVertex(std::vector<glm::vec3>& vertices, const glm::vec3& pos);

public:
    // ---------- Getters / setters / Adders --------------



};

