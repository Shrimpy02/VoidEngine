
// Includes
#include <Core/SMath.h>
#include <glm/glm.hpp>
#include <SceneActors.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/Vertex.h>

// Additional includes
#include <stdexcept>

// TODO : Fix this function so it can take a different function as input. for custom curves.
std::vector<Points> SMath::CreateParametricCurve(const unsigned int _numPoints, const float _resolution)
{
	std::vector<Points> points;

    // Creates a parametric curve from with the given number of points and curve resolution

	for(float i = 0; i < _numPoints; i += _resolution)
	{
        // For now only creates a basic hard coded 2D parametric function
		points.push_back(Points(Generic2DParametricFunction(i)));
	}

	return points;
}

void SMath::ConformCurveToGeometry(std::vector<Points>& _points, std::shared_ptr<VisualActor> _mesh)
{
    if (!_mesh) return;

    // gets the mesh geometry from the visual actor 
    std::shared_ptr<Mesh> conformGeometry = _mesh->GetActorMesh();

    // Gets the vertices for positional values and indices for triangle point pairing.
    std::vector<Vertex>& planeVertices = conformGeometry->GetVertices();
    std::vector<Index>& planeIndices = conformGeometry->GetIndices();

    // iterates trough each point
    for (int j = 1; j < _points.size(); j++)
    {
        // iterates trough each triangle to find the correct one to use for conforming point
        for (int i = 0; i < planeIndices.size() - 2; i += 3)
        {
            // Finds triangles in order in pairs of three by indices index
            unsigned int index1 = planeIndices[i];
            unsigned int index2 = planeIndices[i + 1];
            unsigned int index3 = planeIndices[i + 2];

            // Gets the vertices position belonging to the respective indexes
            glm::vec3 point1(planeVertices[index1].mPosition);
            glm::vec3 point2(planeVertices[index2].mPosition);
            glm::vec3 point3(planeVertices[index3].mPosition);

            // * actor world scale for live scaling of terrain
            point1 *= _mesh->GetGlobalScale();
            point2 *= _mesh->GetGlobalScale();
            point3 *= _mesh->GetGlobalScale();

            // perhaps + actor world position for live moveing of terrain, however this messes with height calculations because of uneven positioning
        	//point1 += _mesh->GetPosition(Actor::TransformSpace::Global);
        	//point2 += _mesh->GetPosition(Actor::TransformSpace::Global);
        	//point3 += _mesh->GetPosition(Actor::TransformSpace::Global);

            // actor world rotation for live rotating of terrain
            glm::quat planesRotation = _mesh->GetGlobalRotation();
            point1 = glm::rotate(planesRotation, point1);
            point2 = glm::rotate(planesRotation, point2);
            point3 = glm::rotate(planesRotation, point3);

            // Calculate the Barycentric Coordinates of the components parent with the world transform of the three points.
            glm::vec3 baryCoords = GetBarycentricCoordinates(point1, point2, point3, _points[j].mPosition);

            // check if point is in correct triangle
            if (baryCoords.x > 0 && baryCoords.x < 1 &&
                baryCoords.y > 0 && baryCoords.y < 1 &&
                baryCoords.z > 0 && baryCoords.z < 1)
            {
                // get the height by Barycentric Coordinates
                float height = GetHightFromBarycentricCoordinates(baryCoords, point1, point2, point3);
                // dirty hardcoded fix for height differential 
            	height += -4;
                // Set point height to triangle height + small off sett
                // Since it got the points array by address we change the value directly so no return required
                _points[j].mPosition.y = height + 0.2f;
            }
        }
    }

}

glm::vec3 SMath::Generic2DParametricFunction(const double _t)
{
	glm::vec3 point;

    // generic spiral like function
    // ingnoer y since its a 2d function
	point.x = (float)_t * (float) cos(_t);
	point.z = (float)_t * (float) sin(_t);

	return point;
}


glm::vec3 SMath::GetBarycentricCoordinates(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _trianglePos)
{
    // Sets all height to 0 so they do not interfere with calculations
    _p1.y = 0;
    _p2.y = 0;
    _p3.y = 0;
    _trianglePos.y = 0;

    // Gets the vectors from point 1 to point 2 and 3
    glm::vec3 p12 = _p2 - _p1;
    glm::vec3 p13 = _p3 - _p1;

    // Gets the normal of these two vectors
    glm::vec3 n = glm::cross(p12, p13);
    // size of cross product = the triangles total area
    float area_123 = n.y;

    // make sure there is no divide by 0 error
    if (area_123 == 0)
        throw std::runtime_error("ERROR::DivideBy0");

    glm::vec3 barCoords; // the coords to return

    // Sub triangle 1
    // Gets the vector between actor's position in triangle and point 2 and point 3 
    glm::vec3 p = _p2 - _trianglePos;
    glm::vec3 q = _p3 - _trianglePos;
    // gets the normal
    n = glm::cross(p, q);
    // gets the area of sub triangle
    barCoords.x = n.y / area_123;

    // Sub triangle 2
    p = _p3 - _trianglePos;
    q = _p1 - _trianglePos;
    n = glm::cross(p, q);
    barCoords.y = n.y / area_123;

    // Sub triangle 3
    p = _p1 - _trianglePos;
    q = _p2 - _trianglePos;
    n = glm::cross(p, q);
    barCoords.z = n.y / area_123;

    return barCoords;
}

float SMath::GetHightFromBarycentricCoordinates(const glm::vec3& _barCoords, const glm::vec3& _p1, const glm::vec3& _p2, const glm::vec3& _p3)
{
    // Generic function to return height by barycentric coordinates and triangle points
    float hight = _barCoords.x * _p1.y + _barCoords.y * _p2.y + _barCoords.z * _p3.y;
    return hight;

}