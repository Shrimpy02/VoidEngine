
// Includes
#include <SceneActors.h>
#include <Utilities/Logger.h>

// ---------------------------------------------------------------
// --------------------- Visual Actor ------------------------------
// ---------------------------------------------------------------

VisualActor::VisualActor(const std::string& _name, Mesh* _mesh)
    : Actor(_name), mMesh(_mesh)
{
    LOG("VisualActor Created: %s", _name.c_str());
}

void VisualActor::Draw(const Shader* _shader) const
{
    if (!mMesh) return;

    mMesh->Draw(_shader);
}

// ---------------------------------------------------------------
// --------------------- BaseActor ------------------------------
// ---------------------------------------------------------------

BaseActor::BaseActor(const std::string& _name, Mesh* _mesh)
    :Actor(_name), mMesh(_mesh)
{
    // Creates and remembers both collision meshes for viualization and custom changing of collision base at run time. Is unoptomized.
    mCollisionCube = CreateCollisionCubeFromMesh(Material::Load("Debug"), mMesh->GetVertices());
    mCollisionSphere = CreateCollisionSphereFromMesh(Material::Load("Debug"), mMesh->GetVertices());
    LOG("BaseActor Created: %s", _name.c_str());
}

BaseActor::~BaseActor()
{
    delete mGroundPlane;
}

