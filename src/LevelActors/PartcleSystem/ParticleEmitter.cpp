
// Includes
#include <LevelActors/PartcleSystem/ParticleEmitter.h>
#include <LevelActors/VisualActor.h>
#include <Core/Shader.h>
#include <Core/SSpawner.h>

// Additional Includes

ParticleEmitter::ParticleEmitter(const std::string& _name, std::shared_ptr<Mesh> _visualMesh, glm::vec3 _pos, glm::vec3 _scale, glm::quat _rotation)
    :Actor(_name), mParticleMesh(_visualMesh)
{
    SetGlobalPosition(_pos);
    SetGlobalScale(_scale);
    SetGlobalRotation(_rotation);

    mEmitterStart = std::chrono::high_resolution_clock::now();

    InitializeLifeTime();
}

void ParticleEmitter::Draw(const std::shared_ptr<Shader> _shader) const
{
    for(std::shared_ptr<VisualActor> particle : mParticles)
    {
        _shader->use();
        _shader->setMat4("model", particle->GetGlobalTransformMatrix());
        particle->Draw(_shader);
    }
}

void ParticleEmitter::Update(float _dt)
{
    for (std::shared_ptr<VisualActor> particle : mParticles)
    {
        particle->Update(_dt);
        particle->UpdateComponents(_dt);
    }
}

void ParticleEmitter::UpdateLifeTime()
{
    std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::high_resolution_clock::now();
    double elapsedTimeMsSinceStart = std::chrono::duration<double, std::milli>(currentTime - mEmitterStart).count();
    elapsedTimeMsSinceStart /= 1000;

    if ((elapsedTimeMsSinceStart) >= (mEmitterDuration))
        StopParticleSystem();

    else if(mEmitterActive && mParticles.size() <= mNumParticles)
    {
        // Uniform distribution
        double emitterDelay = mEmitterDuration / static_cast<double>(mNumParticles);

        // if time since start is greater than delay * num particles, spawn new one
    	if ((elapsedTimeMsSinceStart) >= (emitterDelay * mParticles.size()))
        {
            std::shared_ptr<VisualActor> newParticle = std::make_shared<VisualActor>("snowParticle",
																							mParticleMesh, 
																						GetGlobalPosition(), 
																						glm::vec3(0.1f));
            mParticles.push_back(newParticle);
            newParticle->SetGlobalScale(glm::vec3(0.1f));
            newParticle->AddComponent<PhysicsComponent>("PhysicsComponent");
            newParticle->GetPhysicsComponent()->SetPhysicsPreset(PhysicsPreset::PP_Snow);
            newParticle->GetPhysicsComponent()->SetGravity(mParticleGravity);
            newParticle->GetPhysicsComponent()->SetSurfaceReference(mSurfacereReference);
            SSpawner::SetObjectLocationWithinInSphere(newParticle,GetGlobalPosition(), mEmitterSpawnRange);
        }
    }
}

void ParticleEmitter::ClearParticles()
{
    mParticles.clear();
}

void ParticleEmitter::RemoveParticles(std::shared_ptr<VisualActor> _particle)
{
	auto it = std::find(mParticles.begin(), mParticles.end(), _particle);
	if (it != mParticles.end())
	{
		mParticles.erase(it);
	}
}

void ParticleEmitter::StartParticleSystem()
{
    ReInitializeLifeTime();
    mEmitterActive = true;
}

void ParticleEmitter::StopParticleSystem()
{
    mEmitterActive = false;
    ClearParticles();
}
