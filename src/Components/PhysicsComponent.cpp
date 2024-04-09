
// Classes
#include <Components/PhysicsComponent.h>
#include <SceneActors.h>

PhysicsComponent::~PhysicsComponent()
{
    delete mGroundReference;
}

// Overriden function
void PhysicsComponent::Update(float _dt)
{
	UpdateForces();
	UpdateVelocity(_dt);
	UpdatePosition(_dt);

    //TODO: Fix all stupid extent calculations, this should not be hardcoded
    ConformToGround(0.5);

	ResetValues();
}

void PhysicsComponent::UpdateForces()
{
	// Updates acceleration to equal gravity each tick
	glm::vec3 gravity{ 0.f,-9.81f,0.f };
	mAcceleration = gravity;
}

void PhysicsComponent::UpdateVelocity(float _dt)
{
	// Updates velocity by adding acceleration each tick
	mVelocity += mAcceleration * _dt;

	// Sets velocity to maxSpeed if reached so there is no infinite acceleration
	if(glm::length(mVelocity) > mMaxSpeed)
		mVelocity = glm::normalize(mVelocity) * mMaxSpeed;
}

void PhysicsComponent::UpdatePosition(float _dt)
{
	// sets the owners position based on velocity and delta time
	if(mOwner)
		mOwner->SetPosition(mOwner->GetPosition(Actor::TransformSpace::Global) + (mVelocity * _dt), Actor::TransformSpace::Global);
}

void PhysicsComponent::ResetValues()
{
	mAcceleration = glm::vec3(0.f);
}

void PhysicsComponent::ConformToGround(float _parentExtent)
{
    if (!mGroundReference) return;

    // Gets the ground planes mesh
    Mesh* groundPlane = mGroundReference->GetActorMesh();

    // Gets the vertices for positional values and indices for triangle point pairing.
    std::vector<Vertex>& planeVertices = groundPlane->GetVertices();
    std::vector<Index>& planeIndices = groundPlane->GetIndices();

    // Gets parent position
    glm::vec3 actorPos(mOwner->GetPosition(Actor::TransformSpace::Global));

    // Iterates through indices in pairs of three to calc one triangle at a time
    // NOTE:: This is crazy performance intensive, should update to triangle neighbour searching method
    // NOTE:: Does not take into account relative position for ease of mind because it f`s up the math (not sure why)
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
        point1 *= mGroundReference->GetScale(Actor::TransformSpace::Global);
        point2 *= mGroundReference->GetScale(Actor::TransformSpace::Global);
        point3 *= mGroundReference->GetScale(Actor::TransformSpace::Global);

        // perhaps + actor world position for live moveing of terrain, however this messes with height calculations because of uneven positioning
        // point1 += mGroundPlane->GetPosition(TransformSpace::Global);
        // point2 += mGroundPlane->GetPosition(TransformSpace::Global);
        // point3 += mGroundPlane->GetPosition(TransformSpace::Global);

        // actor world rotation for live rotating of terrain
        glm::quat planesRotation = mGroundReference->GetRotation(Actor::TransformSpace::Global);
        point1 = glm::rotate(planesRotation, point1);
        point2 = glm::rotate(planesRotation, point2);
        point3 = glm::rotate(planesRotation, point3);

        // Calculate the Barycentric Coordinates of the components parent with the world transform of the three points.
        glm::vec3 baryCoords = GetBarycentricCoordinates(point1, point2, point3, actorPos);

        // if clause is reached we know the actor-pos is within this triangle
        if (baryCoords.x > 0 && baryCoords.x < 1 &&
            baryCoords.y > 0 && baryCoords.y < 1 &&
            baryCoords.z > 0 && baryCoords.z < 1)
        {
            // log triangle index
            // std::cout << "Actor within triangle = " << index1 << " " << index2 << " " << index3 << std::endl;

            // Calculates the height of a point in a triangle given by the Barycentric Coordinates and the three points of the triangle
        	float hight = GetHightFromBarycentricCoordinates(baryCoords, point1, point2, point3);
            // Sets the height tor the ground reference and to parent extent
            hight += mGroundReference->GetPosition(Actor::TransformSpace::Global).y;
            hight += _parentExtent;

            // If owning actor is bellow the height of the collision, set it to height of collision so it conforms to ground geometry.
            if (mOwner->GetPosition(Actor::TransformSpace::Global).y < hight)
            {
                // Gets the owners position and overwrites the height with new collision value
                glm::vec3 playPos = mOwner->GetPosition(Actor::TransformSpace::Global);
                playPos.y = hight;

                // Set the owners position
                mOwner->SetPosition(playPos, Actor::TransformSpace::Global);
            }
        }
    }
}


glm::vec3 PhysicsComponent::GetBarycentricCoordinates(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _actorPos)
{
    // Sets all height to 0 so they do not interfere with calculations
    _p1.y = 0;
    _p2.y = 0;
    _p3.y = 0;
    _actorPos.y = 0;

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

float PhysicsComponent::GetHightFromBarycentricCoordinates(const glm::vec3& _barCoords, const glm::vec3& _p1, const glm::vec3& _p2, const glm::vec3& _p3)
{
    // Generic function to return height by barycentric coordinates and triangle points
    float hight = _barCoords.x * _p1.y + _barCoords.y * _p2.y + _barCoords.z * _p3.y;
    return hight;

}