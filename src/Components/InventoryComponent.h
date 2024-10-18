#pragma once

// Includes
#include <Components/Component.h>

// Additional Includes
#include <memory>
#include <glm/glm.hpp>

// Forward Declarations
class Actor;
class VisualActor;

struct Item
{
	int ID;
	std::string mName;

	Item(int Id, std::string name) : ID(Id), mName(name) {}

};

/**
 * @class InventoryComponent
 * @brief 
 */
class InventoryComponent : public Component
{
	// Re-iterate that Actor classes are friends, even though its called in the base component.
	friend Actor;
public:
	// ---------- Global Variables --------------

	std::vector<std::shared_ptr<Item>> mItems;

private:
	// ---------- Local Variables --------------

	

public:
	// ---------- Global functions --------------

	// Constructs a physics components  and attaches it to input actor
	InventoryComponent(const std::string& _name, std::shared_ptr<Actor> _owner)
		: Component(_name, _owner) {}

	~InventoryComponent() override;

	void AddItem(std::shared_ptr<Item> newItem);


private:
	// ---------- Local functions --------------



public:
	// ---------- Getters and setters --------------


};