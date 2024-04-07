#pragma once

/**
 * @class IRender,
 * @brief The render interface, holds the virtual draw function and class type inheritance.
 */
class IRender
{
public:
	// Default construct logic

	// Init to 0 as draw logic changes per class
	virtual void Draw(const class Shader* _shader = nullptr) const = 0;
};