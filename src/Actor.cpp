// Classes
#include <Actor.h>
#include <Utilities/Logger.h>

Actor::~Actor()
{
	// Remove all components and children from memory
	for (auto component : mComponents)
	{
		component = nullptr;
	}
	for (auto child : mChildren)
	{
		child = nullptr;
	}
}

void Actor::Update(float _dt)
{
}

void Actor::UpdateComponents(float _dt)
{
	// calls update for all components 
	for (std::shared_ptr<Component> component : mComponents)
		component->Update(_dt);
}

void Actor::UpdateExtentByRotation(glm::vec3& _extent)
{

}

void Actor::AddChild(std::shared_ptr<Actor> _child)
{
	// Adds child to this actors children array
	// and sets this actor as its parent.
	if(_child)
	{
		mChildren.push_back(_child);
		_child->mParent = shared_from_this();
	}
}

void Actor::RemoveChild(std::shared_ptr<Actor> _child)
{
	// Iterates through child vector and removes the child that matches input.
	auto it = std::find(mChildren.begin(),mChildren.end(), _child);
	if(it != mChildren.end())
	{
		mChildren.erase(it);
		// If "this" is the child`s parent, remove parent reference.
		if(_child->mParent == shared_from_this())
		{
			_child->mParent = nullptr;
		}
	}
}

std::shared_ptr<PhysicsComponent> Actor::GetPhysicsComponent()
{
	for(std::shared_ptr<Component> comps : mComponents)
	{
		if (std::shared_ptr<PhysicsComponent> physicsComp = std::dynamic_pointer_cast<PhysicsComponent>(comps))
			return physicsComp;
	}

	return nullptr;
}

const glm::vec3& Actor::GetLocalPosition() const
{
	return mTransform.GetPosition();
}

const glm::vec3 Actor::GetGlobalPosition() const
{
	// iterates through each parent to get the global position
	glm::vec3 globalPosition = mTransform.GetPosition();
	std::shared_ptr<Actor> current = this->mParent;
	while (current != nullptr)
	{
		globalPosition = glm::rotate(current->mTransform.GetRotation(), globalPosition);
		globalPosition += current->mTransform.GetPosition();
		current = current->mParent;
	}
	return globalPosition;
}

const glm::quat& Actor::GetLocalRotation() const
{
	return mTransform.GetRotation();
}

const glm::quat Actor::GetGlobalRotation() const
{
	// iterates through each parent to get the global rotation
	glm::quat globalRotation = mTransform.GetRotation();
	std::shared_ptr<Actor> current = this->mParent;
	while (current != nullptr)
	{
		globalRotation = current->mTransform.GetRotation() * globalRotation;
		current = current->mParent;
	}
	return globalRotation;
}

const glm::vec3& Actor::GetLocalScale() const
{
	return mTransform.GetScale();
}

const glm::vec3 Actor::GetGlobalScale() const
{
	// iterates through each parent to get the global scale
	glm::vec3 globalScale = mTransform.GetScale();
	std::shared_ptr<Actor> current = mParent;
	while (current != nullptr)
	{
		globalScale *= current->mTransform.GetScale();
		current = current->mParent;
	}
	return globalScale;
}

const glm::mat4 Actor::GetLocalTransformMatrix() const
{
	return mTransform.GetTransformMatrix();
}

const glm::mat4 Actor::GetGlobalTransformMatrix() const
{
	// iterates through each parent to get the global Transform Matrix
	glm::mat4 globalTransform = mTransform.GetTransformMatrix();
	std::shared_ptr<Actor> current = mParent;
	if (mParent)
	{
		while (current->mParent != nullptr)
		{
			globalTransform = current->mParent->GetLocalTransformMatrix() * globalTransform;
			current = current->mParent;
		}
	}
	return globalTransform;
}

glm::vec3 Actor::GetRight() const
{
	return mTransform.GetRight();
}

std::vector<std::shared_ptr<Actor>>& Actor::GetChildren()
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

void Actor::SetLocalPosition(const glm::vec3& _position)
{
	mTransform.SetPosition(_position);
}

void Actor::SetGlobalPosition(const glm::vec3& _position)
{
	if (mParent)
	{
		// Sets the global position of this actor by setting its position relative to its parents position;
		glm::mat4 parentGlobalInverse = glm::inverse(mParent->GetGlobalTransformMatrix());
		glm::vec4 localPosition = parentGlobalInverse * glm::vec4(_position, 1.0f);
		mTransform.SetPosition(glm::vec3(localPosition));
	}
	// if there is no parent, set local position.
	else
		mTransform.SetPosition(_position);
}

void Actor::SetLocalRotation(const glm::quat& _rotation)
{
	mTransform.SetRotation(_rotation);
}

void Actor::SetGlobalRotation(const glm::quat& _rotation)
{
	if (mParent)
	{
		// Sets the global rotation of this actor by setting its rotation relative to its parents rotation;
		glm::quat parentGlobalRotationInverse = glm::inverse(glm::quat_cast(mParent->GetGlobalTransformMatrix()));
		glm::quat localRotation = parentGlobalRotationInverse * _rotation;
		mTransform.SetRotation(localRotation);
	}
	// if there is no parent, set local rotation
	else
		mTransform.SetRotation(_rotation);
}

void Actor::SetLocalScale(const glm::vec3& _scale)
{

	mTransform.SetScale(_scale);
}

void Actor::SetGlobalScale(const glm::vec3& _scale)
{
	if (mParent)
	{
		// Sets the global scale of this actor by setting its scale relative to its parents scale;
		glm::vec3 parentGlobalScale = mParent->GetGlobalScale();
		glm::vec3 relativeScale = _scale / parentGlobalScale;
		mTransform.SetScale(relativeScale);
	}
	// if there is no parent, set local scale
	else
		mTransform.SetScale(_scale);
}

void Actor::SetParent(std::shared_ptr<Actor> _parent)
{
	// Removes this actors parent
	if (mParent)
		mParent->RemoveChild(shared_from_this());

	// Set new parent by input
	mParent = _parent;

	// Adds "this" actor to new parents children array
	if (mParent)
		mParent->AddChild(shared_from_this());
}