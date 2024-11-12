// Includes
#include <RenderElements/Mesh.h>
#include <RenderElements/MeshTypes/DefaultMesh.h>
#include <RenderElements/MeshTypes/DebugMesh.h>
#include <RenderElements/MeshTypes/GraphMesh.h>
#include <RenderElements/MeshTypes/PointCloudMesh.h>
#include <RenderElements/VertexTypes/DefaultVertex.h>
#include <RenderElements/VertexTypes/GraphVertex.h>
#include <RenderElements/VertexTypes/DebugVertex.h>
#include <RenderElements/VertexTypes/PointCloudVertex.h>
#include <RenderElements/Texture.h>
#include <RenderElements/Material.h>

#include <Core/SMath.h>
#include <Utilities/Logger.h>
#include <corecrt_math_defines.h>
#include <laszip_api.h>

// static cache of meshes
std::unordered_map<std::string, std::shared_ptr<Mesh>> Mesh::mCache;

Mesh::Mesh(const std::string _name)
	:mName(_name)
{
}

void Mesh::Draw(const std::shared_ptr<Shader> _shader) const
{

}

std::shared_ptr<Mesh> Mesh::CreateCube(std::shared_ptr<Material> _material, const bool _instance, std::string _customName)
{
	// Default key
    std::string cubeKey = "DefaultCube";

    // Overwrite key if there is custom name
    if (!_customName.empty())
        cubeKey = _customName;
    
    // If instance enabled find mesh in cache and return it
    if(_instance)
    {
        auto it = mCache.find(cubeKey);
        if (it != mCache.end())
            return it->second;
    }

    std::vector<Vertex> vertices = {
        // Back face (looking towards -Z)
        {{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, // Bottom-left
        {{ 0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, // Bottom-right
        {{ 0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, // Top-right
        {{-0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, // Top-left

        // Front face (looking towards +Z)
        {{-0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // Bottom-left
        {{ 0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // Bottom-right
        {{ 0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // Top-right
        {{-0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // Top-left

        // Left face (looking towards -X)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom-right
        {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom-left
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // Top-left
        {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // Top-right

        // Right face (looking towards +X)
        {{ 0.5f, -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom-left
        {{ 0.5f, -0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom-right
        {{ 0.5f,  0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // Top-right
        {{ 0.5f,  0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // Top-left

        // Top face (looking towards +Y)
        {{-0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, // Top-left
        {{ 0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, // Top-right
        {{ 0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom-right
        {{-0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom-left

        // Bottom face (looking towards -Y)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}}, // Top-left
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, // Top-right
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom-right
        {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom-left
    };
    std::vector<Index> indices = {
        // Back face
        0, 2, 1, 0, 3, 2,
        // Front face
        4, 5, 6, 4, 6, 7,
        // Left face
        8, 9, 10, 8, 10, 11,
        // Right face
        12, 14, 13, 12, 15, 14,
        // Top face
        16, 18, 17, 16, 19, 18,
        // Bottom face
        20, 21, 22, 20, 22, 23
    };

	std::shared_ptr<DefaultMesh> cube = std::make_shared<DefaultMesh>(cubeKey, std::move(vertices), std::move(indices), _material);

	// If instance enabled add object to cache
    if(_instance)
		mCache[cubeKey] = cube;

	return cube;
}

std::shared_ptr<Mesh> Mesh::CreateCubeByExtent(std::shared_ptr<Mesh> _extentMesh, std::shared_ptr<Material> _material, std::string _customName)
{
    std::pair minMax = _extentMesh->GetMeshMinMaxExtent();

    // Then create the collision mesh using these extents (Mesh as cube for AABB object)
    // generate a cube using extents
    std::vector<Vertex> vertices = {
        // Front face
        {{minMax.first.x, minMax.first.y,  minMax.second.z}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // Bottom-left
        {{ minMax.second.x, minMax.first.y,  minMax.second.z}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // Bottom-right
        {{ minMax.second.x,  minMax.second.y,  minMax.second.z}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // Top-right
        {{minMax.first.x,  minMax.second.y,  minMax.second.z}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // Top-left
        // Back face
        {{minMax.first.x, minMax.first.y, minMax.first.z}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ minMax.second.x, minMax.first.y, minMax.first.z}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{ minMax.second.x,  minMax.second.y, minMax.first.z}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        {{minMax.first.x,  minMax.second.y, minMax.first.z}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
        // Left face
        {{minMax.first.x, minMax.first.y, minMax.first.z}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{minMax.first.x, minMax.first.y,  minMax.second.z}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{minMax.first.x,  minMax.second.y,  minMax.second.z}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{minMax.first.x,  minMax.second.y, minMax.first.z}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        // Right face
        {{ minMax.second.x, minMax.first.y, minMax.first.z}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ minMax.second.x, minMax.first.y,  minMax.second.z}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ minMax.second.x,  minMax.second.y,  minMax.second.z}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{ minMax.second.x,  minMax.second.y, minMax.first.z}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        // Top face
        {{minMax.first.x,  minMax.second.y, minMax.first.z}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        {{minMax.first.x,  minMax.second.y,  minMax.second.z}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ minMax.second.x,  minMax.second.y,  minMax.second.z}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ minMax.second.x,  minMax.second.y, minMax.first.z}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        // Bottom face
        {{minMax.first.x, minMax.first.y, minMax.first.z}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{minMax.first.x, minMax.first.y,  minMax.second.z}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        {{ minMax.second.x, minMax.first.y,  minMax.second.z}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ minMax.second.x, minMax.first.y, minMax.first.z}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}
    };

    // Generate cube indices
    std::vector<Index> indices = {
        // Front face
        0, 1, 2, 0, 2, 3,
        // Back face
        4, 5, 6, 4, 6, 7,
        // Left face
        8, 9, 10, 8, 10, 11,
        // Right face
        12, 13, 14, 12, 14, 15,
        // Top face
        16, 17, 18, 16, 18, 19,
        // Bottom face
        20, 21, 22, 20, 22, 23
    };

    return std::make_shared<DefaultMesh>(_customName, std::move(vertices), std::move(indices), _material);
}

std::shared_ptr<Mesh> Mesh::CreatePlane(std::shared_ptr<Material> _material, const bool _instance, std::string _customName)
{
    // Default key
    std::string planeKey = "DefaultPlane";

    // Overwrites default key if custom name is added.
    if (!_customName.empty())
        planeKey = _customName;

    // If instance enabled find mesh in cache and return it
    if(_instance)
    {
        auto it = mCache.find(planeKey);
        if (it != mCache.end())
            return it->second;
    }

    // Gen vertices and indices from vector
    std::vector<Vertex> vertices = {
        // Front face
        {{-0.5f, 0.0f,  -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ -0.5f, 0.0f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.0f,  -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        {{0.5f,  0.0f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
    };
    std::vector<Index> indices = {
        // Front face
        0, 1, 2, 1, 3, 2
    };
    
    std::shared_ptr<DefaultMesh> plane = std::make_shared<DefaultMesh>(planeKey, std::move(vertices), std::move(indices), _material);

    // If instance enabled add object to cache
	if(_instance)
		mCache[planeKey] = plane;

    return plane;
}

std::shared_ptr<Mesh> Mesh::CreatePlane(std::vector<Vertex> _planePoints, std::vector<Index> _planeIndicesPoints, std::shared_ptr<Material> _material, const bool _instance, std::string _customName)
{
    // Default key
    std::string planeKey = "DefaultPlane";

    // Overwrites default key if custom name is added.
    if (!_customName.empty())
        planeKey = _customName;

    // If instance enabled find mesh in cache and return it
    if (_instance)
    {
        auto it = mCache.find(planeKey);
        if (it != mCache.end())
            return it->second;
    }

    std::shared_ptr<DefaultMesh> plane = std::make_shared<DefaultMesh>(planeKey, std::move(_planePoints), std::move(_planeIndicesPoints), _material);

    // If instance enabled add object to cache
    if (_instance)
        mCache[planeKey] = plane;

    return plane;
}

std::shared_ptr<Mesh> Mesh::CreatePyramid(std::shared_ptr<Material> _material,const bool _instance, std::string _customName)
{
    // Create key
    std::string pyramidKey = "DefaultPyramid";

    // Overwrites default key if custom name is added.
    if (!_customName.empty()) 
        pyramidKey = _customName;

    // If instance enabled find mesh in cache and return it
    if(_instance)
    {
        auto it = mCache.find(pyramidKey);
        if (it != mCache.end())
            return it->second;
    }

    // Gen vertices and indices from vector
    std::vector<Vertex> vertices = {
        // Base
       {{-0.5f, 0.0f,  -0.5f}, {0.0f,  -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ -0.5f, 0.0f,  0.5f}, {0.0f,  -1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.0f,  -0.5f}, {0.0f,  -1.0f,  0.0f}, {0.0f, 1.0f}},
		 {{0.5f,  0.0f,  0.5f}, {0.0f,  -1.0f,  0.0f}, {1.0f, 1.0f}},

        // Top
         {{0.0f,  0.5f,  0.0f}, {0.0f,  1.0f,  0.0f}, {0.5f, 0.5f}},
    };

    std::vector<Index> indices = {
        // Base
        0, 2, 1,  // Triangle 1: Base
        1, 2, 3,  // Triangle 2: Base

        // Sides
        0, 1, 4,  // Triangle 3: Side 1
        1, 3, 4,  // Triangle 4: Side 2
        3, 2, 4,  // Triangle 5: Side 3
        2, 0, 4   // Triangle 6: Side 4
    };

     std::shared_ptr<DefaultMesh> pyramid = std::make_shared<DefaultMesh>(pyramidKey, std::move(vertices), std::move(indices), _material);

     // If instance enabled add object to cache
     if (_instance)
         mCache[pyramidKey] = pyramid;
    
    return pyramid;
}

std::shared_ptr<Mesh> Mesh::CreateSphereByExtent(std::shared_ptr<Mesh> _extentMesh, std::shared_ptr<Material> _material, std::string _customName)
{
    std::pair minMax = _extentMesh->GetMeshMinMaxExtent();
    
    float radius = 0.0f;

    if(glm::length(minMax.first) > glm::length(minMax.second))
    {
        radius = glm::length(minMax.first);
    } else {
        radius = glm::length(minMax.second);
    }

    radius /= 2;
    radius += 0.08f;
    std::vector<Vertex> vertices;
    std::vector<Index> indices;
    Mesh::GenSphere(vertices, indices, 2, radius);

    return std::make_shared<DefaultMesh>(_customName, std::move(vertices), std::move(indices), _material);
}

std::shared_ptr<Mesh> Mesh::CreateSphere(std::shared_ptr<Material> _material, const int _subdivides, const bool _instance, std::string _customName)
{
    // Create default key based on num subdivides
    std::string sphereKey = "DefaultSphere_" + std::to_string(_subdivides);

    // Overwrites default key if custom name is added.
    if (!_customName.empty()) 
        sphereKey = _customName;

    // If instance enabled find mesh in cache and return it
    if (_instance)
    {
        auto it = mCache.find(sphereKey);
        if (it != mCache.end())
            return it->second;
    }

    // Gen empty vertices and indices from vector
    std::vector<Vertex> vertices;
    std::vector<Index> indices; 

    // Takes vertices and indices vectors as input and populates them with relevant geometry data.
    // Can probably be optimizing quite a bit.
    // TODO: fix textrure warping
	GenSphere(vertices,indices,_subdivides);

    std::shared_ptr<DefaultMesh> sphere = std::make_shared<DefaultMesh>(sphereKey, std::move(vertices), std::move(indices), _material);

    // If instance enabled add object to cache
    if (_instance)
        mCache[sphereKey] = sphere;

	return sphere;
}

std::shared_ptr<Mesh> Mesh::CreateBSplineSurface(std::shared_ptr<Material> _material, int _UResolution, int _VResolution,  int _du,
	int _dv, const std::vector<float>& _uKnot, const std::vector<float>& _vKnot,
	const std::vector<std::vector<glm::vec3>>& _controlPoints, std::string _customName)
{
    // Create key
    std::string surfaceKey = "DefaultSurface";

    // Overwrites default key if custom name is added.
    if (!_customName.empty())
        surfaceKey = _customName;

    // Gen vertices and indices from vector
    std::vector<Vertex> vertices;
    std::vector<Index> indices;

    // Gen spline surface ----
    for (int i = 0; i < _UResolution; ++i) {
        // Compute normalized u parameter in the [0, 1] range
        double u = (double)i / (_UResolution - 1);

        for (int j = 0; j < _VResolution; ++j) {
            // Compute normalized v parameter in the [0, 1] range
            double v = (double)j / (_VResolution - 1);

            // Evaluate the surface at (u, v)
            glm::vec3 surfacePoint = SMath::EvaluateBSplineSurface(u, v, _du, _dv, _uKnot, _vKnot, _controlPoints);
            glm::vec3 surfacePointNormal = SMath::EvaluateBSplineNormal(u,v, _du, _dv, _UResolution, _VResolution, _uKnot, _vKnot, _controlPoints);
            glm::vec2 surfacePointTexCords = glm::vec2(u,v);

            // Store the surface point
            vertices.push_back(Vertex(surfacePoint, surfacePointNormal, surfacePointTexCords));
        }
    }

    for (int i = 0; i < _UResolution - 1; ++i) {
        for (int j = 0; j < _VResolution - 1; ++j) {
            // Compute indices of the four vertices of the current grid cell
            int topLeft = i * _VResolution + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * _VResolution + j;
            int bottomRight = bottomLeft + 1;

            // Create two triangles for this quad (top-left triangle and bottom-right triangle)
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    std::shared_ptr<DefaultMesh> surface = std::make_shared<DefaultMesh>(surfaceKey, std::move(vertices), std::move(indices), _material, _UResolution, _VResolution,_du,_dv, _uKnot,_vKnot,_controlPoints);
    surface->SetIsBSpline(true);
	return surface;
}

std::shared_ptr<Mesh> Mesh::CreatePointCloudFromLASFileSurface(const char* _fileDirectory, float _scaleFactor)
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

    // report how many points the file has
    LOG("Calculating %i points for TerrainSector", numTotalPoints);

    std::vector<PointCloudVertex> vertices;
    std::vector<Index> indices;
    glm::vec3 centroid = glm::vec3(0);

    for(int i = 0; i < numTotalPoints; i++)
    {
        if (laszip_read_point(laszip_reader))
            LOG_ERROR("DLL ERROR: reading points");

        // Apply scaling when reading point coordinates
        float x = (float)(point->X * header->x_scale_factor + header->x_offset) * _scaleFactor;
        float y = (float)(point->Y * header->y_scale_factor + header->y_offset) * _scaleFactor;
        float z = (float)(point->Z * header->z_scale_factor + header->z_offset) * _scaleFactor;

        glm::vec3 colorf = glm::vec3(0);
        glm::vec3 vertPos = glm::vec3(x, z, y);

        vertices.push_back(PointCloudVertex(vertPos, colorf));
        indices.push_back(i);
    }

    // Clean up the LASzip reader
    if (laszip_close_reader(laszip_reader))
        LOG_ERROR("DLL ERROR: closing laszip reader");
    if (laszip_destroy(laszip_reader))
        LOG_ERROR("DLL ERROR: destroying laszip reader");

    std::shared_ptr<PointCloudMesh> surface = std::make_shared<PointCloudMesh>("TerrainSector", std::move(vertices), std::move(indices));
	return surface;
}

std::shared_ptr<Mesh> Mesh::CreateGraphSphere(const int _subdivides, const bool _instance, std::string _customName)
{
    // Create default key based on num subdivides
    std::string sphereKey = "DefaultGraphSphere" + std::to_string(_subdivides);

    // Overwrites default key if custom name is added
    if (!_customName.empty()) 
        sphereKey = _customName;
    
    // If instance enabled find mesh in cache and return it
    if (_instance)
    {
        auto it = mCache.find(sphereKey);
        if (it != mCache.end())
            return it->second;
    }

    // Gen vertices and indices from vector
    std::vector<GraphVertex> vertices;
    std::vector<Index> indices;

    GenSphere(vertices, indices, _subdivides);
    
    std::shared_ptr<GraphMesh> graphSphere = std::make_shared<GraphMesh>(sphereKey, std::move(vertices), std::move(indices));

    // If instance enabled add object to cache
    if (_instance)
        mCache[sphereKey] = graphSphere;

    return graphSphere;
}

std::shared_ptr<Mesh> Mesh::CreateDebugLine(std::vector<glm::vec3> _points)
{
    // Create default line key
    std::string lineKey = "DebugLine";

    std::vector<DebugVertex> vertices;
    std::vector<Index> indices;

    for (glm::vec3 pointLocation : _points)
    {
        DebugVertex newVertex = DebugVertex(pointLocation);
        vertices.push_back(newVertex);
    }

	return std::make_shared<DebugMesh>(lineKey, std::move(vertices), std::move(indices));
}

std::shared_ptr<Mesh> Mesh::CreateDebugLine(std::shared_ptr<Mesh> _mesh)
{
   // // Create default line key
   // std::string lineKey = "DebugLine";
   //
   // std::vector<DebugVertex> vertices;
   // std::vector<Index> indices;
   // std::vector<Vertex> verts = Mesh::GetVertices<Mesh, Vertex>(_mesh);
   //
   // for (const Vertex& vert : verts)
   // {
   //     DebugVertex newVertex = DebugVertex(vert.mPosition);
   //     vertices.push_back(newVertex);
   // }
   //
   // std::vector<Index> inds = _mesh->GetIndices();
   // for(const Index& index : inds)
   // {
   //     Index ind = index;
   //     indices.push_back(ind);
   // }
   //
   // return std::make_shared<DebugMesh>(lineKey, std::move(vertices), std::move(indices));
    return nullptr;
}

std::shared_ptr<Mesh> Mesh::CreateDebugLine(std::pair<glm::vec3, glm::vec3> _extents)
{
    // Create default line key
    std::string lineKey = "DebugLine";

    std::vector<DebugVertex> vertices;
    std::vector<Index> indices; 

    const float point1x = _extents.first.x;
    const float point1y = _extents.first.y;
    const float point1z = _extents.first.z;
    const float point2x = _extents.second.x;
    const float point2y = _extents.second.y;
    const float point2z = _extents.second.z;

    // Back 4 corners 
	vertices.push_back(glm::vec3(point1x, point1y, point1z)); //back left lower corner
    vertices.push_back(glm::vec3(point1x, point2y, point1z)); //back left top corner
    vertices.push_back(glm::vec3(point2x, point2y, point1z)); //back right top corner
    vertices.push_back(glm::vec3(point2x, point1y, point1z)); //back right lower corner

    // Front 4 corners
    vertices.push_back(glm::vec3(point1x, point1y, point2z)); //back left lower corner
    vertices.push_back(glm::vec3(point1x, point2y, point2z)); //back left top corner
    vertices.push_back(glm::vec3(point2x, point2y, point2z)); //back right top corner
    vertices.push_back(glm::vec3(point2x, point1y, point2z)); //back right lower corner

    // Back Square
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);

    // Front Square
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(4);

    // right square
    indices.push_back(7);
    indices.push_back(3);
    indices.push_back(2);
    indices.push_back(6);

    // Left Square
    indices.push_back(5);
    indices.push_back(1);

    // bottom and top done by proxy

	//Generate mesh moveing the vertices and indices into new object along with input material and add it to cache
    return std::make_shared<DebugMesh>(lineKey, std::move(vertices),std::move(indices));
}


std::shared_ptr<Mesh> Mesh::Load(const std::string& _key)
{
    // Find key in cache and load that mesh
    auto it = mCache.find(_key);
    if (it != mCache.end())
        return it->second;
    
    LOG_WARNING("%s: Not found in cache for loading", _key.c_str());
    return nullptr;
}

void Mesh::Unload(const std::string& _key)
{
    // Find key in cache and unload that mesh from the cache-
    auto it = mCache.find(_key);
    if (it != mCache.end())
    {
        mCache.erase(it);
    }
    LOG_WARNING("%s: Not found in cache for unloading", _key.c_str());
}


void Mesh::ClearCache()
{
    mCache.clear();
}

void Mesh::GenSphere(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const int _numSubdivides, float _radius)
{
    // initial vertex position to create a base mesh
    glm::vec3 v0 = glm::vec3(0, 0, _radius);
    glm::vec3 v1 = glm::vec3(_radius, 0, 0);
    glm::vec3 v2 = glm::vec3(0, _radius, 0);
    glm::vec3 v3 = glm::vec3(-_radius, 0, 0);
    glm::vec3 v4 = glm::vec3(0, -_radius, 0);
    glm::vec3 v5 = glm::vec3(0, 0, -_radius);

    // SubDivide positions by num subdivides input
    SubDivide(_vertices, _indices, v0, v1, v2, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v2, v3, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v3, v4, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v4, v1, _numSubdivides, _radius);

    SubDivide(_vertices, _indices, v5, v2, v1, _numSubdivides,_radius);
    SubDivide(_vertices, _indices, v5, v3, v2, _numSubdivides,_radius);
    SubDivide(_vertices, _indices, v5, v4, v3, _numSubdivides,_radius);
    SubDivide(_vertices, _indices, v5, v1, v4, _numSubdivides,_radius);
}

void Mesh::GenSphere(std::vector<GraphVertex>& _vertices, std::vector<Index>& _indices, const int _numSubdivides,
	float _radius)
{
    // initial vertex position to create a base mesh
    glm::vec3 v0 = glm::vec3(0, 0, _radius);
    glm::vec3 v1 = glm::vec3(_radius, 0, 0);
    glm::vec3 v2 = glm::vec3(0, _radius, 0);
    glm::vec3 v3 = glm::vec3(-_radius, 0, 0);
    glm::vec3 v4 = glm::vec3(0, -_radius, 0);
    glm::vec3 v5 = glm::vec3(0, 0, -_radius);

    // SubDivide positions by num subdivides input
    SubDivide(_vertices, _indices, v0, v1, v2, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v2, v3, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v3, v4, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v0, v4, v1, _numSubdivides, _radius);

    SubDivide(_vertices, _indices, v5, v2, v1, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v5, v3, v2, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v5, v4, v3, _numSubdivides, _radius);
    SubDivide(_vertices, _indices, v5, v1, v4, _numSubdivides, _radius);
}

void Mesh::SubDivide(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC, const int _numSubdivides, float _radius)
{
    // if num subdivide min is reached pass positional values into make triangle
    // otherwise process, normalize and subdivide further.
    if (_numSubdivides > 0)
    {
        glm::vec3 v1 = _vecA + _vecB;
        v1 = normalize(v1) * _radius;

        glm::vec3 v2 = _vecA + _vecC;
        v2 = normalize(v2) * _radius;

        glm::vec3 v3 = _vecC + _vecB;
        v3 = normalize(v3) * _radius;

        SubDivide(_vertices, _indices, _vecA, v1, v2, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v1, _vecB, v3, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v2, v3, _vecC, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v1, v3, v2, _numSubdivides - 1, _radius);
    }
    else
    	MakeTriangle(_vertices, _indices, _vecA, _vecB, _vecC);
    
}

void Mesh::SubDivide(std::vector<GraphVertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA,
	const glm::vec3& _vecB, const glm::vec3& _vecC, const int _numSubdivides, float _radius)
{
    // if num subdivide min is reached pass positional values into make triangle
   // otherwise process, normalize and subdivide further.
    if (_numSubdivides > 0)
    {
        glm::vec3 v1 = _vecA + _vecB;
        v1 = normalize(v1) * _radius;

        glm::vec3 v2 = _vecA + _vecC;
        v2 = normalize(v2) * _radius;

        glm::vec3 v3 = _vecC + _vecB;
        v3 = normalize(v3) * _radius;

        SubDivide(_vertices, _indices, _vecA, v1, v2, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v1, _vecB, v3, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v2, v3, _vecC, _numSubdivides - 1, _radius);
        SubDivide(_vertices, _indices, v1, v3, v2, _numSubdivides - 1, _radius);
    }
    else
        MakeTriangle(_vertices, _indices, _vecA, _vecB, _vecC);
}

void Mesh::MakeTriangle(std::vector<Vertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC)
{
    // Calculate normal for the triangle (assuming clockwise vertex order)
    glm::vec3 normal = glm::normalize(glm::cross(_vecB - _vecA, _vecC - _vecA));

    // Calculate texture coordinates based on vertex positions
    glm::vec2 texCoordA = CalculateTexCoord(_vecA);
    glm::vec2 texCoordB = CalculateTexCoord(_vecB);
    glm::vec2 texCoordC = CalculateTexCoord(_vecC);

    // Add vertices with calculated normal and texture coordinates
    _vertices.emplace_back(Vertex{ _vecA, normal, texCoordA });
    _vertices.emplace_back(Vertex{ _vecB, normal, texCoordB });
    _vertices.emplace_back(Vertex{ _vecC, normal, texCoordC });

    // Calculate indices for the newly added vertices
    unsigned int baseIndex = static_cast<unsigned int>(_vertices.size()) - 3;
    _indices.emplace_back(baseIndex);
    _indices.emplace_back(baseIndex + 1);
    _indices.emplace_back(baseIndex + 2);
}

void Mesh::MakeTriangle(std::vector<GraphVertex>& _vertices, std::vector<Index>& _indices, const glm::vec3& _vecA, const glm::vec3& _vecB, const glm::vec3& _vecC)
{
    // Add vertices with calculated normal and texture coordinates
    _vertices.emplace_back(GraphVertex{ _vecA});
    _vertices.emplace_back(GraphVertex{ _vecB});
    _vertices.emplace_back(GraphVertex{ _vecC});

    // Calculate indices for the newly added vertices
    unsigned int baseIndex = static_cast<unsigned int>(_vertices.size()) - 3;
    _indices.emplace_back(baseIndex);
    _indices.emplace_back(baseIndex + 1);
    _indices.emplace_back(baseIndex + 2);
}

glm::vec2 Mesh::CalculateTexCoord(const glm::vec3& _vec)
{
    // Convert spherical coordinates to texture coordinates
    float u = 0.5f + (float) atan2(_vec.z, _vec.x) / (2 * M_PI);
    float v = 0.5f - (float) asin(_vec.y) / M_PI;
    return glm::vec2(u, v);
}

//std::vector<K>& Mesh::GetVertices(std::shared_ptr<T> _mesh)
//{
   // if (auto defmesh = std::dynamic_pointer_cast<DefaultMesh>(_mesh)) {
   //     return defmesh->GetVertices();  
   // }
   // else if (auto grmesh = std::dynamic_pointer_cast<GraphMesh>(_mesh)) {
   //     return grmesh->GetVertices();  
   // }
   // else if (auto debmesh = std::dynamic_pointer_cast<DebugMesh>(_mesh)) {
   //     return debmesh->GetVertices();  
   // }
   // else if (auto pcmesh = std::dynamic_pointer_cast<PointCloudMesh>(_mesh)) {
   //     return pcmesh->GetVertices();  
   // }
   //
   // LOG_ERROR("Type mismatch in GetVertices");
   // throw std::runtime_error("Type mismatch in GetVertices");  // Ensure you return something in all cases
//}


//template <typename T>
//std::pair<glm::vec3, glm::vec3> Mesh::GetMeshMinMaxExtent(std::shared_ptr<T> _mesh)
//{
//    // Deduce the vertex type from the return type of GetVertices()
//    using K = typename std::decay<decltype(_mesh->GetVertices()[0])>::type;
//
//    // Calculate the bounding box (min and max extents) of the existing mesh
//    std::vector<K>& collisionMeshVertices = _mesh->GetVertices();
//    glm::vec3 maxExtent = collisionMeshVertices[0].mPosition;
//    glm::vec3 minExtent = collisionMeshVertices[0].mPosition;
//
//    for (K& vertex : collisionMeshVertices)
//    {
//        minExtent = glm::min(minExtent, vertex.mPosition);
//        maxExtent = glm::max(maxExtent, vertex.mPosition);
//    }
//
//    // Slight offset
//    maxExtent += 0.001f;
//    minExtent -= 0.001f;
//
//	return std::make_pair(minExtent,maxExtent);
//}

