
// Classes
#include "PhysicsComponent.h"
#include "Actor.h"

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

void PhysicsComponent::Update(float _dt)
{
	UpdateForces();
	UpdateVelocity(_dt);
	UpdatePosition(_dt);
	ResetValues();
}
