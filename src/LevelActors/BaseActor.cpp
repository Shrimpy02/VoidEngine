
// Includes
#include <LevelActors/BaseActor.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/Texture.h>
#include <RenderElements/Material.h>
#include <Utilities/Logger.h>
#include <Lua/LuaReader.h>


#include "GraphActor.h"
#include "Utilities/Defines.h"

// Additional Includes

BaseActor::BaseActor(const std::string& _name, std::shared_ptr<Mesh> _visualMesh, CollisionBase _collisionBase, glm::vec3 _pos, glm::vec3 _scale, glm::quat _rotation, std::shared_ptr<Mesh> _collisionMesh)
    :Actor(_name)
{
    SetGlobalPosition(_pos);
    SetGlobalScale(_scale);
    SetGlobalRotation(_rotation);

    if (_visualMesh) {

        mVisualMesh = _visualMesh;
        if (_collisionMesh)
        {
            mCollisionMesh = _collisionMesh;
            mCustomCollisionMesh = true;
        }
        else {
            if(_collisionBase != CollisionBase::Compare)
				SetCollisionBase(_collisionBase);
            UpdateCollisionMeshBasedOnCollisionBase();
            UpdateExtent();
        }
    }
    else {
        LOG_WARNING("BaseActor `%s` has no visible mesh", _name.c_str());
    }

    InitializeLifeTime();

    //mCollisionMesh->SetIsVisible(false);

    LOG("BaseActor Created: %s", _name.c_str());
}

void BaseActor::Draw(const std::shared_ptr<Shader> _shader) const
{
    // Draw visual mesh first
    if (!mVisualMesh) return;
    mVisualMesh->Draw(_shader);

    // Then collision mesh
    if (!mCollisionMesh) return;

    if (!visualizeCollisionMesh) return;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    mCollisionMesh->Draw(_shader);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void BaseActor::Update(float _dt)
{
    // temp
    if(shouldMove)
    {
        glm::vec3 currentPos = GetGlobalPosition();
        glm::vec3 newPos = currentPos + direction * _dt * bulletSpeed;
        SetGlobalPosition(newPos);
    }
    // temp

    UpdateCollisionMeshBasedOnCollisionBase();

    UpdateExtent();

    if(luaControlPosition){ // lua showcase for updating position

        // Init Lua Interpreter
        lua_State* interp = SLuaReader::InitLuaInterpreter();

        // Create actor using lua data
        std::string luaPostiion = SOURCE_DIRECTORY("src/Lua/LuaSrc/SetPosition.lua");
        glm::vec3 position = SLuaReader::Lua_SetActorPosition(luaPostiion, interp);

        SetGlobalPosition(position);
    }
}

void BaseActor::SetMinMaxExtent()
{
    std::pair<glm::vec3, glm::vec3> extents = mVisualMesh->GetMeshMinMaxExtent();

    mMinExtent = extents.first;
    mMaxExtent = extents.second;
}

void BaseActor::UpdateExtent()
{
    if (mCollisionProperties.IsAABB())
    {

        glm::vec3 extent = (mMaxExtent - mMinExtent) * glm::vec3(0.5);

        extent *= GetGlobalScale();

        glm::quat actorRotation = GetGlobalRotation();

        // Rotate the total extent by the actor's rotation for each axis
        glm::vec3 xAxis = actorRotation * glm::vec3(extent.x, 0.0f, 0.0f);
        glm::vec3 yAxis = actorRotation * glm::vec3(0.0f, extent.y, 0.0f);
        glm::vec3 zAxis = actorRotation * glm::vec3(0.0f, 0.0f, extent.z);

        // Calculate new rotated extent
        glm::vec3 rotatedExtents(
            glm::abs(xAxis.x) + glm::abs(yAxis.x) + glm::abs(zAxis.x),
            glm::abs(xAxis.y) + glm::abs(yAxis.y) + glm::abs(zAxis.y),
            glm::abs(xAxis.z) + glm::abs(yAxis.z) + glm::abs(zAxis.z));

        mExtent = rotatedExtents;
        mCenter = ((mMinExtent + mMaxExtent) * 0.5f) + GetGlobalPosition();

    }
    else if (mCollisionProperties.IsBoundingSphere()) {

        mRadius = glm::length(((mMaxExtent - mMinExtent) * glm::vec3(0.5)) * GetGlobalScale());
        mRadius /= 2;
        mRadius += 0.05f * glm::length(GetGlobalScale());
    	mCenter = ((mMinExtent + mMaxExtent) * 0.5f) + GetGlobalPosition();
    }

}

void BaseActor::UpdateCollisionMeshBasedOnCollisionBase()
{
    if (mCollisionProperties.mBase != mCollisionProperties.mBaseCompare && !mCustomCollisionMesh)
    {
        std::shared_ptr<Texture> debugTex = Texture::LoadWhiteTexture();
        std::shared_ptr<Material> debugMat = Material::Load("Debug", { debugTex }, { {glm::vec3(1.0f,0.0f,0.0f)}, {64} });
        std::string collisionName = GetTag();

        if (mCollisionProperties.IsAABB()) {

            collisionName.append("-CollisionCubeAABB");
            mCollisionMesh = Mesh::CreateCubeByExtent(mVisualMesh, debugMat, collisionName);
        	SetMinMaxExtent();
        }
        else if (mCollisionProperties.IsBoundingSphere()) {

            collisionName.append("-CollisionSphere");
            mCollisionMesh = Mesh::CreateSphereByExtent(mVisualMesh, debugMat, collisionName);
            SetMinMaxExtent();
        }
        mCollisionProperties.mBaseCompare = mCollisionProperties.mBase;
    }
    else if (mCustomCollisionMesh && doOnce) {
        SetMinMaxExtent();
        doOnce = false;
    }
}