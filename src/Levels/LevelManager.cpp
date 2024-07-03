
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
//#include <Lights/DirectionalLight.h>
//#include <Lights/PointLight.h>
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

LevelManager::LevelManager(Window* _window) : mWindow(_window)
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
	std::shared_ptr defaultCube = std::make_shared<BaseActor>("DefaultCube", Mesh::CreateCube(nullptr));
	mActiveLevel->AddActorToSceneGraph(defaultCube);
	std::shared_ptr<CameraActor> cam1 = std::make_shared<CameraActor>("Camera1");
	cam1->SetGlobalPosition(glm::vec3(0, 0, 3));
	mActiveLevel->AddActorToSceneGraph(cam1);
	mActiveLevel->mActiveCamera = cam1;

	mActorController = std::shared_ptr<ActorController>(std::make_shared<ActorController>(defaultCube, mWindow));
	mCameraController = std::shared_ptr<CameraController>(std::make_shared<CameraController>(cam1, mWindow));
	mActiveController = mCameraController;
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
	//// Gets all directional light actors of the scene
	//std::vector<Actor*> directionalLights;
	//mSceneGraph.Query<DirectionalLightActor>(directionalLights);
	//
	//// Since there should only be one sun, pas its direction, color and ambient to the general shader.
	//if (!directionalLights.empty())
	//{
	//	auto dl = dynamic_cast<DirectionalLightActor*>(directionalLights[0]);
	//	mShader->setVec3("dl.direction", glm::normalize(dl->GetDirection()));
	//	mShader->setVec3("dl.color", dl->mColor);
	//	mShader->setVec3("dl.ambient", dl->mAmbient);
	//}
	//
	//if (directionalLights.size() > 1)
	//	LOG_WARNING("More than one directional lights are not bound");
}

void LevelManager::BindPointLights()
{
	// Gets all point light actors in the scene
	//std::vector<Actor*> pointLightActors;
	//mSceneGraph.Query<PointLightActor>(pointLightActors);
	//
	//// Passes the num point lights total to shader + light specific for each point light
	//mShader->setInt("numPointLights", static_cast<int>(pointLightActors.size()));
	//for (int i = 0; i < pointLightActors.size(); i++)
	//{
	//	auto PLights = dynamic_cast<PointLightActor*>(pointLightActors[i]);
	//
	//	if (i > MAX_POINT_LIGHTS) { LOG_WARNING("Max point lights reached, no more being processed"); continue; }
	//
	//	// Creates the correct array index reference based on the num elements of lights 
	//	std::string pointLightArrayIndex = "pointLights[" + std::to_string(i) + "]";
	//	// Passes all point light variables to the shader.
	//	mShader->setVec3(pointLightArrayIndex + ".ambient", PLights->mAmbient);
	//	mShader->setVec3(pointLightArrayIndex + ".color", PLights->mColor);
	//	mShader->setVec3(pointLightArrayIndex + ".position", PLights->GetLightPosition());
	//	mShader->setFloat(pointLightArrayIndex + ".constant", PLights->constantVar);
	//	mShader->setFloat(pointLightArrayIndex + ".linear", PLights->linearVar);
	//	mShader->setFloat(pointLightArrayIndex + ".quadratic", PLights->quadraticVar);
	//}
}

void LevelManager::BindCamera()
{
	// Passes the cameras matrix`s to the shader for positional computation 
	mShader->setMat4("view", mActiveLevel->mActiveCamera->GetViewMatrix());
	mShader->setMat4("projection", mActiveLevel->mActiveCamera->GetProjectionMatrix());
	mShader->setVec3("viewPos", mActiveLevel->mActiveCamera->GetGlobalPosition());
}
