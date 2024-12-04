
// Includes
#include <Components/CollisionComponent.h>
#include <RenderElements/Mesh.h>

CollisionComponent::CollisionComponent(const std::string& _name, std::shared_ptr<Actor> _owner)
	: Component(_name, _owner)
{}
