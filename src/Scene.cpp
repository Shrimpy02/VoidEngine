#include <Scene.h>
#include <Mesh.h>
#include <Material.h>
#include <Shader.h>
#include <Camera.h>
#include <Actor.h>
#include <ImGUi/imgui.h>
#include <Defines.h>
#include <CameraController.h>
#include <ActorController.h>
#include <Renderer.h>
#include <memory>
#include <Logger.h>

#include <GLFW/glfw3.h>

#include "ModelLoader/AssimpLoader.h"

Scene::Scene(const std::string& _name, Window* _window)
	:mSceneGraph(_name), mWindow(_window) {}

void Scene::LoadContent()
{
	//auto diffuseTex = Texture::Load(SOURCE_DIRECTORY("assets/Textures/ConstainerDiffuse.jpg"));
	auto diffuseTex = Texture::Load(SOURCE_DIRECTORY("assets/Models/StoneScene/Textures/wgprbiq_2K_Albedo.jpg"));
	auto specTex = Texture::Load(SOURCE_DIRECTORY("assets/Textures/container2Specular.jpg"));
	auto mat = Material::Load("Default", { diffuseTex, specTex }, {});

	mCube0 = new MeshActor("Cube0", Mesh::CreateCube(mat));

	Actor* meshTest = new Actor("test");
	AssimpLoader::Load(SOURCE_DIRECTORY("assets/Models/Sponza/Sponzaf.fbx"), meshTest);

	LOG("finished Loading sponza");
	LOG_WARNING("wartning string finished Loading sponza");
	LOG_ERROR("Somethingwhent wrong!");
	LOG_INFO("Have some infor will ya");
	LOG("finished testing");

	//mCube1 = new MeshActor("Cube1", Mesh::CreateCube(mat));
	//mCube2 = new MeshActor("Cube2", Mesh::CreateCube(mat));

	//mPointLightActor0 = new PointLightActor("PointLight0");
	//mPointLightActor1 = new PointLightActor("PointLight1");
	mDirectionalLightActor = new DirectionalLightActor("DirectionalLight0");

	mShader = new Shader(SOURCE_DIRECTORY("shaderSrc/shader.vs"),SOURCE_DIRECTORY("shaderSrc/shader.fs"));
	
	mSceneGraph.AddChild(mCube0);
	mSceneGraph.AddChild(&mSceneCamera);
	//mSceneGraph.AddChild(mCube1);
	//mSceneGraph.AddChild(mCube2);
	mSceneGraph.AddChild(meshTest);

	mCube0->mCollisionProperties.mType = CollisionType::DYNAMIC;
	//mCube1->mCollisionProperties.mType = CollisionType::DYNAMIC;

	//mSceneGraph.AddChild(mPointLightActor0);
	//mSceneGraph.AddChild(mPointLightActor1);
	mSceneGraph.AddChild(mDirectionalLightActor);

	mDirectionalLightActor->SetRotation(glm::angleAxis(glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)),Actor::TransformSpace::Global);

	mCube0->SetPosition({ 0.f, 15.f, 0.f }, Actor::TransformSpace::Global);
	//mCube1->SetPosition({ 2.f, 0.f, 0.f }, Actor::TransformSpace::Global);
	//mPointLightActor0->SetPosition({0.0f,-1.0f,0.0f}, Actor::TransformSpace::Global);
	//mPointLightActor1->SetPosition({ 0.0f,1.0f,0.0f }, Actor::TransformSpace::Global);
	mSceneCamera.SetPosition({ 0.f, 0.f, 3.f });

	mActorController = std::shared_ptr<ActorController>(new ActorController(mCube0, mWindow));
	mCameraController = std::shared_ptr<CameraController>(new CameraController(&mSceneCamera, mWindow));

	mActiveController = mCameraController;
}

void Scene::UnloadContent()
{
	delete mShader;
	delete mCube0;
	delete mCube1;
	delete mCube2;
	delete mDirectionalLightActor;
	delete mPointLightActor0;
	delete mPointLightActor1;

	Mesh::ClearCache();
	Material::ClearCache();
	Texture::ClearCache();
}

void Scene::UpdateInputController(float _dt)
{
	if (mActiveController)
		mActiveController->Update(_dt);
}

