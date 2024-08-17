
// Includes
#include <Core/SMath.h>
#include <LevelActors/BaseActor.h>
#include <LevelActors/VisualActor.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/Vertex.h>
#include <Utilities/Logger.h>

// Additional includes
#include <stdexcept>

bool SMath::ConformCurveToGeometry(std::vector<std::shared_ptr<Actor>>& _points, std::shared_ptr<VisualActor> _surface, float _offsettHeight)
{
    bool isInContact = false;

    if (!_surface) return isInContact;

    for(std::shared_ptr<Actor> point : _points)
    {
        float locationOnPlaneHeight(0);
        if (IsWithinBarycentricCoordinates(point, _surface, locationOnPlaneHeight))
        {
            // Gets the owners position and overwrites the height with new collision value
            glm::vec3 pointPos = point->GetGlobalPosition();
            pointPos.y = locationOnPlaneHeight + _offsettHeight;

            // Set the owners position
            point->SetGlobalPosition(pointPos);
            isInContact = true;
        }
    }

    return isInContact;
}

bool SMath::ConformObjectToGeometry(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, std::vector<glm::vec3>& _debugSurfacePoints, float _offsettHeight)
{
    bool isInContact = false;
    float locationOnPlaneHeight(0);

    if (!_surface) return isInContact;

    if(std::shared_ptr<BaseActor> baseActor = std::dynamic_pointer_cast<BaseActor>(_object))
    {
        if (IsWithinBarycentricCoordinates(baseActor, _surface, locationOnPlaneHeight, _debugSurfacePoints))
        {
            // If owning actor is bellow surface
            if (baseActor->GetGlobalPosition().y < locationOnPlaneHeight + _offsettHeight + baseActor->GetExtent().y)
            {
                isInContact = true;

                glm::vec3 playPos = baseActor->GetGlobalPosition();
                playPos.y = locationOnPlaneHeight + _offsettHeight + baseActor->GetExtent().y;
                baseActor->SetGlobalPosition(playPos);

                return isInContact;
            }
        }

        // If _object is just an actor do default logic
    } else if (IsWithinBarycentricCoordinates(_object, _surface, locationOnPlaneHeight, _debugSurfacePoints)) {

        // If owning actor is bellow surface
        if (_object->GetGlobalPosition().y < locationOnPlaneHeight + _offsettHeight)
        {
            isInContact = true;

            glm::vec3 playPos = _object->GetGlobalPosition();
            playPos.y = locationOnPlaneHeight + _offsettHeight;
            _object->SetGlobalPosition(playPos);

            return isInContact;
        }
    }

    return isInContact;
}


bool SMath::IsWithinBarycentricCoordinates(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float& _height)
{
    // --------------- Stage 1, Check if object is within extent --------
    if (IsWithinTerrainXZExtent(_object, _surface))
    {
        std::shared_ptr<Mesh> groundPlane = _surface->GetActorVisualMesh();

        std::vector<Vertex>& planeVertices = groundPlane->GetVertices();
        std::vector<Index>& planeIndices = groundPlane->GetIndices();

        glm::vec3 actorPos(_object->GetGlobalPosition());

        // --------------- Stage 2, Get surface global transform --------

        glm::vec3 globalPosition = _surface->GetGlobalPosition();
        glm::vec3 globalScale = _surface->GetGlobalScale();
        glm::quat globalRotation = _surface->GetGlobalRotation();

        glm::mat4 transformMatrix = glm::translate(
            glm::mat4(1.0f), globalPosition) *
            glm::mat4_cast(globalRotation) *
            glm::scale(glm::mat4(1.0f), globalScale);

        // --------------- Stage 3, Iterate through surface in pairs of 3 --------
        for (int i = 0; i < planeIndices.size() - 2; i += 3)
        {
            unsigned int index1 = planeIndices[i];
            unsigned int index2 = planeIndices[i + 1];
            unsigned int index3 = planeIndices[i + 2];

            glm::vec3 point1Pos(planeVertices[index1].mPosition);
            glm::vec3 point2Pos(planeVertices[index2].mPosition);
            glm::vec3 point3Pos(planeVertices[index3].mPosition);

            // Apply the transform
            point1Pos = glm::vec3(transformMatrix * glm::vec4(point1Pos, 1.0f));
            point2Pos = glm::vec3(transformMatrix * glm::vec4(point2Pos, 1.0f));
            point3Pos = glm::vec3(transformMatrix * glm::vec4(point3Pos, 1.0f));

            glm::vec3 baryCoords = GetBarycentricCoordinates(point1Pos, point2Pos, point3Pos, actorPos);

            // If object is on edge move object slightly and re-calculate
            if (baryCoords.x == 0 || baryCoords.y == 0 || baryCoords.z == 0)
            {
                _object->SetGlobalPosition(_object->GetGlobalPosition() + glm::vec3(0.01f, 0.f, 0.01f));
                actorPos = _object->GetGlobalPosition();
                baryCoords = GetBarycentricCoordinates(point1Pos, point2Pos, point3Pos, actorPos);
            }

            // --------------- Stage 4, if object in triangle, update height --------
            if (baryCoords.x > 0 && baryCoords.x < 1 &&
                baryCoords.y > 0 && baryCoords.y < 1 &&
                baryCoords.z > 0 && baryCoords.z < 1)
            {
                // Log triangle index
                //std::cout << "Actor within triangle = " << index1 << " " << index2 << " " << index3 << std::endl;

                // Calculates and updates height from the barycentric coordinates
                _height = GetHeightFromBarycentricCoordinates(baryCoords, point1Pos, point2Pos, point3Pos);

                return true;
            }
        }
    }

    return false;
}

