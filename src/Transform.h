#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm> // For std::clamp

struct Transform
{
private:
    // ----------- Variables ---------------
	glm::vec3 mPosition;
    glm::quat mRotation;
    glm::vec3 mScale;


public:
    // ----------- Functions ---------------
    Transform()
        : mPosition(glm::vec3(0.0f)), mRotation(glm::quat(glm::identity<glm::quat>())), mScale(glm::vec3(1.0f)) {}

    // Adders ---------------

    void AddRotation(const glm::quat& _rotation)
    {
        mRotation = mRotation * _rotation;
        mRotation = glm::normalize(mRotation);
    }

    void AddYaw(float _angleDegrees)
    {
        float angleRadians = glm::radians(_angleDegrees);
        glm::quat yawRotation = glm::angleAxis(angleRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        AddRotation(yawRotation);
    }

    void AddPitch(float _angleDegrees)
    {
        float angleRadians = glm::radians(_angleDegrees);
        glm::vec3 rightVector = glm::rotate(mRotation, glm::vec3(1.0f, 0.0f, 0.0f));
    	glm::quat pitchRotation = glm::angleAxis(angleRadians,rightVector);
        AddRotation(pitchRotation);
    }

    // Setters ---------------

    void SetPosition(const glm::vec3& _position) { mPosition = _position; }
    void SetRotation(const glm::quat& _rotation) { mRotation = _rotation; }
    void SetScale(const glm::vec3& _scale) { mScale = _scale; }

    void SetRotation(const glm::vec3& _forwardDirection, const glm::vec3& _upDirection = glm::vec3(0.0f,0.0f,0.0f))
    {
        glm::vec3 normalizeForward = glm::normalize(_forwardDirection);
        glm::vec3 defaultForward(0.0f, 0.0f, -1.0f);
        glm::quat newRotation = glm::rotation(defaultForward, normalizeForward);
        mRotation = newRotation;
    }

	void SetRotationFromPitch(float _pitchDegrees)
    {
        float pitchRadians = glm::radians(_pitchDegrees);
        glm::quat pitchRotation = glm::angleAxis(pitchRadians, glm::vec3(1.0, 0.0f, 0.0f));
        mRotation = pitchRotation;
    }

    void SetRotationFromYaw(float _yawDegrees)
    {
        float yawRadians = glm::radians(_yawDegrees);
        glm::quat yawRotation = glm::angleAxis(yawRadians, glm::vec3(1.0, 0.0f, 0.0f));
        mRotation = yawRotation;
    }

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
    const glm::vec3& GetPosition() const { return mPosition; }
    const glm::quat& GetRotation() const { return mRotation; }
    const glm::vec3& GetScale() const { return mScale; }

    float GetCurrentPitchDegrees() const
    {
        glm::vec3 eulerAngles = glm::eulerAngles(mRotation);
        float pitch = eulerAngles.x;
        return glm::degrees(pitch);
    }

    float GetCurrentYawDegrees()
    {
        glm::vec3 eulerAngles = glm::eulerAngles(mRotation);
        float yaw = eulerAngles.y;
        return glm::degrees(yaw);
    }

    glm::mat4 GetTransformMatrix() const
    {
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), mScale);
        glm::mat4 rotateMat = glm::toMat4(mRotation);
        glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), mPosition);
        return translateMat * rotateMat * scaleMat;
    }

    glm::vec3 GetRight() const
    {
        return glm::rotate(mRotation, glm::vec3(1.0f, 0.0f, 0.0f));
    }
};
