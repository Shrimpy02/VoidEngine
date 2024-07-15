
// Class Includes
#include <Levels/LevelManager.h>
#include <Levels/Level.h>

#include <SceneActors.h>
#include <SceneGraph.h>
//#include <Material.h>
#include <Actor.h>
#include <Camera.h>
//#include <Renderer.h>

#include <Core/Shader.h>
#include <Controllers/Controller.h>
#include <Controllers/ActorController.h>
//#include <Components/PhysicsComponent.h>
//#include <Components/AIComponent.h>
#include <ModelLoader/AssimpLoader.h>
#include <Lights/DirectionalLight.h>
#include <Lights/PointLight.h>
#include <Utilities/Defines.h>
#include <Utilities/Logger.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/Material.h>
#include <RenderElements/Texture.h>
#include <UserInterface/UserInterfaceManager.h>

//#include <Core/SMath.h>
//#include <SkyBox/Skybox.h>

// Additional Includes
//#include <variant>
//#include <ImGUi/imgui.h>

LevelManager::LevelManager(std::shared_ptr<Window> _window) : mWindow(_window)
{
}

LevelManager::~LevelManager()
{
}

void LevelManager::LoadContent()
{

	mShader = std::make_shared<Shader>(SOURCE_DIRECTORY("shaderSrc/shader.vs"), SOURCE_DIRECTORY("shaderSrc/shader.fs"));
	mUserInterfaceManager = std::make_shared<UserInterfaceManager>(mShader);


	mAllLevels.push_back(std::make_shared<Level>("LevelOne"));
	SetActiveLevel(mAllLevels[0]);

	LoadDefaultLevel();
}

