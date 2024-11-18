
// Includes
#include <Components/PhysicsComponent.h>
#include <Actor.h>
#include <Core/SMath.h>

// Overriden function
void PhysicsComponent::Update(float _dt)
{
	UpdateForces();
	ConformToSurface();
	UpdateVelocity(_dt);
	UpdatePosition(_dt);

   

	ResetValues();
}

void PhysicsComponent::UpdateForces()
{
	if(mGravityEnabled)
	{
		// Updates acceleration to equal gravity each tick
		glm::vec3 gravity{ 0.f,-9.81f,0.f };
		mAcceleration = gravity;
	}
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

void PhysicsComponent::ResetForces()
{
	mAcceleration = glm::vec3(0.f);
	mVelocity = glm::vec3(0.f);
}

void PhysicsComponent::ConformToSurface()
{
	// Reset contact with ground
	inContactWithGround = false;

	mDebugBarryLocations.clear();

	glm::vec3 faceNormals;
	if (SMath::ConformPhysicsObjectToGeometry(mOwner, mSurfaceReference, faceNormals,0.1f))
	{
		inContactWithGround = true;
		mVelocity.y = 0;

		float gravity = -0.981f;
		
		glm::vec3 faceAcceleration = glm::vec3(
			faceNormals.x * faceNormals.y * gravity,     // X 
			(faceNormals.y * faceNormals.y - 1.0f) * gravity, // Y 
			faceNormals.z * faceNormals.y * gravity      // Z 
		);

		faceAcceleration *= glm::vec3(-1, -1, -1);
		mAcceleration += faceAcceleration;
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

void PhysicsComponent::AddVelocity(glm::vec3 _inVel)
{
	mVelocity += _inVel;
}

