
#include <Components/PhysicsSystem.h>

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::Update(float _dt)
{
}

void PhysicsSystem::AddComponent(std::shared_ptr<PhysicsComponent> _inComponent)
{
	mPhysicsComponents.push_back(_inComponent);
}

