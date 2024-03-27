#pragma once
#include <glm/vec3.hpp>

struct Light
{
	virtual ~Light() = default;
	glm::vec3 mAmbient{ 0.5f,0.5f,0.5f };
	glm::vec3 mColor{ 1.0f,1.0f,1.0f };

};