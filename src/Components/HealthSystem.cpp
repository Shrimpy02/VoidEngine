
// includes 
#include <Components/HealthSystem.h>
#include <Components/HealthComponent.h>
#include <Actor.h>

HealthSystem::~HealthSystem()
{
}

void HealthSystem::Update(float _dt)
{
}

void HealthSystem::DoDamage(std::shared_ptr<HealthComponent> _comp, int _damage)
{
	_comp->mHealth -= _damage;
}

void HealthSystem::AddHealth(std::shared_ptr<HealthComponent> _comp, int _health)
{
	_comp->mHealth += _health;
}

bool HealthSystem::IsDead(std::shared_ptr<HealthComponent> _comp)
{
	if (!_comp) return false;

	return _comp->mHealth <= 0;
}

bool HealthSystem::IsDead(std::shared_ptr<Actor> _actor)
{
	if (!_actor || !_actor->GetHealthComponent()) return false;

	return _actor->GetHealthComponent()->mHealth <= 0;
}

void HealthSystem::SetHealth(std::shared_ptr<HealthComponent> _inComp, int _health)
{
	_inComp->mHealth = _health;
}

int HealthSystem::GetHealth(std::shared_ptr<HealthComponent> _inComp)
{
	return _inComp->mHealth;
}

void HealthSystem::AddComponent(std::shared_ptr<HealthComponent> _inComponent)
{
	mHealthComponents.push_back(_inComponent);
}
