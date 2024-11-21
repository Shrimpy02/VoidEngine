
// Includes
#include <Core/SMath.h>
#include <LevelActors/BaseActor.h>
#include <LevelActors/VisualActor.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/MeshTypes/DefaultMesh.h>
#include <RenderElements/MeshTypes/PointCloudMesh.h>
#include <RenderElements/VertexTypes/PointCloudVertex.h>
#include <RenderElements/VertexTypes/DefaultVertex.h>
#include <Utilities/Logger.h>
#include <RenderElements/GraphPoint.h>

// Additional includes
#include <stdexcept>
#include <laszip_api.h>
#include <iostream>
#include <fstream>
#include <string>

void SMath::SeedRandTime()
{
    srand((unsigned)time(NULL));
}

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

bool SMath::ConformPhysicsObjectToGeometry(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, glm::vec3& _normals, float& _faceFriction, float _offsettHeight)
{
    bool isInContact = false;
    float locationOnPlaneHeight(0);

    if (!_surface) return isInContact;

    if (std::shared_ptr<BaseActor> baseActor = std::dynamic_pointer_cast<BaseActor>(_object))
    {
        if (IsWithinBarycentricCoordinates(baseActor, _surface, locationOnPlaneHeight, _normals, _faceFriction))
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
    }
    else if (IsWithinBarycentricCoordinates(_object, _surface, locationOnPlaneHeight, _normals, _faceFriction)) {

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

bool SMath::ConformPointToGeometry(glm::vec3& _point, std::shared_ptr<VisualActor> _surface, float _offsettHeight)
{
    bool isInContact = false;

    if (!_surface) return isInContact;

    float locationOnPlaneHeight(0);
    if (IsWithinBarycentricCoordinates(_point, _surface, locationOnPlaneHeight))
    {
        _point.y = locationOnPlaneHeight + _offsettHeight;

        isInContact = true;
    }

    return isInContact;
}

bool SMath::IsWithinBarycentricCoordinates(glm::vec3 _point, std::shared_ptr<VisualActor> _surface, float& _height)
{
    // --------------- Stage 1, Check if object is within extent --------
    if (IsWithinTerrainXZExtent(_point, _surface))
    {
        std::shared_ptr<DefaultMesh> groundPlane = std::dynamic_pointer_cast<DefaultMesh>(_surface->GetActorVisualMesh());

        std::vector<Vertex>& planeVertices = groundPlane->mVertices;
        std::vector<Index>& planeIndices = groundPlane->GetIndices();

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

            glm::vec3 baryCoords = GetBarycentricCoordinates(point1Pos, point2Pos, point3Pos, _point);

            // If object is on edge move object slightly and re-calculate
            if (baryCoords.x == 0 || baryCoords.y == 0 || baryCoords.z == 0)
            {
                _point = _point + glm::vec3(0.01f, 0.f, 0.01f);
                baryCoords = GetBarycentricCoordinates(point1Pos, point2Pos, point3Pos, _point);
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

bool SMath::IsWithinBarycentricCoordinates(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float& _height)
{
    // --------------- Stage 1, Check if object is within extent --------
    if (IsWithinTerrainXZExtent(_object, _surface))
    {
        std::shared_ptr<DefaultMesh> groundPlane = std::dynamic_pointer_cast<DefaultMesh>(_surface->GetActorVisualMesh());

    	std::vector<Vertex>& planeVertices = groundPlane->mVertices;
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
                // Calculates and updates height from the barycentric coordinates
                _height = GetHeightFromBarycentricCoordinates(baryCoords, point1Pos, point2Pos, point3Pos);
                return true;
            }
        }
    }

    return false;
}

bool SMath::IsWithinBarycentricCoordinates(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface, float& _height, glm::vec3& _normal, float& _faceFriction)
{
    // --------------- Stage 1, Check if object is within extent --------
    if (IsWithinTerrainXZExtent(_object, _surface))
    {
        std::shared_ptr<DefaultMesh> groundPlane = std::dynamic_pointer_cast<DefaultMesh>(_surface->GetActorVisualMesh());

        std::vector<Vertex>& planeVertices = groundPlane->mVertices;
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
                _normal = GetTriangleNormalizedNormal(point1Pos, point2Pos, point3Pos);
                _faceFriction = GetTriangleFriction(planeVertices[index1].mFrictionCoefficient, planeVertices[index2].mFrictionCoefficient, planeVertices[index3].mFrictionCoefficient);
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
        std::shared_ptr<DefaultMesh> groundPlane = std::dynamic_pointer_cast<DefaultMesh>(_surface->GetActorVisualMesh());

        std::vector<Vertex>& planeVertices = groundPlane->mVertices;
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

void SMath::LASFileReadingExample(const char* _fileNameIn, const char* _fileNameOut)
{

    //Trying to read point from .laz file 
    const laszip_CHAR* fileName_inn = _fileNameIn;
    const laszip_CHAR* fileName_out = _fileNameOut;

    // create the reader
    laszip_POINTER laszip_reader;
    if (laszip_create(&laszip_reader))
        LOG_ERROR("DLL ERROR: creating laszip reader");

    // open the reader
    laszip_BOOL is_compressed = true; // true for .laz files false for .las files
    if (laszip_open_reader(laszip_reader, fileName_inn, &is_compressed))
        LOG_ERROR("DLL ERROR: opening laszip reader for '%s'", fileName_inn);

    // get a pointer to the header of the reader that was just populated
    laszip_header* header;
    if (laszip_get_header_pointer(laszip_reader, &header))
        LOG_ERROR("DLL ERROR: getting header pointer from laszip reader");

    // how many points does the file have
    laszip_I64 npoints = (header->number_of_point_records ? header->number_of_point_records : header->extended_number_of_point_records);

    // report how many points the file has
    LOG("file '%s' contains %I64d points", npoints);

    // get a pointer to the points that will be read
    laszip_point* point;
    if (laszip_get_point_pointer(laszip_reader, &point))
        LOG_ERROR("DLL ERROR: getting point pointer from laszip reader");

    // create the writer
    laszip_POINTER laszip_writer;
    if (laszip_create(&laszip_writer))
        LOG_ERROR("DLL ERROR: creating laszip writer");

    // initialize the header for the writer using the header of the reader
    if (laszip_set_header(laszip_writer, header))
        LOG_ERROR("DLL ERROR: setting header for laszip writer");

    // open the writer

    laszip_BOOL compress = (strstr(fileName_out, ".laz") != 0);
    if (laszip_open_writer(laszip_writer, fileName_out, compress))
        LOG_ERROR("DLL ERROR: opening laszip writer for '%s'", fileName_out);

    // read the points
    laszip_I64 numPoints = 0;
    while (numPoints < npoints)
    {
        // read a point

        if (laszip_read_point(laszip_reader))
            LOG_ERROR("DLL ERROR: reading point %I64d\n", numPoints);


        // copy the point
        if (laszip_set_point(laszip_writer, point))
            LOG_ERROR("DLL ERROR: setting point %I64d\n", numPoints);

        // write the point
        if (laszip_write_point(laszip_writer))
            LOG_WARNING("DLL ERROR: writing point %I64d\n", numPoints);

        numPoints++;
    }
}

std::vector<glm::vec3> SMath::LASFileToPoints(const char* _fileDirectory)
{
    // create the reader
    laszip_POINTER laszip_reader;
    if (laszip_create(&laszip_reader))
        LOG_ERROR("DLL ERROR: creating laszip reader");

    // open the reader
    laszip_BOOL is_compressed = true;
    if (laszip_open_reader(laszip_reader, _fileDirectory, &is_compressed))
        LOG_ERROR("DLL ERROR: opening laszip reader for '%s'", _fileDirectory);

    // get a pointer to the header of the reader that was just populated
    laszip_header* header;
    if (laszip_get_header_pointer(laszip_reader, &header))
        LOG_ERROR("DLL ERROR: getting header pointer from laszip reader");
    
    // get a pointer to the points that will be read
    laszip_point* point;
    if (laszip_get_point_pointer(laszip_reader, &point))
        LOG_ERROR("DLL ERROR: getting point pointer from laszip reader");

    // how many points does the file have
    laszip_I64 numTotalPoints = (header->number_of_point_records ? header->number_of_point_records : header->extended_number_of_point_records);

    // read the points
    std::vector<glm::vec3> rPoints;
    for (int i = 0; i < numTotalPoints; i++)
    {
        // Read the next point
        if (laszip_read_point(laszip_reader))
            LOG_ERROR("DLL ERROR: reading point %I64d\n", i);

        // Apply the scaling factors from the LAS header to get actual coordinates
        float x = (float)(point->X * header->x_scale_factor + header->x_offset);
        float y = (float)(point->Y * header->y_scale_factor + header->y_offset);
        float z = (float)(point->Z * header->z_scale_factor + header->z_offset);

        glm::vec3 glmPoint = glm::vec3(x, y, z);
        rPoints.push_back(glmPoint);
    }

    // Clean up the LASzip reader
    if (laszip_close_reader(laszip_reader))
        LOG_ERROR("DLL ERROR: closing laszip reader");
    if (laszip_destroy(laszip_reader))
        LOG_ERROR("DLL ERROR: destroying laszip reader");

    return rPoints;
}

void SMath::LASFileToCustomFileOfPoints(const char* _fileDirectoryInn, const char* _fileDirectoryOut)
{
    std::vector<glm::vec3> filePointCloudPoints = std::move(LASFileToPoints(_fileDirectoryInn));

    std::ofstream fileOut(_fileDirectoryOut);

    if(fileOut.is_open())
    {
        // Reset file
        fileOut.clear();

        // First line is number of points
        fileOut << filePointCloudPoints.size() << "\n";

        // The rest is the points
        for (int i = 0; i < filePointCloudPoints.size(); i++)
        {
            fileOut << filePointCloudPoints[i].x << ", " << filePointCloudPoints[i].z << ", " << filePointCloudPoints[i].y << "\n";
        }

        fileOut.close();
    } else {
        LOG_ERROR("Error opening file for custom LAS conversion");
    }
}

void SMath::CenterPointCloudStructureToWorldZero(std::shared_ptr<Actor> _terrainParent)
{
    glm::vec3 centroid{ glm::vec3(0) };
    int div = 0;

    // Calculate the centroid
    for(std::shared_ptr<Actor> actor : _terrainParent->GetChildren())
    {
        if(std::shared_ptr<VisualActor> terrainSector = std::dynamic_pointer_cast<VisualActor>(actor))
        {
            if(std::shared_ptr<PointCloudMesh> pntcloudMesh = std::dynamic_pointer_cast<PointCloudMesh>(terrainSector->GetMesh()))
            {
                div += pntcloudMesh->GetIndices().size();
                for (PointCloudVertex& vertex : pntcloudMesh->mVertices)
                {
                    centroid += vertex.mPosition;
                }
            }
        }
    }

    if (div == 0) {
        LOG_ERROR("No vertices found in point cloud, div = 0");
        return;
    }

    // divide centroid by total points
    centroid /= div;

    // Apply the centroid
    for (std::shared_ptr<Actor> actor : _terrainParent->GetChildren())
    {
        if (std::shared_ptr<VisualActor> terrainSector = std::dynamic_pointer_cast<VisualActor>(actor))
        {
            if (std::shared_ptr<PointCloudMesh> pntcloudMesh = std::dynamic_pointer_cast<PointCloudMesh>(terrainSector->GetMesh()))
            {
                for (PointCloudVertex& vertex : pntcloudMesh->mVertices)
                {
                    vertex.mPosition -= centroid;
                }
            }
        }
    }
}

void SMath::MovePointCloudPoints(std::shared_ptr<VisualActor> _terrain, glm::vec3 _moveBy)
{
    if (std::shared_ptr<PointCloudMesh> pntcloudMesh = std::dynamic_pointer_cast<PointCloudMesh>(_terrain->GetMesh()))
    {
        for (PointCloudVertex& vertex : pntcloudMesh->mVertices)
        {
            vertex.mPosition += _moveBy;
        }
    }
}

void SMath::MergeVerticesByXZ(std::vector<glm::vec3>& vertices, const int _quality)
{
    // Define a threshold for "neighboring" to avoid floating-point inaccuracies
    const float EPSILON = 0.001f;
    for(int i = 0; i < _quality; i++)
    {
        for (size_t j = 0; j < vertices.size(); ++j) {
            glm::vec3& v1 = vertices[j];

            for (size_t k = j + 1; k < vertices.size(); ++k) {
                glm::vec3& v2 = vertices[k];

                // Check if x and z values are the same within the threshold, but y values differ
                if (std::fabs(v1.x - v2.x) < EPSILON &&
                    std::fabs(v1.z - v2.z) < EPSILON &&
                    std::fabs(v1.y - v2.y) > EPSILON) {

                    // Calculate the y midpoint and update both vertices to the same y value
                    float midpointY = (v1.y + v2.y) / 2.0f;
                    v1.y = midpointY;
                    v2.y = midpointY;
                }
            }
        }
    }

}

void SMath::MergeVerticesXZ(std::vector<glm::vec3>& vertices)
{
    // Define a threshold for "neighboring" to avoid floating-point inaccuracies
    const float EPSILON = 0.001f;

    for (size_t i = 0; i < vertices.size(); ++i) {
        glm::vec3& v1 = vertices[i];

        for (size_t j = i + 1; j < vertices.size(); ++j) {
            glm::vec3& v2 = vertices[j];

            // Check if x and z values are the same within the threshold, but y values differ
            if (std::fabs(v1.x - v2.x) < EPSILON &&
                std::fabs(v1.z - v2.z) < EPSILON) {

                v2.x = v1.x;
                v2.y = v1.y;
                v2.z = v1.z;
            }
        }
    }
}

bool SMath::PointXZOverlapsWithVertex(const glm::vec3& _pos1, const glm::vec3& _pos2, const float _epsilon)
{
    return ((fabs(_pos1.x - _pos2.x) < _epsilon) && (fabs(_pos1.z - _pos2.z) < _epsilon));
}

void SMath::UpdateVerticesNormal(std::vector<Vertex>& _vertices, std::vector<Index>& _indices)
{
    for (size_t i = 0; i < _indices.size(); i += 3) {
        int idx0 = _indices[i];
        int idx1 = _indices[i + 1];
        int idx2 = _indices[i + 2];

        const glm::vec3& v0 = _vertices[idx0].mPosition;
        const glm::vec3& v1 = _vertices[idx1].mPosition;
        const glm::vec3& v2 = _vertices[idx2].mPosition;

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        glm::vec3 triangleNormal = glm::normalize(glm::cross(edge1, edge2));

        _vertices[idx0].mNormal += triangleNormal;
        _vertices[idx0].mNormal += triangleNormal;
        _vertices[idx0].mNormal += triangleNormal;
    }
}

bool SMath::PointWithinArea(glm::vec3 _point, glm::vec3 _minExtent, glm::vec3 _maxExtent)
{
    glm::vec3 localExtent = (_maxExtent - _minExtent) * glm::vec3(0.5);
    glm::vec3 localCentre = (_minExtent + _maxExtent) * glm::vec3(0.5);
    glm::vec3 localdiff = _point - localCentre;

    // Check each axis for non intersection, uses early exit
    for (int i = 0; i < 3; i++)
    {
        if (abs(localdiff[i]) > localExtent[i])
        {
            // Vertex not within section
            return false;

        }
        else
            return true;
    }
    return false;
}

bool SMath::PointWithinArea(glm::vec3 _point, glm::vec3 _minExtent, glm::vec3 _maxExtent, float _yOveride)
{

    glm::vec3 localExtent = (_maxExtent - _minExtent) * glm::vec3(0.5);
    localExtent.y = _yOveride;
    glm::vec3 localCentre = (_minExtent + _maxExtent) * glm::vec3(0.5);
    glm::vec3 localdiff = _point - localCentre;

    // Check each axis for non intersection, uses early exit
    for (int i = 0; i < 3; i++)
    {
        if (abs(localdiff[i]) > localExtent[i])
        {
            // Vertex not within section
            return false;

        } else 
        	return true;
    }
    return false;
}

void SMath::UpdateIndex(std::vector<glm::vec3>& vertexPointsForNewMesh, std::vector<Index>& indices,
    const glm::vec3& vert1, const glm::vec3& vert2, const glm::vec3& vert3, const glm::vec3& vert4) {
    // Use helper function to find or add vertices and return the correct index
    int idx1 = FindOrAddVertex(vertexPointsForNewMesh, vert1);
    int idx2 = FindOrAddVertex(vertexPointsForNewMesh, vert2);
    int idx3 = FindOrAddVertex(vertexPointsForNewMesh, vert3);
    int idx4 = FindOrAddVertex(vertexPointsForNewMesh, vert4);

    // Add indices to create two triangles for the quad
    indices.push_back(idx1);
    indices.push_back(idx2);
    indices.push_back(idx3);

    indices.push_back(idx3);
    indices.push_back(idx2);
    indices.push_back(idx4);
}

std::vector<glm::vec3> SMath::NevillInterpolatedPoints(const std::vector<std::shared_ptr<GraphPoint>>& _controlPoints, const float _step)
{
    std::vector<glm::vec3> temp;
    switch (_controlPoints.size()) {

    case 3:
        temp = NevillInterpolatedParametricCurveFromPoints(_controlPoints[0]->GetLocalPosition(), _controlPoints[1]->GetLocalPosition(), _controlPoints[2]->GetLocalPosition(), _step);
        break;
    default:
        LOG_ERROR("Inncorect number of control points");
        break;
    }

    return temp;
}

std::vector<glm::vec3> SMath::DeCastApproximationPoints(const std::vector<std::shared_ptr<GraphPoint>>& _controlPoints, const float _step)
{
    std::vector<glm::vec3> temp;
    switch (_controlPoints.size()) {
    case 2:
        temp = DeCastParametricCurveFromPoints(_controlPoints[0]->GetLocalPosition(), _controlPoints[1]->GetLocalPosition(), _step);
        break;
    case 3:
        temp = DeCastParametricCurveFromPoints(_controlPoints[0]->GetLocalPosition(), _controlPoints[1]->GetLocalPosition(), _controlPoints[2]->GetLocalPosition(), _step);
        break;
    case 4:
        temp = DeCastParametricCurveFromPoints(_controlPoints[0]->GetLocalPosition(), _controlPoints[1]->GetLocalPosition(), _controlPoints[2]->GetLocalPosition(), _controlPoints[3]->GetLocalPosition(), _step);
        break;
    default:
        LOG_ERROR("Inncorect number of control points");
        break;
    }

    return temp;
}

int SMath::GetRandomInt(const int& _offset, const int& _range)
{
    return _offset + rand() % _range;
}

float SMath::GetRandomFloat(const float& _offset, const float& _range)
{
    return  _offset + static_cast <float>(rand()) / (static_cast <float> (RAND_MAX / _range));
}

float SMath::GetRandomFloatBetweenMinMax(const float& _min, const float& _max)
{
    float randomFraction = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    return _min + randomFraction * (_max - _min);
}


// ------------------------------------- Private -------------------------------------------------

std::vector<glm::vec3> SMath::DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, float _step)
{
    std::vector<glm::vec3> temp;
    for (float t = 0; t <= 1; t += _step)
    {
        glm::vec3 c01 = (1.f - t) * _point1 + t * (_point2);
        temp.push_back(c01);
    }
    return temp;
}

std::vector<glm::vec3> SMath::DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, float _step)
{
    std::vector<glm::vec3> temp;
    for (float t = 0; t <= 1; t += _step)
    {
        glm::vec3 c01 = (1.f - t) * _point1 + t * (_point2);
        glm::vec3 c11 = (1.f - t) * _point2 + t * (_point3);
        glm::vec3 c02 = (1.f - t) * c01 + t * (c11);
        temp.push_back(c02);
    }
    return temp;
}

std::vector<glm::vec3> SMath::DeCastParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, glm::vec3 _point4, float _step)
{
    std::vector<glm::vec3> temp;
    for (float t = 0; t < 1; t += _step)
    {
        glm::vec3 c01 = (1.f - t) * _point1 + t * (_point2);
        glm::vec3 c11 = (1.f - t) * _point2 + t * (_point3);
        glm::vec3 c21 = (1.f - t) * _point3 + t * (_point4);
        glm::vec3 c02 = (1.f - t) * c01 + t * (c11);
        glm::vec3 c12 = (1.f - t) * c11 + t * (c21);
        glm::vec3 c03 = (1.f - t) * c02 + t * (c12);
        temp.push_back(c03);
    }
    return temp;
}

std::vector<glm::vec3> SMath::NevillInterpolatedParametricCurveFromPoints(glm::vec3 _point1, glm::vec3 _point2, glm::vec3 _point3, float _step)
{
    std::vector<glm::vec3> temp;
    for (float t = 0; t < 2; t += _step)
    {
        glm::vec3 p1 = (((0.5f * (t * t)) - (1.5f * t) + 1.f) * _point1);
        glm::vec p2 = (((2.f * t) - (t * t)) * _point2);
        glm::vec p3 = (((0.5f * (t * t)) - (0.5f * t)) * _point3);
        glm::vec3 point = p1 + p2 + p3;
        temp.push_back(point);
    }
    return temp;
}

float SMath::CoxDeBoorRecursive(int _i, int _d, float _uv, const std::vector<float>& _knotVector)
{
    if (_d == 0) {
        // If dimension is 0 compare knot vectors and return value
        return (_knotVector[_i] <= _uv && _uv < _knotVector[_i + 1]) ? 1.0f : 0.0f;
    }
    else {
        // Otherwise sum the left and right basis formula for the total influence of control point at p.
        float left = 0.0f;
        float right = 0.0f;

        // Left basis function term: Check for division by zero
        float denominatorLeft = _knotVector[_i + _d] - _knotVector[_i];
        if (denominatorLeft != 0.0f) {
            left = (_uv - _knotVector[_i]) / denominatorLeft * CoxDeBoorRecursive(_i, _d - 1, _uv, _knotVector);
        }

        // Right basis function term: Check for division by zero
        float denominatorRight = _knotVector[_i + _d + 1] - _knotVector[_i + 1];
        if (denominatorRight != 0.0f) {
            right = (_knotVector[_i + _d + 1] - _uv) / denominatorRight * CoxDeBoorRecursive(_i + 1, _d - 1, _uv, _knotVector);
        }

        return left + right;

    }
}


glm::vec3 SMath::EvaluateBSplineSurface(float _u, float _v, int _du, int _dv, const std::vector<float>& _uKnot, const std::vector<float>& _vKnot, const std::vector<std::vector<glm::vec3>>& _controlPoints)
{
    glm::vec3 surfacePoint(0.0f);
    const int numControlPointsU = _controlPoints.size() - 1;
    const int numControlPointsV = _controlPoints[0].size() - 1;

    // Loop through all control points
    for (int i = 0; i <= numControlPointsU; ++i) {
        for (int j = 0; j <= numControlPointsV; ++j) {
            
            // Evaluates influence of control point i in u direction (basis function)
            float Coxi = CoxDeBoorRecursive(i, _du, _u, _uKnot);
            // Evaluates influence of control point j in v direction (basis function)
            float Coxj = CoxDeBoorRecursive(j, _dv, _v, _vKnot);
            // Sum weight * control point with the surfacePoint
        	glm::vec3 controlPointWight = Coxi * Coxj * _controlPoints[i][j];
            surfacePoint += controlPointWight;
        }
    }

    // Return the point on the surface at (u, v)
    return surfacePoint;
}

glm::vec3 SMath::EvaluateBSplineNormal(float _u, float _v, int _du, int _dv, int _UResolution, int _VResolution, const std::vector<float>& _uKnot,
	const std::vector<float>& _vKnot, const std::vector<std::vector<glm::vec3>>& _controlPoints)
{

    glm::vec3 P = SMath::EvaluateBSplineSurface(_u, _v, _du, _dv, _uKnot, _vKnot, _controlPoints);
    glm::vec3 P_u = SMath::EvaluateBSplineSurface(_u + 1.0 / _UResolution, _v, _du, _dv, _uKnot, _vKnot, _controlPoints);
    glm::vec3 P_v = SMath::EvaluateBSplineSurface(_u, _v + 1.0 / _VResolution, _du, _dv, _uKnot, _vKnot, _controlPoints);

    // Tangents
    glm::vec3 T_u = P_u - P;
    glm::vec3 T_v = P_v - P;

    // Normal (cross product of tangents)
    glm::vec3 N = glm::normalize(cross(T_u, T_v));

    return N;
}

std::vector<glm::vec3> SMath::BSplineFromPoints(const std::vector<std::shared_ptr<GraphPoint>>& _controlPoints,
	const float& _step, const int& _dimension)
{
   
    std::vector<float> knots = GenerateClampedKnotVector(_controlPoints.size(), _dimension);
    std::vector<glm::vec3> curvePoints;
    int resolution = (1 / (_step * 4)) * _controlPoints.size();

    // Ensure correct parameter range
    float tMin = knots[_dimension];
    float tMax = knots[knots.size() - _dimension - 1];

    for (int i = 0; i < resolution; ++i) {
        float t = tMin + (tMax - tMin) * i / resolution;

        glm::vec3 point(0.0f);
        for (int j = 0; j < _controlPoints.size(); ++j) {
            // Evaluate basis function
            float basis = CoxDeBoorRecursive(j, _dimension, t, knots);
            point += basis * _controlPoints[j]->GetGlobalPosition();
        }

        curvePoints.push_back(point);
    }

    return curvePoints;




    //std::vector<glm::vec3> curvePoints;
    //int resolution = 5;
    //std::vector<float> knots = GenerateClampedKnotVector(_controlPoints.size(), _dimension);
    //float tMin = knots[_dimension];
    //float tMax = knots[knots.size() - _dimension - 1];

    //for (int step = 0; step <= resolution; ++step) {
    //    float t = tMin + (tMax - tMin) * step / resolution;
    //    glm::vec3 point(0.0f);

    //    for (int i = 0; i < _controlPoints.size(); ++i) {
    //        float basis = CoxDeBoorRecursive(i, _dimension + 1, t, knots);
    //        point += basis * _controlPoints[i]->GetGlobalPosition();
    //    }
    //    curvePoints.push_back(point);
    //}
    //return curvePoints;

}

std::vector<float> SMath::GenerateClampedKnotVector(int _numControlPoints, int _degree)
{
    int numKnots = _numControlPoints + _degree + 1;
    std::vector<float> knotVector(numKnots);

    // First `degree + 1` values are 0
    for (int i = 0; i <= _degree; ++i) {
        knotVector[i] = 0.0f;
    }

    // Last `degree + 1` values are 1
    for (int i = numKnots - _degree - 1; i < numKnots; ++i) {
        knotVector[i] = 1.0f;
    }

    // Uniformly distribute the middle values
    int numMiddleKnots = numKnots - 2 * (_degree + 1);
    for (int i = 1; i <= numMiddleKnots; ++i) {
        knotVector[_degree + i] = static_cast<float>(i) / numMiddleKnots;
    }

    return knotVector;
}

std::vector<float> SMath::GenerateUniformKnotVector(int _numControlPoints, int _degree)
{
    int numKnots = _numControlPoints + _degree + 1;
    std::vector<float> knotVector(numKnots);

    // Uniformly distribute the knots
    float interval = 1.0f / (numKnots - 1);
    for (int i = 0; i < numKnots; ++i) {
        knotVector[i] = i * interval;
    }

    return knotVector;
}

void SMath::AdjustVertexCoordinates(std::shared_ptr<PointCloudMesh> _pcm, glm::vec3 _offsett)
{
    for (PointCloudVertex& vertex : _pcm->mVertices)
    {
        vertex.mPosition += _offsett;
        //vertex.mPosition = abs(vertex.mPosition);
    }
}

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

glm::vec3 SMath::GetTriangleNormalizedNormal(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3)
{
    // Compute two edges of the triangle
    glm::vec3 edge1 = _p2 - _p1;
    glm::vec3 edge2 = _p3 - _p1;

    // Compute the cross product of the two edges to get the normal
    glm::vec3 normal = glm::cross(edge1, edge2);

    // Normalize result
    normal = glm::normalize(normal);

    return normal;
}

float SMath::GetHeightFromBarycentricCoordinates(const glm::vec3& _barCoords, const glm::vec3& _p1, const glm::vec3& _p2, const glm::vec3& _p3)
{
    // Calculates height by barycentric coordinates and triangle points
    return ( _barCoords.x * _p1.y + _barCoords.y * _p2.y + _barCoords.z * _p3.y);
}

float SMath::GetTriangleFriction(float _f1, float _f2, float _f3)
{
    float avgFrictionCoef = (_f1 + _f2 + _f3) / 3;
    return avgFrictionCoef;
}

bool SMath::IsWithinTerrainXZExtent(std::shared_ptr<Actor> _object, std::shared_ptr<VisualActor> _surface)
{
    if (!_object || !_surface) { LOG_ERROR("No object or surface refrence for extentbounds"); return false; }

    return (_object->GetGlobalPosition().x >= -_surface->mExtent.x && _object->GetGlobalPosition().x <= _surface->mExtent.x) &&
        (_object->GetGlobalPosition().z >= -_surface->mExtent.z && _object->GetGlobalPosition().z <= _surface->mExtent.z);
}

bool SMath::IsWithinTerrainXZExtent(const glm::vec3& _point, std::shared_ptr<VisualActor> _surface)
{
    if (!_surface) { LOG_ERROR("No surface refrence for extentbounds"); return false; }

    return (_point.x >= -_surface->mExtent.x && _point.x <= _surface->mExtent.x) &&
        (_point.z >= -_surface->mExtent.z && _point.z <= _surface->mExtent.z);
}

int SMath::FindOrAddVertex(std::vector<glm::vec3>& vertices, const glm::vec3& pos)
{
    float const epsilon = 0.001f;

    for (int i = 0; i < vertices.size(); ++i) {
        if (std::fabs(vertices[i].x - pos.x) < epsilon &&
            std::fabs(vertices[i].z - pos.z) < epsilon) {
            // If the vertex is close enough, return the existing index
            return i;
        }
    }
    // If no matching vertex was found, add the new vertex and return its index
    vertices.push_back(pos);
    return vertices.size() - 1;
}
