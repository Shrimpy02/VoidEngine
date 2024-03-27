#include "Actor.h"

#include <iostream>

void Actor::Update(float dt)
{
}

void Actor::AddChild(Actor* _child)
{
	// Adds child to this actors children array
	// and sets this actor as its parent.
	if(_child)
	{
		mChildren.push_back(_child);
		_child->mParent = this;
	}
}

void Actor::RemoveChild(Actor* _child)
{
	// Iterates through child vector and removes the child that matches input.
	auto it = std::find(mChildren.begin(),mChildren.end(), _child);
	if(it != mChildren.end())
	{
		mChildren.erase(it);
		// If this actor is the child`s parent, remove parent reference.
		if(_child->mParent == this)
		{
			_child->mParent = nullptr;
		}
	}
}

void Actor::SetParent(Actor* _parent)
{
	// Removes this actor from parents children array
	if(mParent)
		mParent->RemoveChild(this);

	// Sets new parent
	mParent = _parent;

	// Adds this actor to new parents children array
	if(mParent)
		mParent->AddChild(this);
}

void Actor::SetTransform(const Transform& _transform)
{
	mTransform = _transform;
}

void Actor::SetPosition(const glm::vec3& _position, Actor::TransformSpace _type)
{
	if(_type == TransformSpace::Local)
		mTransform.SetPosition(_position);
	
	else if(_type == TransformSpace::Global)
	{
		if (mParent)
		{
			// Sets the global position of this actor by setting its position relative to its parents position;
			glm::mat4 parentGlobalInverse = glm::inverse(mParent->GetTransformMatrix(TransformSpace::Global));
			glm::vec4 localPosition = parentGlobalInverse * glm::vec4(_position, 1.0f);
			mTransform.SetPosition(glm::vec3(localPosition));
		}
		else
			mTransform.SetPosition(_position);
	}
}

void Actor::SetRotation(const glm::quat& _rotation, Actor::TransformSpace type)
{
	if (type == TransformSpace::Local)
		mTransform.SetRotation(_rotation);
	
	else if (type == TransformSpace::Global)
	{
		if (mParent)
		{
			glm::quat parentGlobalRotationInverse = glm::inverse(glm::quat_cast(mParent->GetTransformMatrix(TransformSpace::Global)));
			glm::quat localRotation = parentGlobalRotationInverse * _rotation;
			mTransform.SetRotation(localRotation);
		}
		else
			mTransform.SetRotation(_rotation);
		
	}
}

void Actor::SetScale(const glm::vec3& _scale, Actor::TransformSpace type)
{
	if (type == TransformSpace::Local)
		mTransform.SetScale(_scale);
	
	else if (type == TransformSpace::Global)
	{
		if (mParent)
		{
			glm::vec3 parentGlobalScale = mParent->GetScale(TransformSpace::Global);
			glm::vec3 relativeScale = _scale / parentGlobalScale;
			mTransform.SetScale(relativeScale);
		}
		else
			mTransform.SetScale(_scale);
		
	}
}

const glm::vec3& Actor::GetPosition(Actor::TransformSpace _type) const
{
	if(_type == TransformSpace::Local)
		return mTransform.GetPosition();
		
	else if(_type == TransformSpace::Global)
	{
		glm::vec3 globalPosition = mTransform.GetPosition();
		const Actor* current = this->mParent;
		while (current != nullptr)
		{
			globalPosition = glm::rotate(current->mTransform.GetRotation(), globalPosition);
			globalPosition += current->mTransform.GetPosition();
			current = current->mParent;
		}
		return globalPosition;
	}

	std::cout << "NO POSITION RETURNED\n";
	const glm::vec3 defaultPos = glm::vec3();

	return defaultPos;
}

const glm::quat& Actor::GetRotation(Actor::TransformSpace _type) const
{
	if (_type == TransformSpace::Local)
		return mTransform.GetRotation();

	else if (_type == TransformSpace::Global)
	{
		glm::quat globalRotation = mTransform.GetRotation();
		const Actor* current = this->mParent;
		while (current != nullptr)
		{
			globalRotation = current->mTransform.GetRotation() * globalRotation;
			current = current->mParent;
		}
		return globalRotation;
	}

	std::cout << "NO ROTATION RETURNED\n";
	const glm::quat defaultRot = glm::quat();

	return defaultRot;
}

const glm::vec3& Actor::GetScale(Actor::TransformSpace _type) const
{
	if (_type == TransformSpace::Local)
		return mTransform.GetScale();

	else if (_type == TransformSpace::Global)
	{
		glm::vec3 globalScale = mTransform.GetScale();
		const Actor* current = this->mParent;
		while (current != nullptr)
		{
			globalScale *= current->mTransform.GetScale();
			current = current->mParent;
		}
		return globalScale;
	}

	std::cout << "NO SCALE RETURNED\n";
	const glm::vec3 defaultScale = glm::vec3();

	return defaultScale;
}

const Transform& Actor::GetTransform() const
{
	return mTransform;
}

const glm::mat4 Actor::GetTransformMatrix(Actor::TransformSpace _type) const
{
	if (_type == TransformSpace::Local)
		return mTransform.GetTransformMatrix();

	else if (_type == TransformSpace::Global)
	{
		glm::mat4 globalTransform = mTransform.GetTransformMatrix();
		const Actor* current = this;
		while (current->mParent != nullptr)
		{
			globalTransform = current->mParent->GetTransformMatrix(TransformSpace::Local) * globalTransform;
			current = current->mParent;
		}
		return globalTransform;
	}

	std::cout << "NO TRANSFORM-MATRIX RETURNED\n";
	const glm::mat4 defaultTransMatrix = glm::mat4();

	return defaultTransMatrix;
}

std::vector<Actor*>& Actor::GetChildren()
{
	return mChildren;
}

glm::vec3 Actor::GetRight() const
{
	return mTransform.GetRight();
}