void Scene::UpdateSceneGraph(Actor* _actor, float _dt, Transform _globalTransform)
{
	if (!_actor) return;

	_globalTransform.SetTransformMatrix(_globalTransform.GetTransformMatrix() * _actor->GetTransformMatrix(Actor::TransformSpace::Local));

	_actor->Update(_dt);

	const auto& children = _actor->GetChildren();
	for (Actor* child : children)
	{
		UpdateSceneGraph(child, _dt, _globalTransform);
	}
}

void Scene::RenderSceneGraph(Actor* _actor, float _dt, Transform _globalTransform)
{
	if (!_actor) return;

	_globalTransform.SetTransformMatrix(_globalTransform.GetTransformMatrix() * _actor->GetTransformMatrix(Actor::TransformSpace::Local));

	if (auto iRender = dynamic_cast<IRender*>(_actor))
	{
		mShader->setMat4("model", _globalTransform.GetTransformMatrix());
		iRender->Draw(mShader);
	}

	const auto& children = _actor->GetChildren();
	for (Actor* child : children)
	{
		RenderSceneGraph(child, _dt, _globalTransform);
	}
}

void Scene::Update(float _dt)
{
	// Update input first
	UpdateInputController(_dt);

	// Update the scene
	UpdateSceneGraph(&mSceneGraph, _dt);

	// Then handle collision
	HandleCollision();
}

void Scene::Render(float _dt)
{
	glEnable(GL_DEPTH_TEST);

	mShader->use();
	BindDirectionalLights();
	BindPointLights();
	BindCamera();
	RenderSceneGraph(&mSceneGraph, _dt);
	RenderUI();

	glDepthFunc(GL_LEQUAL);
}

void Scene::RenderUI()
{
	imgui_WorldObjectSettings();
	imgui_Logger();
	//ImGui::ShowDemoWindow();
}

void Scene::imgui_WorldObjectSettings()
{
	if(ImGui::Begin("WorldSettings"))
	{
		if(ImGui::CollapsingHeader("Dev Options"))
		{
			// Decides if world is rendered as wire frame or solid mesh.
			// ---------------------------------------------------------
			ImGui::Checkbox("Wire-Frame Mode", &mShouldShowWireFrame);

			if (mShouldShowWireFrame)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			mShader->setBool("ShouldDisableTexture", mShouldShowWireFrame);
		}

		ImGui::Separator();

		if(ImGui::CollapsingHeader("Scene Actors"))
		{

			// Shows all actors in scene, based on selection can take control
			// Note only shows the parent actors so to speak as you cannot control children
			// ----------------------------------------------------------------------------
			std::vector<Actor*>& tempSceneActors = mSceneGraph.GetChildren();
			std::vector<const char*> tempSceneActorNames;

			for (auto* actor : tempSceneActors) {
				tempSceneActorNames.push_back(actor->GetTag().c_str());
			}

			ImGui::ListBox("##LB", &mMainSelectionIndex, tempSceneActorNames.data(), tempSceneActorNames.size());
			
			Actor* currentActor = tempSceneActors[mMainSelectionIndex];
			// Handles all local Sub UI for Actor world settings
			// ----------------------------------------------
			if(currentActor)
				imguiSub_WorldDetails(currentActor);

			// Handles all local Sub UI for Actor Collision settings
			// ----------------------------------------------
			IBounded* collisionPtr = dynamic_cast<IBounded*>(currentActor);
			if (collisionPtr)
				imguiSub_Collision(collisionPtr);

			// Handles all local Sub UI for Actor Light settings
			// ----------------------------------------------
			Light* lightptr = dynamic_cast<Light*>(currentActor);
			if (lightptr)
				imguiSub_Light(lightptr);
		}

		ImGui::Separator();

		// World Physics related options
		if (ImGui::CollapsingHeader("World Physics"))
		{
			// Handles visibility of collision debug mesh
			// ------------------------------------------
			ImGui::Checkbox("Show Collision debug mesh", &mShouldDrawCollisionDebugMesh);
			std::vector<Actor*> tempActors;
			mSceneGraph.Query<MeshActor>(tempActors);

			for (auto* actor : tempActors)
			{
				MeshActor* mA = dynamic_cast<MeshActor*>(actor);
				if(mA)
				{
					if (mShouldDrawCollisionDebugMesh)
						mA->SetDrawDebugCollisionMesh(true);
					else
						mA->SetDrawDebugCollisionMesh(false);
				}
			}
		}
	}
	ImGui::End();
}

