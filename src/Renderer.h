#pragma once

// Render interface
class IRender
{
public:
	virtual void Draw(const class Shader* _shader = nullptr) const = 0;
};