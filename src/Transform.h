#pragma once

// Includes 
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm> // For std::clamp

/**
 * @struct Transform,
 * @brief Transform contains all transform logic for an in world object.
 * It has position, rotation and scale in addition to all functions to move and change these values.
 */
struct Transform
{
private:
    // ----------- Local Variables ---------------

	glm::vec3 mPosition;
    glm::quat mRotation;
    glm::vec3 mScale;

public:
    // ----------- Global Functions ---------------

    // Constructor
	Transform()
        : mPosition(glm::vec3(0.0f)), mRotation(glm::quat(glm::identity<glm::quat>())), mScale(glm::vec3(1.0f)) {}

    // Adders ---------------

    // Adds rotation to the object from a quaternion input
    void AddRotation(const glm::quat& _rotation)
    {
        mRotation = mRotation * _rotation;
        mRotation = glm::normalize(mRotation);
    }

    // Adds yaw rotation based on in angle as degrees 
    void AddYaw(float _angleDegrees)
    {
        float angleRadians = glm::radians(_angleDegrees);
        glm::quat yawRotation = glm::angleAxis(angleRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        AddRotation(yawRotation);
    }

    // Adds pitch rotation based on in angle as degrees
    void AddPitch(float _angleDegrees)
    {
        float angleRadians = glm::radians(_angleDegrees);
        glm::vec3 rightVector = glm::rotate(mRotation, glm::vec3(1.0f, 0.0f, 0.0f));
    	glm::quat pitchRotation = glm::angleAxis(angleRadians,rightVector);
        AddRotation(pitchRotation);
    }

    // Setters ---------------

    // Sets local position to in position as vec3
    void SetPosition(const glm::vec3& _position) { mPosition = _position; }

    // Sets local rotation to in rotation as quaternion
    void SetRotation(const glm::quat& _rotation) { mRotation = _rotation; }

    // Sets local scale to in scale as vec3
    void SetScale(const glm::vec3& _scale) { mScale = _scale; }

    // Set rotation based in forward direction vec3 and up direction vec3 (default 0)
    void SetRotation(const glm::vec3& _forwardDirection, const glm::vec3& _upDirection = glm::vec3(0.0f,0.0f,0.0f))
    {
        glm::vec3 normalizeForward = glm::normalize(_forwardDirection);
        glm::vec3 defaultForward(0.0f, 0.0f, -1.0f);
        glm::quat newRotation = glm::rotation(defaultForward, normalizeForward);
        mRotation = newRotation;
    }

    // Sets rotation based on in pitch angle as degrees
	void SetRotationFromPitch(float _pitchDegrees)
    {
        float pitchRadians = glm::radians(_pitchDegrees);
        glm::quat pitchRotation = glm::angleAxis(pitchRadians, glm::vec3(1.0, 0.0f, 0.0f));
        mRotation = pitchRotation;
    }

    // Sets rotation based on in yaw angle as degrees
    void SetRotationFromYaw(float _yawDegrees)
    {
        float yawRadians = glm::radians(_yawDegrees);
        glm::quat yawRotation = glm::angleAxis(yawRadians, glm::vec3(1.0, 0.0f, 0.0f));
        mRotation = yawRotation;
    }

    // Sets position, rotation and scale based on a transformation matrix input
    void SetTransformMatrix(const glm::mat4& _transMatrix)
    {
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(_transMatrix, scale, rotation, translation, skew, perspective);

        SetPosition(translation);
        SetRotation(rotation);
        SetScale(scale);
    }

	// Getters ---------------

    // Returns local position
    const glm::vec3& GetPosition() const { return mPosition; }

    // Returns local rotation
    const glm::quat& GetRotation() const { return mRotation; }

    // Returns local scale
    const glm::vec3& GetScale() const { return mScale; }

    // Returns current pitch as degrees
    float GetCurrentPitchDegrees() const
    {
        glm::vec3 eulerAngles = glm::eulerAngles(mRotation);
        float pitch = eulerAngles.x;
        return glm::degrees(pitch);
    }

    // Returns current yaw as degrees
    float GetCurrentYawDegrees()
    {
        glm::vec3 eulerAngles = glm::eulerAngles(mRotation);
        float yaw = eulerAngles.y;
        return glm::degrees(yaw);
    }

    // Returns a transformation matrix based on the local scale, rotation and position values
    glm::mat4 GetTransformMatrix() const
    {
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), mScale);
        glm::mat4 rotateMat = glm::toMat4(mRotation);
        glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), mPosition);
        return translateMat * rotateMat * scaleMat;
    }

    // Gets the right direction of this object as vec3
    glm::vec3 GetRight() const
    {
        return glm::rotate(mRotation, glm::vec3(1.0f, 0.0f, 0.0f));
    }
};
