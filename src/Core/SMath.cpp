
// Includes
#include <Core/SMath.h>
#include <glm/glm.hpp>
#include <SceneActors.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/Vertex.h>
#include <Utilities/Logger.h>

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

bool SMath::ConformCurveToGeometry(std::vector<Points>& _points, std::shared_ptr<VisualActor> _surface)
{
    bool isInContact = false;

    if (!_surface) isInContact;

    // gets the mesh geometry from the visual actor 
    std::shared_ptr<Mesh> conformGeometry = _surface->GetActorVisualMesh();

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
            point1 *= _surface->GetGlobalScale();
            point2 *= _surface->GetGlobalScale();
            point3 *= _surface->GetGlobalScale();

            // perhaps + actor world position for live moveing of terrain, however this messes with height calculations because of uneven positioning
        	//point1 += _mesh->GetPosition(Actor::TransformSpace::Global);
        	//point2 += _mesh->GetPosition(Actor::TransformSpace::Global);
        	//point3 += _mesh->GetPosition(Actor::TransformSpace::Global);

            // actor world rotation for live rotating of terrain
            glm::quat planesRotation = _surface->GetGlobalRotation();
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
                return isInContact;
            }
        }
    }

}

bool SMath::ConformObjectToGeometry(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float _offsettHeight)
{
    bool isInContact = false;
    float locationOnPlaneHeight(0);

    // Returns if there is no ground in scene
    if (!_surface) return isInContact;

    
    if(IsWithinBarycentricCoordinates(_object, _surface, locationOnPlaneHeight))
    {
        // If owning actor is bellow the height of the collision, set it to height of collision so it conforms to ground geometry.
        if (_object->GetGlobalPosition().y < locationOnPlaneHeight)
        {
            isInContact = true;

            // Gets the owners position and overwrites the height with new collision value
            glm::vec3 playPos = _object->GetGlobalPosition();
            playPos.y = locationOnPlaneHeight + _offsettHeight;

            // Set the owners position
            _object->SetGlobalPosition(playPos);
            return isInContact;
        }
    }

    return isInContact;
}

