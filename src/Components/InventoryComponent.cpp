
#include <Components/InventoryComponent.h>

InventoryComponent::~InventoryComponent()
{
}

void InventoryComponent::AddItem(std::shared_ptr<Item> newItem)
{
	mItems.push_back(newItem);
}
