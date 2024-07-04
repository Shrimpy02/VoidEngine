
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
#include <Controllers/CameraController.h>
#include <Controllers/ActorController.h>
//#include <Components/PhysicsComponent.h>
//#include <Components/AIComponent.h>
//#include <ModelLoader/AssimpLoader.h>
#include <Lights/DirectionalLight.h>
#include <Lights/PointLight.h>
#include <Utilities/Defines.h>
#include <Utilities/Logger.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/Material.h>
#include <RenderElements/Texture.h>
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

	mAllLevels.push_back(std::make_shared<Level>("LevelOne"));
	SetActiveLevel(mAllLevels[0]);

	LoadDefaultLevel();
}

void LevelManager::LoadDefaultLevel()
{
	// Texture / Materials
	std::shared_ptr<Texture> whiteTex = Texture::Load(SOURCE_DIRECTORY("assets/Textures/white.jpg"));
	std::shared_ptr<Material> debugMat = Material::Load("Debug", { whiteTex }, {{glm::vec3(1.0f,0.0f,0.0f)}, {64} });

	// Objects
	std::shared_ptr defaultCube = std::make_shared<BaseActor>("DefaultCube", Mesh::CreateCube(debugMat));
	mActiveLevel->AddActorToSceneGraph(defaultCube);

	// Camera
	std::shared_ptr<CameraActor> cam1 = std::make_shared<CameraActor>("Camera1");
	cam1->SetGlobalPosition(glm::vec3(0, 0, 3));
	mActiveLevel->AddActorToSceneGraph(cam1);
	mActiveLevel->mActiveCamera = cam1;

	// Lights
	std::shared_ptr<DirectionalLightActor> dla = std::make_shared<DirectionalLightActor>("DirLight1");
	mActiveLevel->AddActorToSceneGraph(dla);
	dla->SetGlobalPosition(glm::vec3(0,10,0));
	dla->SetGlobalRotation(glm::angleAxis(glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

	mActorController = std::shared_ptr<ActorController>(std::make_shared<ActorController>(defaultCube, mWindow));
	mCameraController = std::shared_ptr<CameraController>(std::make_shared<CameraController>(cam1, mWindow));
	mActiveController = mActorController;
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
	//HandleCollision();
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
	//RenderUI();
	//mSkybox->Render(&mSceneCamera);

	glDepthFunc(GL_LEQUAL);
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
	_globalTransform.SetTransformMatrix(_globalTransform.GetTransformMatrix() * _actor->GetLocalTransformMatrix());

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
	if (mActiveController)
		mActiveController->Update(_dt);
}

void LevelManager::FramebufferSizeCallback(std::shared_ptr<Window> _window, int _width, int _height)
{
	mActiveLevel->mActiveCamera->SetAspectRatio(static_cast<float>(_width) / static_cast<float>(_height));
}

void LevelManager::MouseMoveCallback(std::shared_ptr<Window> _window, double _xpos, double _ypos)
{
	if (mActiveController)
		mActiveController->HandleMouseMove(_window, _xpos, _ypos);
}

void LevelManager::MouseButtonCallback(std::shared_ptr<Window> _window, int _button, int _action, int _mods)
{
	if (mActiveController)
		mActiveController->HandleMouseButton(_window, _button, _action, _mods);
}

void LevelManager::MouseScrollCallback(std::shared_ptr<Window> _window, double _xoffset, double _yoffset)
{
	if (mActiveController)
		mActiveController->HandleMouseScroll(_window, _xoffset, _yoffset);
}

void LevelManager::CharCallback(std::shared_ptr<Window> _window, unsigned int _codepoint)
{
	// Mostly for imgui logic. has no glfw oriented function as of yet. 
}

void LevelManager::KeyCallback(std::shared_ptr<Window> _window, int _key, int _scancode, int _action, int _mods)
{
	if (mActiveController)
		mActiveController->HandleKeyboard(_window, _key, _scancode, _action, _mods);
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
