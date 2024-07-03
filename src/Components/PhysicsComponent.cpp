
// Classes
#include <Components/PhysicsComponent.h>
#include <SceneActors.h>
#include <Core/SMath.h>
#include <GLFW/glfw3.h>

PhysicsComponent::~PhysicsComponent()
{
  
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
		mOwner->SetGlobalPosition(mOwner->GetGlobalPosition() + (mVelocity * _dt));
}

void PhysicsComponent::ResetValues()
{
	mAcceleration = glm::vec3(0.f);
}

void PhysicsComponent::ConformToGround(float _parentExtent)
{
    if (!mGroundReference) return;
    inContactWithGround = false;
    // Gets the ground planes mesh
    std::shared_ptr<Mesh> groundPlane = mGroundReference->GetActorMesh();

    // Gets the vertices for positional values and indices for triangle point pairing.
    std::vector<Vertex>& planeVertices = groundPlane->GetVertices();
    std::vector<Index>& planeIndices = groundPlane->GetIndices();

    // Gets parent position
    glm::vec3 actorPos(mOwner->GetGlobalPosition());

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
        point1 *= mGroundReference->GetGlobalScale();
        point2 *= mGroundReference->GetGlobalScale();
        point3 *= mGroundReference->GetGlobalScale();

        // perhaps + actor world position for live moveing of terrain, however this messes with height calculations because of uneven positioning
        // point1 += mGroundPlane->GetPosition(TransformSpace::Global);
        // point2 += mGroundPlane->GetPosition(TransformSpace::Global);
        // point3 += mGroundPlane->GetPosition(TransformSpace::Global);

        // actor world rotation for live rotating of terrain
        glm::quat planesRotation = mGroundReference->GetGlobalRotation();
        point1 = glm::rotate(planesRotation, point1);
        point2 = glm::rotate(planesRotation, point2);
        point3 = glm::rotate(planesRotation, point3);

        // Calculate the Barycentric Coordinates of the components parent with the world transform of the three points.
        glm::vec3 baryCoords = SMath::GetBarycentricCoordinates(point1, point2, point3, actorPos);

        // if clause is reached we know the actor-pos is within this triangle
        if (baryCoords.x > 0 && baryCoords.x < 1 &&
            baryCoords.y > 0 && baryCoords.y < 1 &&
            baryCoords.z > 0 && baryCoords.z < 1)
        {
            // log triangle index
            // std::cout << "Actor within triangle = " << index1 << " " << index2 << " " << index3 << std::endl;

            // Calculates the height of a point in a triangle given by the Barycentric Coordinates and the three points of the triangle
        	float hight = SMath::GetHightFromBarycentricCoordinates(baryCoords, point1, point2, point3);
            // Sets the height tor the ground reference and to parent extent
            hight += mGroundReference->GetGlobalPosition().y;
            hight += _parentExtent;

            // If owning actor is bellow the height of the collision, set it to height of collision so it conforms to ground geometry.
            if (mOwner->GetGlobalPosition().y < hight)
            {
                // Gets the owners position and overwrites the height with new collision value
                glm::vec3 playPos = mOwner->GetGlobalPosition();
                playPos.y = hight;

                // Set the owners position
                mOwner->SetGlobalPosition(playPos);
                inContactWithGround = true;
            }
        }
    }
}

void PhysicsComponent::Jump(float jumpStrength, glm::vec3 _jumpDirection)
{

	if (!inContactWithGround) return;

    //double currentTime = glfwGetTime();
    //if (currentTime - mLastJumpTime < 0.5) return;

	mVelocity += _jumpDirection * jumpStrength;

    //mLastJumpTime = currentTime;

}