void LevelManager::LoadDefaultLevel()
{
	// Texture / Materials
	std::shared_ptr<Texture> defTex = Texture::Load(SOURCE_DIRECTORY("assets/Textures/ConstainerDiffuse.jpg"));
	std::shared_ptr<Material> defMat = Material::Load("Default", { defTex }, {{glm::vec3(1.0f,1.0f,1.0f)}, {64} });

	// Objects
	std::shared_ptr<BaseActor> defaultCube1 = std::make_shared<BaseActor>("DefaultCube1", Mesh::CreateCube(defMat));
	mActiveLevel->AddActorToSceneGraph(defaultCube1);
	defaultCube1->SetGlobalPosition(glm::vec3(1.f,0.f,0.f));
	defaultCube1->mCollisionProperties.SetCollisionBase(CollisionBase::AABB);
	defaultCube1->mCollisionProperties.SetCollisionType(CollisionType::DYNAMIC);
	defaultCube1->SetGlobalRotation(glm::angleAxis(glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

	std::shared_ptr<BaseActor> defaultCube2 = std::make_shared<BaseActor>("DefaultCube2", Mesh::CreateCube(defMat));
	mActiveLevel->AddActorToSceneGraph(defaultCube2);
	defaultCube2->SetGlobalPosition(glm::vec3(-1.f, 0.f, 0.f));
	defaultCube2->mCollisionProperties.SetCollisionBase(CollisionBase::AABB);
	defaultCube2->mCollisionProperties.SetCollisionType(CollisionType::DYNAMIC);

	//std::shared_ptr<Actor> Model = std::make_shared<Actor>("DefaultModel");
	//AssimpLoader::Load(SOURCE_DIRECTORY("assets/Models/Ground/UneavenPlane.fbx"), Model);
	//mActiveLevel->AddActorToSceneGraph(Model);

	// Camera
	std::shared_ptr<CameraActor> cam1 = std::make_shared<CameraActor>("Camera1");
	cam1->SetGlobalPosition(glm::vec3(0, 0, 5));
	mActiveLevel->AddActorToSceneGraph(cam1);
	mActiveLevel->mActiveCamera = cam1;

	std::shared_ptr<CameraActor> cam2 = std::make_shared<CameraActor>("Camera2");
	cam2->SetGlobalPosition(glm::vec3(2, 0, 5));
	mActiveLevel->AddActorToSceneGraph(cam2);

	// Lights
	std::shared_ptr<DirectionalLightActor> dla = std::make_shared<DirectionalLightActor>("DirLight1");
	mActiveLevel->AddActorToSceneGraph(dla);
	dla->SetGlobalPosition(glm::vec3(0,10,0));
	dla->SetGlobalRotation(glm::angleAxis(glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

	mController = std::shared_ptr<ActorController>(std::make_shared<ActorController>(cam1, mWindow));
}

void LevelManager::UnloadContent()
{
	Mesh::ClearCache();
	Material::ClearCache();
	Texture::ClearCache();
	TagUnique::ClearCache();
	LOG("Cache Cleard");
}

void LevelManager::SetActiveLevel(std::shared_ptr<Level> _activeLevel)
{
	mActiveLevel = _activeLevel;
}

void LevelManager::Update(float _dt)
{
	// Update input first
	UpdateInputController(_dt);

	// Update the scene graph -> all objects in scene
	UpdateLevelSceneGraph(mActiveLevel->mSceneGraph, _dt);

	// Then handle collision for all objects in scene
	ProcessCollision();

	// Update interfaceData
	mUserInterfaceManager->SetActiveLevel(mActiveLevel);
	mUserInterfaceManager->SetController(mController);
}

void LevelManager::Render(float _dt)
{
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Define what shader to use, then bind light and camera
	mShader->use();
	BindDirectionalLights();
	BindPointLights();
	BindCamera();

	// Render the scene graph -> all objects in scene
	RenderLevelSceneGraph(mActiveLevel->mSceneGraph, _dt);
	// Render UI over top, should be called last so it displays updated rather than outdated information
	mUserInterfaceManager->RenderUI();
	//mSkybox->Render(&mSceneCamera);

	glDepthFunc(GL_LEQUAL);
}

void LevelManager::ProcessCollision()
{
	// Get all IBounded Actors of the active level
	std::vector<std::shared_ptr<Actor>> collisionActors;
	mActiveLevel->mSceneGraph->Query<IBounded>(collisionActors);

	// For each actor that can bound check it against all others
	for (int i = 0; i < collisionActors.size(); i++)
	{
		// Get first objects collision
		std::shared_ptr<IBounded> actorColliderA = std::dynamic_pointer_cast<IBounded>(collisionActors[i]);
		actorColliderA->SetIsColliding(false);

		for (int j = i + 1; j < collisionActors.size(); j++)
		{
			// Get second objects collision
			std::shared_ptr<IBounded> actorColliderB = std::dynamic_pointer_cast<IBounded>(collisionActors[j]);
			actorColliderB->SetIsColliding(false);

			// Skip intersection
			// ------------------------------------------------

			// Skip intersection if either ignore response
			if (actorColliderA->mCollisionProperties.IsIgnoreResponse() ||
				actorColliderB->mCollisionProperties.IsIgnoreResponse())
				continue;

			// Skip intersection if both are static
			if (actorColliderA->mCollisionProperties.IsStatic() &&
				actorColliderB->mCollisionProperties.IsStatic())
				continue;
			
			// Check intersection
			// ------------------------------------------------

			glm::vec3 mtv{ 0.f };
			if(actorColliderA->isIntersecting(actorColliderB, &mtv))
			{
				// mtv vector init for each object
				glm::vec3 mtvA(0.f), mtvB(0.f);

				// If both actors are dynamic, split the MTV between them
				if (actorColliderA->mCollisionProperties.IsDynamic() && actorColliderB->mCollisionProperties.IsDynamic()) {
					mtvA = mtv * -0.5f;
					mtvB = mtv * 0.5f;
				}

				// If only actor A is dynamic, apply the full MTV to A
				else if (actorColliderA->mCollisionProperties.IsDynamic())
					mtvA = -mtv;

				// If only actor B is dynamic, apply the full MTV to B
				else if (actorColliderB->mCollisionProperties.IsDynamic())
					mtvB = mtv;

				// No adjustment for static objects
				// Apply MTV adjustments to objects it has effected
				if (actorColliderA->mCollisionProperties.IsDynamic())
					collisionActors[i]->SetGlobalPosition(collisionActors[i]->GetGlobalPosition() + mtvA);

				if (actorColliderB->mCollisionProperties.IsDynamic())
					collisionActors[j]->SetGlobalPosition(collisionActors[j]->GetGlobalPosition() + mtvB);

				actorColliderA->SetIsColliding(true);
				actorColliderB->SetIsColliding(true);

			}
		}
	}
}

void LevelManager::UpdateLevelSceneGraph(std::shared_ptr<Actor> _actor, float _dt, Transform _globalTransform)
{
	if (!_actor) return;

	// Set transform matrix
	_globalTransform.SetTransformMatrix(_globalTransform.GetTransformMatrix() * _actor->GetLocalTransformMatrix());
	
	_actor->UpdateComponents(_dt);
	_actor->Update(_dt);
	
	const auto& children = _actor->GetChildren();
	for (std::shared_ptr<Actor> child : children)
		UpdateLevelSceneGraph(child, _dt, _globalTransform);
}

void LevelManager::RenderLevelSceneGraph(std::shared_ptr<Actor> _actor, float _dt, Transform _globalTransform)
{
	// if there is no actor reference end function
	if (!_actor) return;

	// Set transform matrix
	_globalTransform.SetTransformMatrix(_globalTransform.GetTransformMatrix() * _actor->GetGlobalTransformMatrix());

	// Cast to actor to se if they inherit from IRender,
	// if they do call their inherited draw function and bind the model matrix
	if (std::shared_ptr<IRender> iRender = std::dynamic_pointer_cast<IRender>(_actor))
	{
		mShader->setMat4("model", _globalTransform.GetTransformMatrix());
		iRender->Draw(mShader);
	}

	// for each child recursively run thrh this function
	const auto& children = _actor->GetChildren();
	for (std::shared_ptr<Actor> child : children)
	{
		RenderLevelSceneGraph(child, _dt, _globalTransform);
	}
}

void LevelManager::UpdateInputController(float _dt)
{
	if (mController)
		mController->Update(_dt);
}

void LevelManager::FramebufferSizeCallback(std::shared_ptr<Window> _window, int _width, int _height)
{
	mActiveLevel->mActiveCamera->SetAspectRatio(static_cast<float>(_width) / static_cast<float>(_height));
}

void LevelManager::MouseMoveCallback(std::shared_ptr<Window> _window, double _xpos, double _ypos)
{
	if (mController)
		mController->HandleMouseMove(_window, _xpos, _ypos);
}

void LevelManager::MouseButtonCallback(std::shared_ptr<Window> _window, int _button, int _action, int _mods)
{
	if (mController)
		mController->HandleMouseButton(_window, _button, _action, _mods);
}

void LevelManager::MouseScrollCallback(std::shared_ptr<Window> _window, double _xoffset, double _yoffset)
{
	if (mController)
		mController->HandleMouseScroll(_window, _xoffset, _yoffset);
}

void LevelManager::CharCallback(std::shared_ptr<Window> _window, unsigned int _codepoint)
{
	// Mostly for imgui logic. has no glfw oriented function as of yet. 
}

void LevelManager::KeyCallback(std::shared_ptr<Window> _window, int _key, int _scancode, int _action, int _mods)
{
	if (mController)
		mController->HandleKeyboard(_window, _key, _scancode, _action, _mods);
}

void LevelManager::BindDirectionalLights()
{
	// Gets all directional light actors of the scene
	std::vector<std::shared_ptr<Actor>> directionalLights;
	mActiveLevel->mSceneGraph->Query<DirectionalLightActor>(directionalLights);
	
	// Since there should only be one sun, pas its direction, color and ambient to the general shader.
	if (!directionalLights.empty())
	{
		std::shared_ptr<DirectionalLightActor> dl = std::dynamic_pointer_cast<DirectionalLightActor>(directionalLights[0]);
		if(dl)
		{
			glm::vec3 dir = glm::normalize(dl->GetDirection());
			mShader->setVec3("dl.direction", dir);
			mShader->setVec3("dl.color", dl->mColor);
			mShader->setVec3("dl.ambient", dl->mAmbient);
		} else {
			LOG_ERROR("Cast for directioanl lighting faild");
		}
	}
	
	if (directionalLights.size() > 1)
		LOG_WARNING("More than one directional lights are not bound");
}

void LevelManager::BindPointLights()
{
	// Gets all point light actors in the scene
	std::vector<std::shared_ptr<Actor>> pointLightActors;
	mActiveLevel->mSceneGraph->Query<PointLightActor>(pointLightActors);
	
	// Passes the num point lights total to shader + light specific for each point light
	mShader->setInt("numPointLights", static_cast<int>(pointLightActors.size()));
	for (int i = 0; i < pointLightActors.size(); i++)
	{
		std::shared_ptr<PointLightActor> pl = std::dynamic_pointer_cast<PointLightActor>(pointLightActors[i]);

		if(pl)
		{
			if (i > MAX_POINT_LIGHTS) { LOG_WARNING("Max point lights reached, no more being processed"); continue; }
			// Creates the correct array index reference based on the num elements of lights 
			std::string pointLightArrayIndex = "pointLights[" + std::to_string(i) + "]";
			// Passes all point light variables to the shader.
			mShader->setVec3(pointLightArrayIndex + ".ambient", pl->mAmbient);
			mShader->setVec3(pointLightArrayIndex + ".color", pl->mColor);
			mShader->setVec3(pointLightArrayIndex + ".position", pl->GetLightPosition());
			mShader->setFloat(pointLightArrayIndex + ".constant", pl->constantVar);
			mShader->setFloat(pointLightArrayIndex + ".linear", pl->linearVar);
			mShader->setFloat(pointLightArrayIndex + ".quadratic", pl->quadraticVar);
		} else {
			LOG_ERROR("Cast for point lighting faild");
		}

	}
}

void LevelManager::BindCamera()
{
	// Passes the cameras matrix`s to the shader for positional computation 
	mShader->setMat4("view", mActiveLevel->mActiveCamera->GetViewMatrix());
	mShader->setMat4("projection", mActiveLevel->mActiveCamera->GetProjectionMatrix());
	mShader->setVec3("viewPos", mActiveLevel->mActiveCamera->GetGlobalPosition());
}

