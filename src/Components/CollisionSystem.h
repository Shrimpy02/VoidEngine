#pragma once

// Includes
#include <Components/System.h>
#include <Components/CollisionComponent.h>
#include <glm/glm.hpp>

// Additional Includes
#include <memory>
#include <array>

// Forward Declarations
class Actor;
class VisualActor;

/**
 * @class CollisionSystem
 * @brief
 */
class CollisionSystem : public System
{
public:
	// ---------- Global Variables --------------

private:
	// ---------- Local Variables --------------
	
	std::vector<CollisionComponent> mCollisionComponents = {};

public:
	// ---------- Global functions --------------

	// Constructs an ai components  and attaches it to input actor
	CollisionSystem();

	// De-constructor
	~CollisionSystem() override;

	// Overridden Update for the component, distribution to all tick functions from here. (Called each frame)
	void Update(float _dt);

	void CheckCollision();

private:
	// ---------- Local functions --------------

	bool IntersectsAABBx2(CollisionComponent& _c1, CollisionComponent& _c2, glm::vec3& _mtv);

public:
	// ---------- Getters and setters --------------

	void CrateComponent(std::string _name, std::shared_ptr<Actor> _actor, CollisionResponse _inRespones, CollisionType _inType, CollisionBase _inBase);

};