void Scene::imguiSub_WorldDetails(Actor* _aptr)
{
	// Actor specific details
	// ----------------------
	ImGui::Text("Actor Details");
	ImGui::Separator();

	// Resets Actor control back to camera when changing selected item
	if (mOldSelectionIndex != mCurrentSelectionIndex)
		mCanControlActor = false;
	mOldSelectionIndex = mCurrentSelectionIndex;

	// Decides if user can control the selected actor
	// ----------------------------------------------
	ImGui::Checkbox("Control Selected Actor", &mCanControlActor);
	if (mCanControlActor)
	{
		// Sets the appropriate controller:
		mActiveController = mActorController;

		// Decides how fast the actor moves
		// ----------------------------------------------
		float tempActorMoveSpeed = mActorController->GetMovementSpeed();
		ImGui::Text("Actor Movement Speed"); ImGui::SameLine(); ImGui::SetNextItemWidth(mItemWidth); ImGui::InputFloat("##AMS", &tempActorMoveSpeed);
		mActorController->SetMovementSpeed(tempActorMoveSpeed);
		mActorController->SetNewActorToControll(_aptr);
	}
	else
		mActiveController = mCameraController;

	// Can edit the x, y, z position for selected actor
	// ----------------------------------------------
	glm::vec3 currentActorsPosition = _aptr->GetPosition(Actor::TransformSpace::Global);

	ImGui::Text("World Position: ");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##PX", &currentActorsPosition.x);

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##PY", &currentActorsPosition.y);

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 0, 1, 1), "Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##PZ", &currentActorsPosition.z);

	_aptr->SetPosition(currentActorsPosition, Actor::TransformSpace::Global);

	// Can edit the x, y, z rotation for selected actor
	// ----------------------------------------------
	glm::quat currentActorsRotation = _aptr->GetRotation(Actor::TransformSpace::Global);

	// Convert quaternion to Euler angles
	glm::vec3 actorRotationInEulerAngles = glm::degrees(glm::eulerAngles(currentActorsRotation));
	float pitch = actorRotationInEulerAngles.x;
	float yaw = actorRotationInEulerAngles.y;
	float roll = actorRotationInEulerAngles.z;

	const float maxPitch = 89.9; // Maximum pitch angle to avoid gimbal lock
	pitch = glm::clamp(pitch, -maxPitch, maxPitch);

	ImGui::Text("World Rotation: ");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Pitch");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##RX", &pitch);

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 1), "Yaw ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##RY", &yaw);

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 0, 1, 1), "Roll");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(mItemWidth);
	ImGui::InputFloat("##RZ", &roll);

	// Convert Euler angles back to quaternion
	glm::vec3 backActorsEulerAngles = glm::radians(glm::vec3(pitch, yaw, roll));
	_aptr->SetRotation(glm::quat(backActorsEulerAngles), Actor::TransformSpace::Global);

	// Can edit the x, y, z scale for selected actor
	// ----------------------------------------------
	ImGui::Text("World Scale: ");
	ImGui::SameLine();
	// Toggle Uniform scaling
	ImGui::Checkbox("Uniform Scale", &mIsUniformScale);

	if (mIsUniformScale) {

		// sets new uniform
		ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "Uniform Scale");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(mItemWidth);
		ImGui::InputFloat("##US", &mCurrentUniformScale);

		// if there is a change in uniform value
		if (mCurrentUniformScale != mOldUniformScale)
		{
			// set scale of actor to its current scale * uniform
			// should only proc once since Old is set to current immediately after
			_aptr->SetScale(mActorOriginalScale * mCurrentUniformScale, Actor::TransformSpace::Global);

			mOldUniformScale = mCurrentUniformScale;
		}

	}
	else {

		glm::vec3 currentActorsScale = _aptr->GetScale(Actor::TransformSpace::Global);
		mCurrentUniformScale = 1;

		ImGui::TextColored(ImVec4(1, 0, 0, 1), "X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(mItemWidth);
		ImGui::InputFloat("##SX", &currentActorsScale.x);

		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(mItemWidth);
		ImGui::InputFloat("##SY", &currentActorsScale.y);

		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 0, 1, 1), "Z");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(mItemWidth);
		ImGui::InputFloat("##SZ", &currentActorsScale.z);

		_aptr->SetScale(glm::vec3(currentActorsScale.x, currentActorsScale.y, currentActorsScale.z), Actor::TransformSpace::Global);
		mActorOriginalScale = _aptr->GetScale(Actor::TransformSpace::Global);
	}
}

