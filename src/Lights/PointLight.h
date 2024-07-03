#pragma once

// Includes
#include <Lights/Lights.h>
#include <Actor.h>

// Additional Includes

// Forward Declarations

// Defines the max amount of lights in a scene.
// If this is changed remember to change in fragment shader as well!
#define MAX_POINT_LIGHTS 64

/**
 * @class PointLight,
 * @brief Inherits from light class, properties and logic for light falloff over distance.
 */
class PointLight : public Light
{
public:
    // This combination of default variables gives a light range of 50 units
    float constantVar = 1.f;
    float linearVar = 0.09f;
    float quadraticVar = 0.032f;

    // Returns the position of the light source as a vec3
    virtual glm::vec3 GetLightPosition() const = 0;

    // Set the AttenuationCoefficients from in floats, 
    void SetAttenuationCoefficients(float _const, float _lin, float _quad)
    {
        constantVar = _const;
        linearVar = _lin;
        quadraticVar = _quad;
    }
};

// ---------------------------------------------------------------
// ----------------- PointLightActor -----------------------------
// ---------------------------------------------------------------

/**
 * @class PointLightActor,
 * @brief The PointLightActor inherits from PointLight class
 * for light properties and Actor class for scene inclusion.
 */
class PointLightActor : public Actor, public PointLight
{
public:
    // constructs as a regular actor
    PointLightActor(const std::string& name) :Actor(name) {}

    // Inherited function returns the position of this light source as a vec3
    virtual glm::vec3 GetLightPosition() const
    {
        return GetGlobalPosition();
    }

};