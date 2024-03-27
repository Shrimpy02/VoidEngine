#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Actor.h>

class CameraActor : public Actor
{
public:
	// ---------- Global Variables --------------


private:
	// ---------- Local Variables --------------

    float mMaxMovementSpeed;
    float mAccelerationSpeed;
    float mDampingFactor;

    float mAngularAccelerationSpeed = 1000.f;
    float mMaxAngularSpeed = 500.f;
    float mAngularDampingFactor = 20.0f;

    glm::vec3 mVelocity{ 0.f, 0.f, 0.f };
    glm::vec3 mAcceleration{ 0.f, 0.f, 0.f };
    glm::vec2 mAngularVelocity{ 0.f, 0.f };
    glm::vec2 mAngularAcceleration{ 0.f, 0.f };

    float mAspectRatio;
    float mNearPlane;
    float mFarPlane;
    float mFieldOfView;

    float mYawDegrees = 0.0f;
    float mPitchDegrees = 0.0f;

    glm::mat4 mProjectionMatrix;

public:
	// ---------- Global functions --------------

	CameraActor(const std::string& _name,
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::quat rotation = glm::quat(glm::identity<glm::quat>()),
        float aspectRatio = 1280.f / 720.f,
        float nearPlane = 0.1f,
        float farPlane = 100.0f,
        float fieldOfView = 45.0f,
        float maxMovementSeed = 100.f,
        float accelerationSpeed = 50.0f,
        float dampingFactor = 5.0f);

    virtual void Update(float _dt) override;

private:
	// ---------- Local functions --------------

    void UpdateVelocity(float _dt);
    void UpdatePosition(float _dt);
    void UpdateRotation(float _dt);
    void UpdateAngularVelocity(float _dt);
    void UpdateAngularDamping(float _dt);
    void UpdateDamping(float _dt);

    void UpdateProjectionMatrix();
    void UpdateRotationFromYawPitch();

public:
	// ---------- Getters / setters / Adders --------------

    // Adders
    void AddVelocity(const glm::vec3& _velocity) { mVelocity += _velocity; }
    void AddYawDegrees(float _angleDegrees){    mYawDegrees += _angleDegrees;
    											mYawDegrees = std::fmod(mYawDegrees, 360.0f);
    											UpdateRotationFromYawPitch();}

    void AddPitchDegrees(float _angleDegrees) {  // Calculate new pitch and clamp it to prevent flipping
    											float newPitch = glm::clamp(mPitchDegrees + _angleDegrees, -89.0f, 89.0f);
    											mPitchDegrees = newPitch;
    											UpdateRotationFromYawPitch();}

    void AddAngularVelocity(const glm::vec2& _angularVelocityDelta) { mAngularVelocity += _angularVelocityDelta; }
    void AddAngularAcceleration(const glm::vec2& _angularAccelerationDelta) { mAngularAcceleration += _angularAccelerationDelta; }

    // Setters
    void SetAspectRatio(float _aspectRatio){ mAspectRatio = _aspectRatio;
    										UpdateProjectionMatrix(); }

    void SetVelocity(const glm::vec3& _velocity) { mVelocity = _velocity;}
    void SetAccelerationSpeed(float _accelerationSpeed) { mAccelerationSpeed = _accelerationSpeed; }
    void SetAcceleration(const glm::vec3& _acceleration) { mAcceleration = _acceleration; }
    void SetMaxMovementSpeed(float _movementSpeed) { mMaxMovementSpeed = _movementSpeed; }

    void SetAngularVelocity(const glm::vec2& _angularVelocity) { mAngularVelocity = _angularVelocity; }
    void SetAngularAcceleration(const glm::vec2& _angularAcceleration) { mAngularAcceleration = _angularAcceleration; }
    void SetAngularDampingFactor(float _dampingFactor) { mAngularDampingFactor = _dampingFactor; }

    // Getters
    const glm::vec2& GetAngularVelocity() const { return mAngularAcceleration; }
    const glm::vec2& GetAngularAcceleration() const { return mAngularAcceleration; }
    float GetAngularDampingFactor() const { return mAngularDampingFactor; }

    float GetMaxMovementSpeed() const { return mMaxMovementSpeed; }
    float GetAspectRatio() const { return mAspectRatio; }
    float GetNearPlane() const { return mNearPlane; }
    float GetFarPlane() const { return mFarPlane; }
    float GetFieldOfView() const { return mFieldOfView; }
    float GetAccelerationSpeed() const { return mAccelerationSpeed; }
    const glm::vec3& GetAcceleration() const { return mAcceleration; }

    glm::vec3 GetFront() const { return glm::rotate(GetRotation(TransformSpace::Local), glm::vec3(0.0f, 0.0f, -1.0f)); };
    glm::vec3 GetUp() const { return glm::rotate(GetRotation(TransformSpace::Local), glm::vec3(0.0f, 1.0f, 0.0f)); }
    const glm::vec3& GetVelocity() const { return mVelocity; }
    glm::mat4 GetViewMatrix() const { return glm::lookAt(GetPosition(TransformSpace::Local), GetPosition(TransformSpace::Local) + GetFront(), glm::vec3(0.0f, 1.0f, 0.0f)); };
    const glm::mat4& GetProjectionMatrix() const { return mProjectionMatrix; }
    glm::mat4 GetVPMatrix() const { return mProjectionMatrix * GetViewMatrix(); }
    float GetAngularAccelerationSpeed() const { return mAngularAccelerationSpeed; }

};