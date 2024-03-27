#pragma once
#include <Lights/Lights.h>
#include <Actor.h>

// Change this variable in the shader too
#define MAX_POINT_LIGHTS 64

class PointLight : public Light
{
public:
    // ---------- Global Variables --------------
    virtual glm::vec3 GetLightPosition() const = 0;

    // gives a light range of 50 units
    float constantVar = 1.f;
    float linearVar = 0.09f;
    float quadraticVar = 0.032f;

private:
    // ---------- Local Variables --------------


public:
    // ---------- Global functions --------------


private:
    // ---------- Local functions --------------



public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    void SetAttenuationCoefficients(float _const, float _lin, float _quad)
    {
        constantVar = _const;
        linearVar = _lin;
        quadraticVar = _quad;
    }

    // Getters

};

class PointLightActor : public Actor, public PointLight
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------


public:
    // ---------- Global functions --------------
    PointLightActor(const std::string& name) :Actor(name) {};

    virtual glm::vec3 GetLightPosition() const
    {
        return GetPosition(Actor::TransformSpace::Global);
    }


private:
    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    // Getters

};