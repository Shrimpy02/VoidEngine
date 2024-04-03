// Classes
#include <Actor.h>
#include <Logger.h>

Actor::~Actor()
{
	// Remove all components and children from memory
	for (auto component : mComponents)
	{
		delete component;
		component = nullptr;
	}
	for (auto child : mChildren)
	{
		delete child;
		child = nullptr;
	}
}

void Actor::Update(float _dt)
{
}

void Actor::UpdateComponents(float _dt)
{
	for (auto component : mComponents)
		component->Update(_dt);
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

const glm::vec3& Actor::GetPosition(Actor::TransformSpace _type) const
{
	// gets the local position of the actor 
	if (_type == TransformSpace::Local)
		return mTransform.GetPosition();

	// gets the global position of the actor 
	else if (_type == TransformSpace::Global)
	{
		// iterates through each parent to get the global position
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

	// If nothing was found return default position.
	LOG_ERROR("Actor %s: returned null-position", mTag.GetValue().c_str());
	const glm::vec3 defaultPos = glm::vec3();

	return defaultPos;
}

const glm::quat& Actor::GetRotation(Actor::TransformSpace _type) const
{
	// gets the local rotation of the actor 
	if (_type == TransformSpace::Local)
		return mTransform.GetRotation();

	// gets the global rotation of the actor 
	else if (_type == TransformSpace::Global)
	{
		// iterates through each parent to get the global rotation
		glm::quat globalRotation = mTransform.GetRotation();
		const Actor* current = this->mParent;
		while (current != nullptr)
		{
			globalRotation = current->mTransform.GetRotation() * globalRotation;
			current = current->mParent;
		}
		return globalRotation;
	}

	// If nothing was found return default rotation.
	LOG_ERROR("Actor %s: returned null-rotation", mTag.GetValue().c_str());
	const glm::quat defaultRot = glm::quat();

	return defaultRot;
}

const glm::vec3& Actor::GetScale(Actor::TransformSpace _type) const
{
	// gets the local scale of the actor 
	if (_type == TransformSpace::Local)
		return mTransform.GetScale();

	// gets the global scale of the actor 
	else if (_type == TransformSpace::Global)
	{
		// iterates through each parent to get the global scale
		glm::vec3 globalScale = mTransform.GetScale();
		const Actor* current = this->mParent;
		while (current != nullptr)
		{
			globalScale *= current->mTransform.GetScale();
			current = current->mParent;
		}
		return globalScale;
	}

	// If nothing was found return default scale.
	LOG_ERROR("Actor %s: returned null-scale", mTag.GetValue().c_str());
	const glm::vec3 defaultScale = glm::vec3(1.f);

	return defaultScale;
}

const Transform& Actor::GetTransform() const
{
	return mTransform;
}

const glm::mat4 Actor::GetTransformMatrix(Actor::TransformSpace _type) const
{
	// gets the local Transform Matrix of the actor  
	if (_type == TransformSpace::Local)
		return mTransform.GetTransformMatrix();

	// gets the global Transform Matrix of the actor 
	else if (_type == TransformSpace::Global)
	{
		// iterates through each parent to get the global Transform Matrix
		glm::mat4 globalTransform = mTransform.GetTransformMatrix();
		const Actor* current = this;
		while (current->mParent != nullptr)
		{
			globalTransform = current->mParent->GetTransformMatrix(TransformSpace::Local) * globalTransform;
			current = current->mParent;
		}
		return globalTransform;
	}

	// If nothing was found return default Transform Matrix.
	LOG_ERROR("Actor %s: returned null-Transform Matrix", mTag.GetValue().c_str());;
	const glm::mat4 defaultTransMatrix = glm::mat4();

	return defaultTransMatrix;
}

glm::vec3 Actor::GetRight() const
{
	return mTransform.GetRight();
}

std::vector<Actor*>& Actor::GetChildren()
{
	return mChildren;
}

void Actor::SetLocalTransformMatrix(const glm::mat4& _transformMatrix)
{
	mTransform.SetTransformMatrix(_transformMatrix);
}

void Actor::SetTransform(const Transform& _transform)
{
	mTransform = _transform;
}

void Actor::SetPosition(const glm::vec3& _position, Actor::TransformSpace _type)
{
	// if the transform space is local, set the local position
	if(_type == TransformSpace::Local)
		mTransform.SetPosition(_position);

	// other wise if it`s global, set the global position
	else if(_type == TransformSpace::Global)
	{
		if (mParent)
		{
			// Sets the global position of this actor by setting its position relative to its parents position;
			glm::mat4 parentGlobalInverse = glm::inverse(mParent->GetTransformMatrix(TransformSpace::Global));
			glm::vec4 localPosition = parentGlobalInverse * glm::vec4(_position, 1.0f);
			mTransform.SetPosition(glm::vec3(localPosition));
		}
		// if there is no parent, set local position.
		else
			mTransform.SetPosition(_position);
			
	}
}

void Actor::SetRotation(const glm::quat& _rotation, Actor::TransformSpace _type)
{
	// if the transform space is local, set the local rotation
	if (_type == TransformSpace::Local)
		mTransform.SetRotation(_rotation);

	// other wise if it`s global, set the global rotation
	else if (_type == TransformSpace::Global)
	{
		if (mParent)
		{
			// Sets the global rotation of this actor by setting its rotation relative to its parents rotation;
			glm::quat parentGlobalRotationInverse = glm::inverse(glm::quat_cast(mParent->GetTransformMatrix(TransformSpace::Global)));
			glm::quat localRotation = parentGlobalRotationInverse * _rotation;
			mTransform.SetRotation(localRotation);
		}
		// if there is no parent, set local rotation
		else
			mTransform.SetRotation(_rotation);
		
	}
}

void Actor::SetScale(const glm::vec3& _scale, Actor::TransformSpace _type)
{
	// if the transform space is local, set the local scale
	if (_type == TransformSpace::Local)
		mTransform.SetScale(_scale);

	// other wise if it`s global, set the global rotation
	else if (_type == TransformSpace::Global)
	{
		if (mParent)
		{
			// Sets the global scale of this actor by setting its scale relative to its parents scale;
			glm::vec3 parentGlobalScale = mParent->GetScale(TransformSpace::Global);
			glm::vec3 relativeScale = _scale / parentGlobalScale;
			mTransform.SetScale(relativeScale);
		}
		// if there is no parent, set local scale
		else
			mTransform.SetScale(_scale);
		
	}
}

void Actor::SetParent(Actor* _parent)
{
	// Removes this actor from parents children array
	if (mParent)
		mParent->RemoveChild(this);

	// Sets new parent
	mParent = _parent;

	// Adds this actor to new parents children array
	if (mParent)
		mParent->AddChild(this);
}