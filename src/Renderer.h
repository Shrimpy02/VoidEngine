#pragma once

// Includes

// Additional Includes
#include <memory> 

// Forward Declarations

/**
 * @class IRender,
 * @brief The render interface, holds the virtual draw function and class type inheritance.
 */
class IRender
{
public:
	// Default construct logic

	// Init to 0 as draw logic changes per class
	virtual void Draw(const std::shared_ptr<class Shader> _shader = nullptr) const = 0;
};