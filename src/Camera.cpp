
// Class includes
#include <Camera.h>

CameraActor::CameraActor(
    const std::string& name,
    glm::vec3 position,
    glm::quat rotation,
    float aspectRatio,
    float nearPlane,
    float farPlane,
    float fieldOfView,
    float maxMovementSpeed,
    float accelerationSpeed,
    float dampingFactor)
    : Actor(name),
    mMaxMovementSpeed(maxMovementSpeed),
    mAccelerationSpeed(accelerationSpeed),
    mDampingFactor(dampingFactor),
    mAspectRatio(aspectRatio),
    mNearPlane(nearPlane),
    mFarPlane(farPlane),
    mFieldOfView(fieldOfView)
{
    // sets teh position and rotation to constructed values 
    SetGlobalPosition(position);
    SetGlobalRotation(rotation);
    // Updates the projection matrix with new positional and rotational values
    UpdateProjectionMatrix();
}

void CameraActor::Update(float _dt)
{
    // Updates all forces acting on the camera each tick
    UpdateVelocity(_dt);
    UpdatePosition(_dt);
    UpdateDamping(_dt);
    UpdateAngularVelocity(_dt);
    UpdateRotation(_dt);
    UpdateAngularDamping(_dt);
}

void CameraActor::UpdateVelocity(float _dt)
{
    // Adds velocity based on acceleration
    mVelocity += mAcceleration * _dt;

    // if velocity is faster than max movement speed, limit to max movement speed
    if (glm::length(mVelocity) > mMaxMovementSpeed)
        mVelocity = glm::normalize(mVelocity) * mMaxMovementSpeed;
    
}

void CameraActor::UpdateDamping(float dt)
{
    // Reduces the acceleration based on itself * damping factor over time 
    float dampingFactor = glm::length(mVelocity) > 0 ? mDampingFactor : 0.0f;

    // reduces velocity over time (drag)
    mVelocity -= mVelocity * dampingFactor * dt;

    // if velocity is low, Set velocity to 0.
    if (glm::length(mVelocity) < 0.001f)
        mVelocity = glm::vec3(0.0f);
    
}

void CameraActor::UpdateAngularDamping(float dt)
{
    // same as regular damping but for angles.
    mAngularAcceleration = glm::vec2(0.0f);

    if (glm::length(mAngularVelocity) > 0)
    {
        mAngularVelocity -= mAngularVelocity * mAngularDampingFactor * dt;
        if (glm::length(mAngularVelocity) < 0.001f)
        {
            mAngularVelocity = glm::vec2(0.0f);
        }
    }
}

void CameraActor::UpdateAngularVelocity(float dt)
{
    mAngularVelocity += mAngularAcceleration * dt;
    // Clamp angular velocity to the maximum allowed
    mAngularVelocity = glm::clamp(mAngularVelocity, -mMaxAngularSpeed, mMaxAngularSpeed);
}

void CameraActor::UpdateRotation(float dt)
{
    if (mSnappedToActor)
    {
        AddYawDegrees(mAngularVelocity.x * dt, mSnappedToActor->GetGlobalPosition());
        AddPitchDegrees(mAngularVelocity.y * dt, mSnappedToActor->GetGlobalPosition());

    } else {
        // updates rotation each tick by the angular velocity
        AddYawDegrees(mAngularVelocity.x * dt);
        AddPitchDegrees(mAngularVelocity.y * dt);
    }
}

void CameraActor::UpdatePosition(float dt)
{
    // updates position by velocity
    glm::vec3 front = GetFront();
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::cross(right, front);

    SetGlobalPosition(GetLocalPosition() + mVelocity.x * dt * right + mVelocity.y * dt * up + mVelocity.z * dt * front);
}

void CameraActor::UpdateProjectionMatrix()
{
    // updates the projection matrix with new values
    mProjectionMatrix = glm::perspectiveRH(glm::radians(mFieldOfView), mAspectRatio, mNearPlane, mFarPlane);
}

void CameraActor::UpdateRotationFromYawPitch()
{
    // Updates rotation from yaw and pitch
    glm::quat pitchQuat = glm::angleAxis(glm::radians(mPitchDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat yawQuat = glm::angleAxis(glm::radians(mYawDegrees), glm::vec3(0.0f, 1.0f, 0.0f));

    // Combined rotation
    glm::quat newRotation = yawQuat * pitchQuat;
    newRotation = glm::normalize(newRotation); // Ensure the quaternion is normalized

    // Assuming SetRotation directly sets the Transform's rotation
    this->SetGlobalRotation(newRotation);
}

void CameraActor::UpdateRotationFromYawPitch(const glm::vec3& rotationCenter)
{
    // Get current position relative to rotation center
    glm::vec3 currentPosition = GetGlobalPosition();
    glm::vec3 offset = currentPosition - rotationCenter;

    // Calculate rotation based on yaw and pitch
    glm::quat pitchQuat = glm::angleAxis(glm::radians(mPitchDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat yawQuat = glm::angleAxis(glm::radians(mYawDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat newRotation = yawQuat * pitchQuat;
    newRotation = glm::normalize(newRotation);

    // Apply rotation to offset position
    glm::vec3 rotatedOffset = glm::rotate(newRotation, offset);

    // Calculate new position relative to rotation center
    glm::vec3 newPosition = rotationCenter + rotatedOffset;

    // Update global position and rotation
    SetGlobalPosition(newPosition);
    SetGlobalRotation(newRotation);

    // Ensure the camera looks at the rotation center
    LookAt(rotationCenter);
}

void CameraActor::LookAt(const glm::vec3& target)
{
    glm::vec3 direction = glm::normalize(target - GetGlobalPosition());
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Calculate the new rotation quaternion
    glm::quat lookRotation = glm::quatLookAt(direction, up);
    SetGlobalRotation(lookRotation);
}