void Scene::imguiSub_Collision(IBounded* _cptr)
{
	//// Handles all collision setting logic with
	//// ImGui and shared_ptr of the object selected

	//ImGui::Text("Collision Details");
	//ImGui::Separator();
	//// Shows if actor is colliding 
	//// ----------------------------------------------
	//ImGui::Text("Is Colliding: "); ImGui::SameLine();
	//if (_cptr->IsColliding())
	//	ImGui::TextColored(ImVec4(0, 1, 0, 1), "True");
	//else
	//	ImGui::TextColored(ImVec4(1, 0, 0, 1), "False");

	//// Edit collision type
	//// ----------------------------------------------
	//const char* typeItems[] = { "STATIC","DYNAMIC","KINETIC" };
	//int currentTypeItem = 0;

	//currentTypeItem = static_cast<int>(_cptr->GetCollisionProperties()->mType);

	//ImGui::Text("Collision Type");
	//ImGui::Combo("##TB", &currentTypeItem, typeItems, IM_ARRAYSIZE(typeItems));

	//_cptr->GetCollisionProperties()->SetCollisionType(static_cast<CollisionType>(currentTypeItem));

	//// Edit collision response
	//// ----------------------------------------------
	//const char* responseItems[] = { "BLOCK","OVERLAP","IGNORE" };
	//int currentResponseItem = 0;

	//currentResponseItem = static_cast<int>(_cptr->GetCollisionProperties()->mResponse);

	//ImGui::Text("Collision Response");
	//ImGui::Combo("##RB", &currentResponseItem, responseItems, IM_ARRAYSIZE(responseItems));

	//_cptr->GetCollisionProperties()->SetCollisionResponse(static_cast<CollisionResponse>(currentResponseItem));
}

void Scene::imguiSub_Light(Light* _lptr)
{
	// Handles all collision setting logic with
	// ImGui and shared_ptr of the object selected

	ImGui::Text("Lighting Details");
	ImGui::Separator();
	// Edit ambient light strength
	// ----------------------------------------------
	float ambientStrength = _lptr->mAmbient.x;
	ImGui::Text("Ambient Strength: "); ImGui::SameLine(); ImGui::InputFloat(" ", &ambientStrength);
	_lptr->mAmbient.x = ambientStrength;
	_lptr->mAmbient.y = ambientStrength;
	_lptr->mAmbient.z = ambientStrength;

	// Edit light Color
	// ----------------------------------------------
	glm::vec3 lightColor = _lptr->mColor;
	float colorValues[3] = { lightColor.x,lightColor.y,lightColor.z };
	ImGui::Text("Light Color: "); ImGui::SameLine(); ImGui::ColorEdit3("  ", &colorValues[0]);
	_lptr->mColor.x = colorValues[0];
	_lptr->mColor.y = colorValues[1];
	_lptr->mColor.z = colorValues[2];
}

void Scene::imgui_Logger()
{
	// All errors post here in adddition to termial?
}

void Scene::HandleCollision()
{
	std::vector<Actor*> collisionActors;
	mSceneGraph.Query<IBounded>(collisionActors);

	for (auto i = 0; i < collisionActors.size(); i++)
	{
		for(auto j = i+1; j < collisionActors.size(); j++)
		{
			// Get the two collision actors
			IBounded* iA = dynamic_cast<IBounded*>(collisionActors[i]);
			IBounded* iB = dynamic_cast<IBounded*>(collisionActors[j]);

			shint++;

			// for imgui
			//iA->SetIsColliding(true);
			//iB->SetIsColliding(true);


			//iB->SetIsColliding(false);

			// if both are static, skip collision check
			// Skip intersection if a object ignores collision
			if (iA->GetCollisionProperties().IsIgnoreResponse() ||
				iB->GetCollisionProperties().IsIgnoreResponse())
			{
				continue;
			}

			// Skip intersection checks for two static objects
			if (iA->GetCollisionProperties().IsStatic() &&
				iB->GetCollisionProperties().IsStatic())
			{
				continue;
			}

			auto a = iA->GetAABB();
			auto b = iB->GetAABB();

		
			
			glm::vec3 mtv(0.f);
			if(a.IsIntersecting(b,&mtv))
			{
				//iA->SetIsColliding(true);
				//iB->SetIsColliding(true);

				if (a.IsIntersecting(b, &mtv))
				{
					std::cout << "Shint reached";
				}

				bool isADynamic = iA->GetCollisionProperties().IsDynamic();
				bool isBDynamic = iB->GetCollisionProperties().IsDynamic();

				glm::vec3 mtvA(0.f), mtvB(0.f);

				if(isADynamic && isBDynamic)
				{
					// If both actors are dynamic, split the MTV between them
					mtvA = -mtv * 0.5f;
					mtvB = mtv * 0.5f;
				}
				else if (isADynamic)
				{
					// If only actor A is dynamic, apply the full MTV to A
					mtvA = -mtv;
				}
				else if (isBDynamic)
				{
					// If only actor B is dynamic, apply the full MTV to B
					mtvB = mtv;
				}
				// No adjustment for static objects
				// Apply MTV adjustments
				if (isADynamic)
				{
					collisionActors[i]->SetPosition(collisionActors[i]->GetPosition(Actor::TransformSpace::Global) + mtvA, Actor::TransformSpace::Global);
				}
				if (isBDynamic)
				{
					collisionActors[j]->SetPosition(collisionActors[j]->GetPosition(Actor::TransformSpace::Global) + mtvB, Actor::TransformSpace::Global);
				}
			}
		}
	}
}