bool SMath::IsWithinBarycentricCoordinates(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float& _height)
{
    // Gets the surface mesh
    std::shared_ptr<Mesh> groundPlane = _surface->GetActorVisualMesh();

    // Gets the surface vertices and indices
    std::vector<Vertex>& planeVertices = groundPlane->GetVertices();
    std::vector<Index>& planeIndices = groundPlane->GetIndices();

    // Gets object position
    glm::vec3 actorPos(_object->GetGlobalPosition());

    //------------------------------
    std::pair<glm::vec3, glm::vec3> minMaxExtent = Mesh::GetMeshMinMaxExtent(groundPlane);

    minMaxExtent.first *= _surface->GetGlobalScale();
    minMaxExtent.second *= _surface->GetGlobalScale();

    minMaxExtent.first += _surface->GetGlobalPosition();
    minMaxExtent.second += _surface->GetGlobalPosition();

    glm::quat rotation = _surface->GetGlobalRotation();
    minMaxExtent.first = glm::rotate(rotation, minMaxExtent.first);
    minMaxExtent.second = glm::rotate(rotation, minMaxExtent.second);

    if ((_object->GetGlobalPosition().x > minMaxExtent.first.x && _object->GetGlobalPosition().x < minMaxExtent.second.x) && 
        (_object->GetGlobalPosition().z > minMaxExtent.first.z && _object->GetGlobalPosition().z < minMaxExtent.second.z))
        //&& ((_object->GetGlobalPosition().y + _object->GetExtent().y) > _surface->GetGlobalPosition().y && (_object->GetGlobalPosition().y + _object->GetExtent().y) > _surface->GetGlobalPosition().y))
        std::cout << "within x, y, z" << std::endl;
    else
        std::cout << "not within x, y, z" << std::endl;
    //------------------------------


    // Iterates through indices in pairs of three to calc one triangle at a time
    for (int i = 0; i < planeIndices.size() - 2; i += 3)
    {
        // Finds triangles in order in pairs of three by indices index
        unsigned int index1 = planeIndices[i];
        unsigned int index2 = planeIndices[i + 1];
        unsigned int index3 = planeIndices[i + 2];

        // Gets the vertices position belonging to the respective indexes
        glm::vec3 point1Pos(planeVertices[index1].mPosition);
        glm::vec3 point2Pos(planeVertices[index2].mPosition);
        glm::vec3 point3Pos(planeVertices[index3].mPosition);

        // Applies scene scale to points
        point1Pos *= _surface->GetGlobalScale();
        point2Pos *= _surface->GetGlobalScale();
        point3Pos *= _surface->GetGlobalScale();

        // Applies scene location to points
        point1Pos += _surface->GetGlobalPosition();
        point2Pos += _surface->GetGlobalPosition();
        point3Pos += _surface->GetGlobalPosition();

        // Applies scene rotation to points
        glm::quat planesRotation = _surface->GetGlobalRotation();
        point1Pos = glm::rotate(planesRotation, point1Pos);
        point2Pos = glm::rotate(planesRotation, point2Pos);
        point3Pos = glm::rotate(planesRotation, point3Pos);

        // Calculate the Barycentric Coordinates of the components parent with the world transform of the three points.
        glm::vec3 baryCoords = GetBarycentricCoordinates(point1Pos, point2Pos, point3Pos, actorPos);

        // If object is on edge move object slightly and return
        if(baryCoords.x == 0 || baryCoords.y == 0 || baryCoords.z == 0)
        {
            _object->SetGlobalPosition(_object->GetGlobalPosition() + glm::vec3(0.01f, 0.f, 0.01f));
            return false;
        }

        // If object pos is within these limits it is within tested triangle
    	if (baryCoords.x > 0 && baryCoords.x < 1 &&
            baryCoords.y > 0 && baryCoords.y < 1 &&
            baryCoords.z > 0 && baryCoords.z < 1)
        {
            // Log triangle index
            //std::cout << "Actor within triangle = " << index1 << " " << index2 << " " << index3 << std::endl;

            // Calculates the barycentric coordinate height
            _height = GetHightFromBarycentricCoordinates(baryCoords, point1Pos, point2Pos, point3Pos);

            // Adds global height to calculations since it ignores them in BarycentricCoordinates calculations
            _height += _surface->GetGlobalPosition().y;

            return true;
        }
    }

    return false;
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
template <typename T>
glm::vec3 SMath::GetActorMeshExtent(std::shared_ptr<T> _actor)
{
    // Calculate the bounding box (min and max extents) of the existing mesh
    std::vector<Vertex>& collisionMeshVertices = _actor->GetActorVisualMesh->GetVertices();
    glm::vec3 maxExtent = collisionMeshVertices[0].mPosition;
    glm::vec3 minExtent = collisionMeshVertices[0].mPosition;

    for (Vertex& vertex : collisionMeshVertices)
    {
        minExtent = glm::min(minExtent, vertex.mPosition);
        maxExtent = glm::max(maxExtent, vertex.mPosition);
    }

    glm::quat actorRotation = _actor->GetGlobalRotation();

    // Get the total extent
    glm::vec3 extent = (maxExtent - minExtent) * 0.5f * _actor->GetGlobalScale();

    // Rotate the total extent by the actor's rotation for each axis
    glm::vec3 xAxis = actorRotation * glm::vec3(extent.x, 0.0f, 0.0f);
    glm::vec3 yAxis = actorRotation * glm::vec3(0.0f, extent.y, 0.0f);
    glm::vec3 zAxis = actorRotation * glm::vec3(0.0f, 0.0f, extent.z);

    // Calculate new rotated extent
    glm::vec3 rotatedExtents(
        glm::abs(xAxis.x) + glm::abs(yAxis.x) + glm::abs(zAxis.x),
        glm::abs(xAxis.y) + glm::abs(yAxis.y) + glm::abs(zAxis.y),
        glm::abs(xAxis.z) + glm::abs(yAxis.z) + glm::abs(zAxis.z)
    );

    return rotatedExtents;

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
        //return glm::vec3(0);
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