bool SMath::IsWithinBarycentricCoordinates(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float& _height, std::vector<glm::vec3>& _debugSurfacePoints)
{
    // --------------- Stage 1, Check if object is within extent --------
	if (IsWithinTerrainXZExtent(_object, _surface))
	{
        std::shared_ptr<Mesh> groundPlane = _surface->GetActorVisualMesh();

        std::vector<Vertex>& planeVertices = groundPlane->GetVertices();
        std::vector<Index>& planeIndices = groundPlane->GetIndices();

        glm::vec3 actorPos(_object->GetGlobalPosition());

        // --------------- Stage 2, Get surface global transform --------

        glm::vec3 globalPosition = _surface->GetGlobalPosition();
        glm::vec3 globalScale = _surface->GetGlobalScale();
        glm::quat globalRotation = _surface->GetGlobalRotation();

        glm::mat4 transformMatrix = glm::translate(
                                                   glm::mat4(1.0f), globalPosition) * 
                                                    glm::mat4_cast(globalRotation) * 
                                                    glm::scale(glm::mat4(1.0f), globalScale);

        // --------------- Stage 3, Iterate through surface in pairs of 3 --------
        for (int i = 0; i < planeIndices.size() - 2; i += 3)
        {
            unsigned int index1 = planeIndices[i];
            unsigned int index2 = planeIndices[i + 1];
            unsigned int index3 = planeIndices[i + 2];

            glm::vec3 point1Pos(planeVertices[index1].mPosition);
            glm::vec3 point2Pos(planeVertices[index2].mPosition);
            glm::vec3 point3Pos(planeVertices[index3].mPosition);

            // Apply the transform
            point1Pos = glm::vec3(transformMatrix * glm::vec4(point1Pos, 1.0f));
            point2Pos = glm::vec3(transformMatrix * glm::vec4(point2Pos, 1.0f));
            point3Pos = glm::vec3(transformMatrix * glm::vec4(point3Pos, 1.0f));

            // Push locations for debug drawing
            _debugSurfacePoints.push_back(point1Pos);
            _debugSurfacePoints.push_back(point2Pos);
            _debugSurfacePoints.push_back(point1Pos);
            _debugSurfacePoints.push_back(point3Pos);
            _debugSurfacePoints.push_back(point2Pos);
            _debugSurfacePoints.push_back(point3Pos);

            glm::vec3 baryCoords = GetBarycentricCoordinates(point1Pos, point2Pos, point3Pos, actorPos);

            // If object is on edge move object slightly and re-calculate
            if (baryCoords.x == 0 || baryCoords.y == 0 || baryCoords.z == 0)
            {
                _object->SetGlobalPosition(_object->GetGlobalPosition() + glm::vec3(0.01f, 0.f, 0.01f));
                actorPos = _object->GetGlobalPosition();
            	baryCoords = GetBarycentricCoordinates(point1Pos, point2Pos, point3Pos, actorPos);
            }

            // --------------- Stage 4, if object in triangle, update height --------
            if (baryCoords.x > 0 && baryCoords.x < 1 &&
                baryCoords.y > 0 && baryCoords.y < 1 &&
                baryCoords.z > 0 && baryCoords.z < 1)
            {
                // Log triangle index
                //std::cout << "Actor within triangle = " << index1 << " " << index2 << " " << index3 << std::endl;

                // Calculates and updates height from the barycentric coordinates
                _height = GetHeightFromBarycentricCoordinates(baryCoords, point1Pos, point2Pos, point3Pos);

                return true;
            }
        }
	}

    return false;
}