void Scene::FramebufferSizeCallback(Window* _window, int _width, int _height)
{
	mSceneCamera.SetAspectRatio(static_cast<float>(_width) / static_cast<float>(_height));
}

void Scene::MouseMoveCallback(Window* _window, double _xpos, double _ypos)
{
	if (mActiveController)
		mActiveController->HandleMouseMove(_window, _xpos, _ypos);
}

void Scene::MouseButtonCallback(Window* _window, int _button, int _action, int _mods)
{
	if (mActiveController)
		mActiveController->HandleMouseButton(_window, _button, _action, _mods);
}

void Scene::MouseScrollCallback(Window* _window, double _xoffset, double _yoffset)
{
	if (mActiveController)
		mActiveController->HandleMouseScroll(_window, _xoffset, _yoffset);
}

void Scene::CharCallback(Window* _window, unsigned int codepoint)
{
	// Mostly for imgui logic. has no glfw oriented function as of yet. 
}

void Scene::KeyCallback(Window* _window, int _key, int _scancode, int _action, int _mods)
{
	if (mActiveController)
		mActiveController->HandleKeyboard(_window, _key, _scancode, _action, _mods);
}

void Scene::BindDirectionalLights()
{
	// light
	std::vector<Actor*> directionalLights;
	mSceneGraph.Query<DirectionalLightActor>(directionalLights);

	if (!directionalLights.empty())
	{
		auto dl = dynamic_cast<DirectionalLightActor*>(directionalLights[0]);
		mShader->setVec3("dl.direction", glm::normalize(dl->GetDirection()));
		mShader->setVec3("dl.color", dl->mColor);
		mShader->setVec3("dl.ambient", dl->mAmbient);
	}
}

void Scene::BindPointLights()
{
	std::vector<Actor*> pointLightActors;
	mSceneGraph.Query<PointLightActor>(pointLightActors);

	mShader->setInt("numPointLights", static_cast<int>(pointLightActors.size()));
	for (int i = 0; i < pointLightActors.size(); i++)
	{
		auto PLights = dynamic_cast<PointLightActor*>(pointLightActors[i]);

		std::string pointLightArrayIndex = "pointLights[" + std::to_string(i) + "]";
		mShader->setVec3(pointLightArrayIndex + ".ambient", PLights->mAmbient);
		mShader->setVec3(pointLightArrayIndex + ".color", PLights->mColor);
		mShader->setVec3(pointLightArrayIndex + ".position", PLights->GetLightPosition());
		mShader->setFloat(pointLightArrayIndex + ".constant", PLights->constantVar);
		mShader->setFloat(pointLightArrayIndex + ".linear", PLights->linearVar);
		mShader->setFloat(pointLightArrayIndex + ".quadratic", PLights->quadraticVar);
	}
}

void Scene::BindCamera()
{
	mShader->setMat4("view", mSceneCamera.GetViewMatrix());
	mShader->setMat4("projection", mSceneCamera.GetProjectionMatrix());
	mShader->setVec3("viewPos", mSceneCamera.GetPosition(Actor::TransformSpace::Global));
}