void BaseActor::Draw(const Shader* _shader) const
{
    // Draw visual mesh first
    if (!mMesh) return;
    mMesh->Draw(_shader);

    // Draw collision mesh after depending on collision base, draw as wire frame
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (mShouldDrawCollisionMesh) {

        if (mCollisionCube && mCollisionProperties.IsAABB())
            mCollisionCube->Draw(_shader);

    	else if(mCollisionSphere && mCollisionProperties.IsBoundingSphere())
            mCollisionSphere->Draw(_shader);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void BaseActor::Update(float _dt)
{
    Mesh* groundPlane = mGroundPlane->GetActorMesh();
    std::vector<Vertex>& planeVertices = groundPlane->GetVertices();
    std::vector<Index>& planeIndices = groundPlane->GetIndices();

    glm::vec3 actorPos(GetPosition(TransformSpace::Global));

    for(int i = 0; i < planeIndices.size()-2; i+=3)
    {
        unsigned int index1 = planeIndices[i];
        unsigned int index2 = planeIndices[i+1];
        unsigned int index3 = planeIndices[i+2];

        glm::vec3 point1(planeVertices[index1].mPosition);
        glm::vec3 point2(planeVertices[index2].mPosition);
        glm::vec3 point3(planeVertices[index3].mPosition);

        point1 *= mGroundPlane->GetScale(TransformSpace::Global);
        point2 *= mGroundPlane->GetScale(TransformSpace::Global);
        point3 *= mGroundPlane->GetScale(TransformSpace::Global);

       // point1 += mGroundPlane->GetPosition(TransformSpace::Global);
       // point2 += mGroundPlane->GetPosition(TransformSpace::Global);
       // point3 += mGroundPlane->GetPosition(TransformSpace::Global);

        glm::quat planesRotation = mGroundPlane->GetRotation(Actor::TransformSpace::Global);
        
        // Rotate points by quaternion
        point1 = glm::rotate(planesRotation, point1);
        point2 = glm::rotate(planesRotation, point2);
        point3 = glm::rotate(planesRotation, point3);

        glm::vec3 baryCoords = GetBarycentricCoordinates(point1, point2, point3, actorPos);

        if (baryCoords.x > 0 && baryCoords.x < 1 &&
            baryCoords.y > 0 && baryCoords.y < 1 &&
            baryCoords.z > 0 && baryCoords.z < 1)
        {
            std::cout << "Actor within triangle = " << index1 << " " << index2 << " " << index3 << std::endl;

            glm::vec3 scaledMinExtent = mMinExtent * GetScale(TransformSpace::Global);
            glm::vec3 scaledMaxExtent = mMaxExtent * GetScale(TransformSpace::Global);
            glm::vec3 extent = (scaledMaxExtent - scaledMinExtent) * 0.5f;

           // point1.y = mGroundPlane->GetPosition(TransformSpace::Global).y;
           // point2.y += mGroundPlane->GetPosition(TransformSpace::Global).y;
           // point3.y += mGroundPlane->GetPosition(TransformSpace::Global).y;

        	float hight = GetHightFromBarycentricCoordinates(baryCoords, point1, point2, point3);
            hight += mGroundPlane->GetPosition(TransformSpace::Global).y;
            hight += extent.y;

        	if(GetPosition(TransformSpace::Global).y < hight)
            {
                glm::vec3 playPos = GetPosition(TransformSpace::Global);
                playPos.y = hight;

                SetPosition(playPos, TransformSpace::Global);

            }

        }

    }
}


glm::vec3 BaseActor::GetBarycentricCoordinates(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _actorPos)
{
    _p1.y = 0;
    _p2.y = 0;
    _p3.y = 0;
    _actorPos.y = 0;

    // Gets the vectors from point 1 to point 2 and 3
    glm::vec3 p12 = _p2 - _p1;
    glm::vec3 p13 = _p3 - _p1;

    // Gets the normal of these two vectors
    glm::vec3 n = glm::cross(p12, p13);
    // the normals length / 2 = the triangles total area
    float area_123 = n.y;

    // make sure there is no divide by 0 error
    if (area_123 == 0)
        throw std::runtime_error("ERROR::DivideBy0");

    glm::vec3 barCoords; // the coords to return

    // Sub triangle 1
    // Gets the vector between actor's position in triangle and point 2 and point 3 
    glm::vec3 p = _p2 - _actorPos;
    glm::vec3 q = _p3 - _actorPos;
    // gets the normal
    n = glm::cross(p, q);
    // gets the area of sub triangle
    barCoords.x = n.y / area_123;

    // Sub triangle 2
    p = _p3 - _actorPos;
    q = _p1 - _actorPos;
    n = glm::cross(p, q);
    barCoords.y = n.y / area_123;

    // Sub triangle 3
    p = _p1 - _actorPos;
    q = _p2 - _actorPos;
    n = glm::cross(p, q);
    barCoords.z = n.y / area_123;

    return barCoords;
}

float BaseActor::GetHightFromBarycentricCoordinates(const glm::vec3& _barCoords,const glm::vec3& _p1,const glm::vec3& _p2, const glm::vec3& _p3)
{

    float hight = _barCoords.x * _p1.y + _barCoords.y * _p2.y + _barCoords.z * _p3.y;
    return hight;

}

float BaseActor::Function(float x, float y)
{


    return 0.0f;
}

AABB BaseActor::GetAABB() const
{
    if (mCollisionCube)
    {
        // Calculate scaled min and max extents
        glm::vec3 scaledMinExtent = mMinExtent * GetScale(TransformSpace::Global);
        glm::vec3 scaledMaxExtent = mMaxExtent * GetScale(TransformSpace::Global);

        // Apply object's position
        glm::vec3 position = GetPosition(TransformSpace::Global);

        // Calculate local center of the AABB, there is a center variable but is calculated at setup and there fore is not dynamic
        glm::vec3 center = (scaledMinExtent + scaledMaxExtent) * 0.5f + position;

        // calculated total extent of the AABB from centre 
        glm::vec3 extent = (scaledMaxExtent - scaledMinExtent) * 0.5f;

        // Construct and return the AABB
        return AABB(center, extent);
    }
    else {
        // Return default AABB
        LOG_WARNING("Default AABB Returned");
        return AABB(GetPosition(TransformSpace::Global), GetScale(TransformSpace::Global) * 0.5f);
    }
}

BoundingSphere BaseActor::GetBoundingSphere() const
{
    if (mCollisionSphere)
    {
        // getes the average scale so that the sphere is kind of scaled with mesh scaleing. Temp fix
        float averageScale(0);
        for (int i = 0; i < 3; i++)
        {
            averageScale += GetScale(Actor::TransformSpace::Global)[i];
        }
        averageScale /= 3;

        // Calculate scaled min and max extents
        float scaledRadius = mRadius * averageScale;

        // Apply object's position
        glm::vec3 position = GetPosition(TransformSpace::Global);

        // Calculate local center of the AABB
        glm::vec3 center = glm::vec3(0.f) + position;

        // Calculate extent of the AABB
        float radius = glm::length(scaledRadius);

        // Construct and return the AABB
        return BoundingSphere(center, radius);
    }
    else {
        // Return default Bounding sphere
        LOG_WARNING("Default BoundignSphere Returned");
        return BoundingSphere(GetPosition(TransformSpace::Global), 0.5f);
    }
}

bool BaseActor::LineTraceTroughTriangle(glm::vec3 _TP1, glm::vec3 _TP2, glm::vec3 _TP3, glm::vec3 _startPos, glm::vec3 _endPos, glm::vec3& _intersectPoint)
{

    // Gets the direction of the line trace as a unit vector
    glm::vec3 traceDirection = glm::normalize(_endPos - _startPos);







   
   // std::cout << traceDirection.x << ", " << traceDirection.y << ", " << traceDirection.z << std::endl;

    return true;

}


CollisionProperties* BaseActor::GetCollisionProperties()
{
    // Returns pointer to class object
    return &mCollisionProperties;
}


// ---------------------------------------------------------------
// --------------------- CollisionActor ------------------------------
// ---------------------------------------------------------------

CollisionActor::CollisionActor(const std::string& _name, Mesh* _mesh, CollisionProperties _inCollisionProps)
    : Actor(_name), mCollisionMesh(_mesh), mCollisionProperties(_inCollisionProps)
{
    // System to calculates extent and centre conforming to init collision mesh. 
    // TODO : kind of does`nt work with un-even scale or with object rotation. That needs to be figured out

	// default extent init
    glm::vec3 maxExtent(0);
    glm::vec3 minExtent(0);
    glm::vec3 center(0);
    float largetsDiff(0.f);

    // For each
    for (int i = 0; i < 3; i++)
    {
        glm::vec3 pos(0);

        // check all vertices looking for the longest vertex away.  
        for (int j = 0; j < _mesh->GetVertices().size(); j++)
        {
            // Gets max extent for axis
            if (_mesh->GetVertices()[j].mPosition[i] > maxExtent[i])
                maxExtent[i] = _mesh->GetVertices()[j].mPosition[i];

            // Gets min extent for axis
            else if (_mesh->GetVertices()[j].mPosition[i] < minExtent[i])
                minExtent[i] = _mesh->GetVertices()[j].mPosition[i];

            // Gets center location for axis 
            pos[i] = _mesh->GetVertices()[j].mPosition[i];

            // Finds the vertex that is the furthest from the center
            if (largetsDiff < abs(_mesh->GetVertices()[j].mPosition[i]))
                largetsDiff = abs(_mesh->GetVertices()[j].mPosition[i]);
        }
        center += pos;
    }

    // Divide by num vertices for average location of center
    center /= static_cast<float>(_mesh->GetVertices().size());

    // set the class values to the calculated values
    mMaxExtent = maxExtent;
    mMinExtent = minExtent;
    mCenter = center;
    mRadius = largetsDiff;

    // Gets the debug material from material cache
    std::string debugString = "Debug";
    mCollisionMesh->SetMaterial(Material::GetMaterialFromCache(debugString));

    // Crude log for assimp import test
    if(mCollisionProperties.IsAABB())
        LOG("CollisionActor(AABB) Created: %s", _name.c_str());
    else
        LOG("CollisionActor(BoundingSphere) Created: %s", _name.c_str());
}

void CollisionActor::Draw(const Shader* _shader) const
{
    if (!mShouldDrawCollisionMesh) return;

    if (mCollisionMesh)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mCollisionMesh->Draw(_shader);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

AABB CollisionActor::GetAABB() const
{
    if (mCollisionMesh)
    {
        // Calculate scaled min and max extents
        glm::vec3 scaledMinExtent = mMinExtent * GetScale(TransformSpace::Global);
        glm::vec3 scaledMaxExtent = mMaxExtent * GetScale(TransformSpace::Global);

        // Apply object's position
        glm::vec3 position = GetPosition(TransformSpace::Global);

        // Gets the center position after transform. Witch is good. 
        // Calculate local center of the AABB
        glm::vec3 center = (scaledMinExtent + scaledMaxExtent) * 0.5f + position;

        // Calculate extent of the AABB
        glm::vec3 extent = (scaledMaxExtent - scaledMinExtent) * 0.5f;

        // Construct and return the AABB
        return AABB(center, extent);
    }
    else {

        // Return default AABB
        LOG_WARNING("Default AABB Returned");
        return AABB(GetPosition(TransformSpace::Global), GetScale(TransformSpace::Global) * 0.5f);
    }
}

BoundingSphere CollisionActor::GetBoundingSphere() const
{
    if (mCollisionMesh)
    {
        // getes the average scale so that the sphere is kind of scaled with mesh scaleing. Temp fix
        float averageScale(0);
        for(int i = 0; i < 3; i++)
        {
            averageScale += GetScale(Actor::TransformSpace::Global)[i];
        }
        averageScale /= 3;

        // Calculate scaled min and max extents
        float scaledRadius = mRadius * averageScale;

        // Apply object's position
        glm::vec3 position = GetPosition(TransformSpace::Global);

        // Gets the center position after transform. Witch is good. 
        // Calculate local center of the AABB
        glm::vec3 center = glm::vec3(0.f) + position;

        // Calculate extent of the AABB
        float radius = glm::length(scaledRadius);

        // Construct and return the AABB
        return BoundingSphere(center, radius);
    }
    else {
        // Return default Bounding sphere
        LOG_WARNING("Default BoundignSphere Returned");
        return BoundingSphere(GetPosition(TransformSpace::Global), 0.5f);
    }

}

CollisionProperties* CollisionActor::GetCollisionProperties() 
{
    return &mCollisionProperties;
}
