#pragma once

// Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Actor.h>

/**
 * @class CameraActor,
 * @brief Camera actor class inherits from actor for world location.
 * It controls how the player sees the world and flying camera movement.
 */
class CameraActor : public Actor
{
public:
	// ---------- Global Variables --------------


private:
	// ---------- Local Variables --------------

    // Settings
    float mMaxMovementSpeed;
    float mAccelerationSpeed;
    float mDampingFactor;

    float mAspectRatio;
    float mNearPlane;
    float mFarPlane;
    float mFieldOfView;

    // Acceleration
    float mAngularAccelerationSpeed = 1000.f;
    float mMaxAngularSpeed = 500.f;
    float mAngularDampingFactor = 20.0f;

    // Movement
    glm::vec3 mVelocity{ 0.f, 0.f, 0.f };
    glm::vec3 mAcceleration{ 0.f, 0.f, 0.f };
    glm::vec2 mAngularVelocity{ 0.f, 0.f };
    glm::vec2 mAngularAcceleration{ 0.f, 0.f };

    // Rotation
    float mYawDegrees = 0.0f;
    float mPitchDegrees = 0.0f;

    // Projection matrix
    glm::mat4 mProjectionMatrix;

public:
	// ---------- Global functions --------------

    // Constructor
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

    // Inherited update function from Actor, updates all local functions
    void Update(float _dt) override;

private:
	// ---------- Local functions --------------

    // Updates the velocity of the camera object
    void UpdateVelocity(float _dt);
    // Updates the position of the camera based on its velocity
    void UpdatePosition(float _dt);
    // Updates the rotation of the camera based on its angular velocity 
    void UpdateRotation(float _dt);
    // Updates the angular velocity
    void UpdateAngularVelocity(float _dt);
    // Update the angular damping
    void UpdateAngularDamping(float _dt);
    // Update movement damping
    void UpdateDamping(float _dt);

    // Updates the projection matrix with current rotation and position
    void UpdateProjectionMatrix();

    // Updates the rotation of the camera by its yaw and pitch.
    void UpdateRotationFromYawPitch();

public:
	// ---------- Getters / setters / Adders --------------


    // Getters

    // Returns the angular velocity as a vec2
    const glm::vec2& GetAngularVelocity() const { return mAngularAcceleration; }
    // Returns the angular acceleration as a vec2
    const glm::vec2& GetAngularAcceleration() const { return mAngularAcceleration; }
    // Returns the angular damping factor as a float
    float GetAngularDampingFactor() const { return mAngularDampingFactor; }
    // gets teh angular acceleration speed as a float
    float GetAngularAccelerationSpeed() const { return mAngularAccelerationSpeed; }

    // Returns the max movement speed of the camera as a float
    float GetMaxMovementSpeed() const { return mMaxMovementSpeed; }
    // Returns the aspect ration of the window view port as a float
    float GetAspectRatio() const { return mAspectRatio; }
    // Returns the near plane as a float 
    float GetNearPlane() const { return mNearPlane; }
    // returns the far plane as a float
    float GetFarPlane() const { return mFarPlane; }
    // Returns the Field of view as a float
    float GetFieldOfView() const { return mFieldOfView; }
    // Returns the acceleration speed as a float
    float GetAccelerationSpeed() const { return mAccelerationSpeed; }
    // Returns teh acceleration as aa vec3
    const glm::vec3& GetAcceleration() const { return mAcceleration; }

    // Returns the front of the camera as a vec3
    glm::vec3 GetFront() const { return glm::rotate(GetLocalRotation(), glm::vec3(0.0f, 0.0f, -1.0f)); };
    // Returns the cameras up vector as a vec3
	glm::vec3 GetUp() const { return glm::rotate(GetLocalRotation(), glm::vec3(0.0f, 1.0f, 0.0f)); }
    // Returns the velocity of the camera as a vec3
	const glm::vec3& GetVelocity() const { return mVelocity; }
    // returns the view matrix of this camera as a mat4
    glm::mat4 GetViewMatrix() const { return glm::lookAt(GetLocalPosition(), GetLocalPosition() + GetFront(), glm::vec3(0.0f, 1.0f, 0.0f)); };
    // Returns teh projection matrix as a mat4
	const glm::mat4& GetProjectionMatrix() const { return mProjectionMatrix; }
    // returns the VP matrix asa a mat4
	glm::mat4 GetVPMatrix() const { return mProjectionMatrix * GetViewMatrix(); }


    // Adders

    // Adds velocity to camera
    void AddVelocity(const glm::vec3& _velocity) { mVelocity += _velocity; }

    // adds rotation in yaw axis from input degrees
    void AddYawDegrees(float _angleDegrees){    mYawDegrees += _angleDegrees;
    											mYawDegrees = std::fmod(mYawDegrees, 360.0f);
    											UpdateRotationFromYawPitch();}
    // adds rotation in pitch axis from input degrees
    void AddPitchDegrees(float _angleDegrees) {  // Calculate new pitch and clamp it to prevent flipping
    											float newPitch = glm::clamp(mPitchDegrees + _angleDegrees, -89.0f, 89.0f);
    											mPitchDegrees = newPitch;
    											UpdateRotationFromYawPitch();}
    // adds angular velocity
    void AddAngularVelocity(const glm::vec2& _angularVelocityDelta) { mAngularVelocity += _angularVelocityDelta; }
    // adds angular acceleration
	void AddAngularAcceleration(const glm::vec2& _angularAccelerationDelta) { mAngularAcceleration += _angularAccelerationDelta; }

    // Setters

    // Sets the windows aspect ratio
    void SetAspectRatio(float _aspectRatio){ mAspectRatio = _aspectRatio;
    										UpdateProjectionMatrix(); }
    // sets the cameras velocity from input 
    void SetVelocity(const glm::vec3& _velocity) { mVelocity = _velocity;}
    // Sets the cameras acceleration speed from input
    void SetAccelerationSpeed(float _accelerationSpeed) { mAccelerationSpeed = _accelerationSpeed; }
    // Set camera acceleration
    void SetAcceleration(const glm::vec3& _acceleration) { mAcceleration = _acceleration; }
    // Set max movement speed camera can reach
    void SetMaxMovementSpeed(float _movementSpeed) { mMaxMovementSpeed = _movementSpeed; }
    // Set the angular velocity of the camera
    void SetAngularVelocity(const glm::vec2& _angularVelocity) { mAngularVelocity = _angularVelocity; }
    // Sets the angular acceleration
	void SetAngularAcceleration(const glm::vec2& _angularAcceleration) { mAngularAcceleration = _angularAcceleration; }
    // Sets the angular damping factor
	void SetAngularDampingFactor(float _dampingFactor) { mAngularDampingFactor = _dampingFactor; }


};