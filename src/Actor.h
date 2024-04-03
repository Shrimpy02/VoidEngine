#pragma once
// Includes 
#include <Component.h>
#include <Transform.h>
#include <Tag.h>

// Additional includes
#include <vector>

/**
 * @class Actor
 * @brief The actor acts as an object in the world. It contains parent/child link,
 * components and world/local location. All requirements to exist in a scene.
 */
class Actor
{
public:
	// ---------- Global Variables --------------

	// Enum for what space to transform in.
	enum class TransformSpace{
		Local,
		Global
	};

private:
	// ---------- Local Variables --------------

	 // Scene graph
	Actor* mParent = nullptr;
	std::vector<Actor*> mChildren;

	// Components
	std::vector<Component*> mComponents;

	// Other
	TagUnique mTag;
	Transform mTransform{};

public:
	// ---------- Global functions --------------

	// Constructor
	Actor(const std::string& _name) : mTag(_name), mParent(nullptr) {}

	// Disable moveing and copying
	Actor(const Actor&) = delete;
	Actor& operator=(const Actor&) = delete;
	Actor(Actor&&) = default;
	Actor& operator=(Actor&&) = default;

	// Deconstructs children and components, virtual passes deconstructing to inherited first.
	virtual ~Actor();

	// Updates per tick, virtual for inherited functions
	virtual void Update(float _dt);

	// updates all components per tick
	void UpdateComponents(float _dt);

	// Adds a child to this actor, also sets the child`s parent to this actor
	void AddChild(Actor* _child);

	// Removes a specific child from from this actor, also removes child`s parent reference if this actor is its parent.
	void RemoveChild(Actor* _child);

	// Template function that takes in a class and name, if that class has component as a base it creates
	// that object as a component and adds it to the components vector of this actor.
	template <typename T>
	void AddComponent(const std::string& _componentName)
	{
		static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");

		auto component = new T(_componentName, this);
		component->Init();
		mComponents.emplace_back(component);
	}

	// Template function that recursively checks itself and its children if they are actors.
	// If true they are added to the input vector address.
	template <typename T>
	void Query(std::vector<Actor*>& _actors)
	{
		if (dynamic_cast<T*>(this))
			_actors.emplace_back(this);

		for (auto child : this->mChildren)
			child->Query<T>(_actors);
	}

private:
	// ---------- Local functions --------------

public:
	// ---------- Getters / setters / adders --------------

	// Getters ---------
	// transform getters

	const glm::vec3& GetPosition(Actor::TransformSpace _type) const;
	const glm::quat& GetRotation(Actor::TransformSpace _type) const;
	const glm::vec3& GetScale(Actor::TransformSpace _type) const;
	const Transform& GetTransform() const;
	const glm::mat4 GetTransformMatrix(Actor::TransformSpace _type) const;
	Transform* GetTransform() { return &mTransform; }
	glm::vec3 GetRight() const;

	// other getters

	const std::string& GetTag() { return mTag.GetValue(); }
	std::vector<Actor*>& GetChildren();

	// Setters ---------
	// transform setters

	void SetLocalTransformMatrix(const glm::mat4& _transformMatrix);
	void SetTransform(const Transform& _transform);
	void SetPosition(const glm::vec3& _position, Actor::TransformSpace _type = Actor::TransformSpace::Local);
	void SetRotation(const glm::quat& _rotation, Actor::TransformSpace _type = Actor::TransformSpace::Local);
	void SetScale(const glm::vec3& _scale, Actor::TransformSpace _type = Actor::TransformSpace::Local);

	// other setters
	void SetParent(Actor* _parent);
};