std::vector<glm::vec3> SMath::NevillParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, float _step)
{
    std::vector<glm::vec3> temp;
    for (float t = 0; t < 2; t += _step)
    {
        glm::vec3 point = ((((1 / 2) * (t * t)) - ((3 / 2) * t) + 1) * _point1) + (((2 * t) - (t * t)) * _point2) + ((((1 / 2) * (t * t)) - ((1 / 2) * t)) * _point3);
        temp.push_back(point);
    }
    return temp;
}

std::vector<glm::vec3> SMath::DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, float _step)
{
    std::vector<glm::vec3> temp;
    for (float t = 0; t <= 1; t += _step)
    {
        glm::vec3 c01 = (1 - t) * _point1 + t * (_point2);
        temp.push_back(c01);
    }
    return temp;
}

std::vector<glm::vec3> SMath::DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, float _step)
{
    std::vector<glm::vec3> temp;
    for (float t = 0; t <= 1; t += _step)
    {
        glm::vec3 c01 = (1 - t) * _point1 + t * (_point2);
        glm::vec3 c11 = (1 - t) * _point2 + t * (_point3);
        glm::vec3 c02 = (1 - t) * c01 + t * (c11);
        temp.push_back(c02);
    }
    return temp;
}

std::vector<glm::vec3> SMath::DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, glm::vec3 _point4, float _step)
{
    std::vector<glm::vec3> temp;
    for (float t = 0; t < 1; t += _step)
    {
        glm::vec3 c01 = (1 - t) * _point1 + t * (_point2);
        glm::vec3 c11 = (1 - t) * _point2 + t * (_point3);
        glm::vec3 c21 = (1 - t) * _point3 + t * (_point4);
        glm::vec3 c02 = (1 - t) * c01 + t * (c11);
        glm::vec3 c12 = (1 - t) * c11 + t * (c21);
        glm::vec3 c03 = (1 - t) * c02 + t * (c12);
        temp.push_back(c03);
    }
    return temp;
}

// ------------------------------------- Private -------------------------------------------------

glm::vec3 SMath::GetBarycentricCoordinates(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _objectPos)
{
    // Sets all height to 0 so they do not interfere with calculations
    _p1.y = 0;
    _p2.y = 0;
    _p3.y = 0;
    _objectPos.y = 0;

    // Gets the vectors from point 1 to point 2 and 3
    glm::vec3 p12 = _p2 - _p1;
    glm::vec3 p13 = _p3 - _p1;

    // Finds the normal of these two vectors with cross product
    glm::vec3 n = glm::cross(p12, p13); 
    float area_123 = n.y;   // Size of cross product = the triangles total area

    // Make sure there is no divide by 0 error
    if (area_123 == 0)
        throw std::runtime_error("ERROR::DivideBy0");

    // Sub Triangle Vectors
    glm::vec3 vecP1 = _p1 - _objectPos;
    glm::vec3 vecP2 = _p2 - _objectPos;
    glm::vec3 vecP3 = _p3 - _objectPos;
    glm::vec3 barCoords;

	// Sub triangle 1 ---
    n = glm::cross(vecP2, vecP3);
    barCoords.x = n.y / area_123;

    // Sub triangle 2 ---
    n = glm::cross(vecP3, vecP1);
    barCoords.y = n.y / area_123;

    // Sub triangle 3 ---
    n = glm::cross(vecP1, vecP2);
    barCoords.z = n.y / area_123;

    return barCoords;
}

float SMath::GetHeightFromBarycentricCoordinates(const glm::vec3& _barCoords, const glm::vec3& _p1, const glm::vec3& _p2, const glm::vec3& _p3)
{
    // Calculates height by barycentric coordinates and triangle points
    return ( _barCoords.x * _p1.y + _barCoords.y * _p2.y + _barCoords.z * _p3.y);
}

bool SMath::IsWithinTerrainXZExtent(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface)
{
    if (!_object || !_surface) { LOG_ERROR("No object or surface refrence for extentbounds"); return false; }

    return (_object->GetGlobalPosition().x >= -_surface->mExtent.x && _object->GetGlobalPosition().x <= _surface->mExtent.x) &&
        (_object->GetGlobalPosition().z >= -_surface->mExtent.z && _object->GetGlobalPosition().z <= _surface->mExtent.z);
}
