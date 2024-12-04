#pragma once

// Includes
#include <Components/Component.h>
#include <Collision/CollisionProperties.h>

// Additional Includes
#include <memory>
#include <glm/glm.hpp>

// Forward Declarations
class Actor;
class VisualActor;

/**
 * @class CollisionComponent
 * @brief 
 */
class CollisionComponent : public Component
{
	// Re-iterate that Actor classes are friends, even though its called in the base component.
	friend Actor;
public:
	// ---------- Global Variables --------------

	glm::vec3 mCenter{ 0.f,0.f,0.f };
	glm::vec3 mMinExtent{ 0.f,0.f,0.f };
	glm::vec3 mMaxExtent{ 0.f,0.f,0.f };
	glm::vec3 mExtent{ 0.f,0.f,0.f };
	float mRadius{ 0.5f };

	// objects collision properties
	CollisionProperties mCollisionProperties = { CollisionType::STATIC, CollisionResponse::BLOCK, CollisionBase::AABB };

private:
	// ---------- Local Variables --------------


public:
	// ---------- Global functions --------------

	CollisionComponent(const std::string& _name, std::shared_ptr<Actor> _owner);

	// Allow moving
	CollisionComponent(CollisionComponent&&) noexcept = default;
	CollisionComponent& operator=(CollisionComponent&&) noexcept = default;

private:
	// ---------- Local functions --------------



public:
	// ---------- Getters and setters --------------


};
