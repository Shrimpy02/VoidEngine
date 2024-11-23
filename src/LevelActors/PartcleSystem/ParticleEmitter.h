#pragma once

// Includes
#include <Renderer.h>
#include <Actor.h>
#include <LifeTime.h>
#include <chrono>


// Additional Includes
#include <memory>

// Forward Declarations
class Shader;
class Mesh;

/**
 * @class ParticleEmitter
 * @brief Represents a complete object in a scene, it inherits from actor for world location,
 * IRender for mesh rendering and IBounded for collision detection and processing.
 */
class ParticleEmitter : public Actor, public IRender, public ILifeTime
{
public:
    // ---------- Global Variables --------------

private:
    // ---------- Local Variables --------------

    std::vector<std::shared_ptr<VisualActor>> mParticles;
    std::shared_ptr<VisualActor> mSurfacereReference{ nullptr };
     std::shared_ptr<Mesh> mParticleMesh{ nullptr };
    float mParticleLifetime = 10.f;
    float mParticleGravity = -1.5f;
    int mNumParticles = 1000;
    double mEmitterDuration = 20.f;
    float mEmitterEmitDelay = 0.1f;
    bool mEmitterActive = true;
    float mEmitterSpawnRange = 1.f;


    // high res timer aproach
    std::chrono::time_point<std::chrono::steady_clock> mEmitterStart;

public:
    // ---------- Global functions --------------

    // Constructor
    ParticleEmitter(const std::string& _name, std::shared_ptr<Mesh> _visualMesh, glm::vec3 _pos = glm::vec3(0), glm::vec3 _scale = glm::vec3(1), glm::quat _rotation = glm::quat());

    ~ParticleEmitter() = default;

    // Overriden from IRender passes draw call to mesh. 
    void Draw(const std::shared_ptr<Shader> _shader = nullptr) const override;

    // Overriden from Actor, handles local tick logic
    void Update(float _dt) override;

    void UpdateLifeTime();

private:
    // ---------- Local functions --------------

    void ClearParticles();

    void RemoveParticles(std::shared_ptr<VisualActor> _particle);

    void StartParticleSystem();

    void StopParticleSystem();

public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters

    void SetSurfaceReference(std::shared_ptr<VisualActor> _inRef) { mSurfacereReference = _inRef; }

    // Getters


};