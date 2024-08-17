#pragma once

// Includes 
#include <Components/PhysicsComponent.h>
#include <Components/AIComponent.h>
#include <Transform.h>

// Additional includes
#include <vector>
#include <memory>

// Forward Declarations


/**
 * @class Actor
 * @brief The actor acts as an object in the world. It contains parent/child link,
 * components and world/local location. All things needed for existence in a scene.
 */
class Actor : public std::enable_shared_from_this<Actor>
{
public:
	// ---------- Global Variables --------------

	int mTriangleIndex;

private:
	// ---------- Local Variables --------------

	 // Scene graph
	std::shared_ptr<Actor> mParent{nullptr};
	std::vector<std::shared_ptr<Actor>> mChildren;
	// Components
	std::vector<std::shared_ptr<Component>> mComponents;

	// Other
	TagUnique mTag;
	Transform mTransform{};

public:
	// ---------- Global functions --------------

	// Constructor
	Actor(const std::string& _name) : mTag(_name), mParent(nullptr) {}

	// Disable moveing and copying of class
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

	void UpdateExtentByRotation(glm::vec3& _extent);

	// Adds a child to this actor, also sets the child`s parent to this actor
	void AddChild(std::shared_ptr<Actor> _child);

	// Removes a specific child from from this actor, also removes child`s parent reference if this actor is its parent.
	void RemoveChild(std::shared_ptr<Actor> _child);

	// Template function that takes in a class and name, if that class has component as a base it creates
	// that object as a component and adds it to the components vector of this actor.
	template <typename T>
	void AddComponent(const std::string& _componentName)
	{
		// check if this component derives from component class
		static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");

		// if it does create a new one and add it to this actors component vector and run init
		std::shared_ptr<T> component = std::make_shared<T>(_componentName, shared_from_this());
		component->Init();
		mComponents.emplace_back(component);
	}

	// Template function that recursively checks itself and its children if they are actors.
	// If true they are added to the input vector address.
	template <typename T>
	void Query(std::vector<std::shared_ptr<Actor>>& _actors)
	{
		// if this derives from actor, add to vector
		if (dynamic_cast<T*>(this))
			_actors.emplace_back(shared_from_this());

		// recursively check each of this actors children
		for (auto child : this->mChildren)
			child->Query<T>(_actors);
	}

	// Template function that recursively checks itself and its children if they are appropriate component.
	// If true they are added to the input vector address.
	template <typename T>
	void QueryPhysicsComponents(std::vector<std::shared_ptr<PhysicsComponent>>& physicsComponents)
	{
		for (std::shared_ptr<Component> component : mComponents)
		{
			std::shared_ptr<PhysicsComponent> physicsComp = std::dynamic_pointer_cast<PhysicsComponent>(component);
			if (physicsComp)
			{
				physicsComponents.push_back(physicsComp);
			}
		}
	}

private:
	// ---------- Local functions --------------

public:
	// ---------- Getters / setters / adders --------------

	// Getters ---------
	// transform getters

	// Returns actors current local position as vec3
	const glm::vec3& GetLocalPosition() const;
	// Returns actors current global position as vec3
	const glm::vec3 GetGlobalPosition() const;
	// Returns actors current local rotation as quaternion
	const glm::quat& GetLocalRotation() const;
	// Returns actors current global rotation as quaternion
	const glm::quat GetGlobalRotation() const;
	// Returns the actors local current scale as a vec3
	const glm::vec3& GetLocalScale() const;
	// Returns the actors global current scale as a vec3
	const glm::vec3 GetGlobalScale() const;
	// Returns the Transformation local matrix of this actor as a mat4
	const glm::mat4 GetLocalTransformMatrix() const;
	// Returns the Transformation global matrix of this actor as a mat4
	const glm::mat4 GetGlobalTransformMatrix() const;
	// Returns the transform of this actor as a Transform pointer
	std::shared_ptr<Transform> GetTransform() { return std::make_shared<Transform>(mTransform); }
	// Returns the right vector of this actor as a vec3
	glm::vec3 GetRight() const;

	// other getters

	// Returns pointer to the first physics component of this actor.
	std::shared_ptr<PhysicsComponent> GetPhysicsComponent();


	// Returns pointer to the first Ai component of this actor.
	std::shared_ptr<AIComponent> GetAIComponent();

	// Returns the name of this actor
	const std::string& GetTag() { return mTag.GetValue(); }
	// Returns a vector of actor pointers containing all of this actors children
	std::vector<std::shared_ptr<Actor>>& GetChildren();
	// Returns a vector of component pointers containing all of this actors components
	std::vector<std::shared_ptr<Component>> GetComponents() { return mComponents; }


	// Setters ---------
	// transform setters

	// Sets the local transformation matrix for this actor by in mat4
	void SetLocalTransformMatrix(const glm::mat4& _transformMatrix);
	// Sets the transform object of this actor by in transform reference
	void SetTransform(const Transform& _transform);
	// Sets the local position of this actor by in vec3 and in TransformSpace type
	void SetLocalPosition(const glm::vec3& _position);
	// Sets the global position of this actor by in vec3 and in TransformSpace type
	void SetGlobalPosition(const glm::vec3& _position);
	// Sets the local rotation of this actor by in quaternion and in TransformSpace type
	void SetLocalRotation(const glm::quat& _rotation);
	// Sets the global rotation of this actor by in quaternion and in TransformSpace type
	void SetGlobalRotation(const glm::quat& _rotation);
	// Sets the local scale of this actor by in vec3 and in TransformSpace type
	void SetLocalScale(const glm::vec3& _scale);
	// Sets the global scale of this actor by in vec3 and in TransformSpace type
	void SetGlobalScale(const glm::vec3& _scale);
	// Sets teh parent of this actor to in Actor pointer
	void SetParent(std::shared_ptr<Actor> _parent);
};