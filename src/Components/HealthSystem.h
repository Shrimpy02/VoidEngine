#pragma once

// Includes
#include <Components/System.h>

// Additional Includes
#include <memory> 

// Forward Declarations
class HealthComponent;

// Enums

/**
 * @class HealthSystem
 * @brief
 */
class HealthSystem : public System
{
public:
	// ---------- Global Variables --------------

private:
	// ---------- Local Variables --------------

	// All AI Components
	std::vector<std::shared_ptr<HealthComponent>> mHealthComponents;

public:
	// ---------- Global functions --------------

	//
	HealthSystem() : System() {};

	// De-constructor
	~HealthSystem() override;

	void Update(float _dt);

	void DoDamage(std::shared_ptr<HealthComponent> _comp, int _damage);

	void AddHealth(std::shared_ptr<HealthComponent> _comp, int _health);

	bool IsDead(std::shared_ptr<HealthComponent> _comp);

	bool IsDead(std::shared_ptr<Actor> _actor);

private:
	// ---------- Local functions --------------



public:
	// ---------- Getters and setters --------------

	void AddComponent(std::shared_ptr<HealthComponent> _inComponent);

	void SetHealth(std::shared_ptr<HealthComponent> _inComp, int _health);

	int GetHealth(std::shared_ptr<HealthComponent> _inComp);
};