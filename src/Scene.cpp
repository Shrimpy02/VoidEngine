
// Class Includes
#include <Scene.h>
#include <SceneActors.h>
#include <Material.h>
#include <Actor.h>
#include <Camera.h>
#include <Renderer.h>

#include <Core/Shader.h>
#include <Controllers/Controller.h>
#include <Controllers/CameraController.h>
#include <Controllers/ActorController.h>
#include <Components/PhysicsComponent.h>
#include <Components/AIComponent.h>
#include <ModelLoader/AssimpLoader.h>
#include <Lights/DirectionalLight.h>
#include <Lights/PointLight.h>
#include <Utilities/Defines.h>
#include <Utilities/Logger.h>
#include <Core/SMath.h>

// Additional Includes
#include <variant>
#include <ImGUi/imgui.h>

Scene::Scene(const std::string& _name, Window* _window)
	:mSceneGraph(_name), mWindow(_window) {}

void Scene::LoadContent()
{
	// Texture Loading
	// --------------------------------------------
	auto diffuseTex = Texture::Load(SOURCE_DIRECTORY("assets/Textures/ConstainerDiffuse.jpg"));
	auto specTex = Texture::Load(SOURCE_DIRECTORY("assets/Textures/container2Specular.jpg"));
	auto crateMat = Material::Load("Crate", { diffuseTex, specTex }, {});

	auto whiteTex = Texture::Load(SOURCE_DIRECTORY("assets/Textures/white.jpg"));
	auto debugMat = Material::Load("Debug", { whiteTex }, {{glm::vec3(1.0f,0.0f,0.0f)}, {64} });

	// Shader Loading
	// --------------------------------------------
	mShader = new Shader(SOURCE_DIRECTORY("shaderSrc/shader.vs"), SOURCE_DIRECTORY("shaderSrc/shader.fs"));

	// Actor Loading
	// --------------------------------------------
	// Default
	mMACube0 = new BaseActor("Player", Mesh::CreateCube(crateMat));
	mMACube1 = new BaseActor("NPC", Mesh::CreateCube(crateMat));
	//mVAPlane0 = new VisualActor("VAPlane0", Mesh::CreatePlane(crateMat));

	// Lights
	mDirectionalLightActor = new DirectionalLightActor("DirectionalLight0");

	// Assimp Import
	Actor* GroundPlane = new Actor("GroundPlane");
	AssimpLoader::Load(SOURCE_DIRECTORY("assets/Models/Ground/UneavenPlane.fbx"), GroundPlane);


	// Adding Actors to SceneGraph
	// --------------------------------------------
	// Objects
	mSceneGraph.AddChild(&mSceneCamera);
	mSceneGraph.AddChild(mMACube0);
	mSceneGraph.AddChild(mMACube1);
	//mSceneGraph.AddChild(mVAPlane0);
	mSceneGraph.AddChild(GroundPlane);

	// Creates a curve
	std::vector<Points> parametricCurve = SMath::CreateParametricCurve(10, 0.5f);
	// Conforms the curve to the imported geometry
	SMath::ConformCurveToGeometry(parametricCurve, dynamic_cast<VisualActor*>(GroundPlane->GetChildren()[0]->GetChildren()[0]));
	// iterates through each point and creates a visual actor and sets its position for scene visualization
	for (int i = 0; i < parametricCurve.size(); i++)
	{
		VisualActor* newVAPoint = new VisualActor("CurvePoint" + std::to_string(i), Mesh::CreateSphere(debugMat, 1));
		newVAPoint->SetPosition(parametricCurve[i].mPosition,Actor::TransformSpace::Global);
		mSceneGraph.AddChild(newVAPoint);
	}
		

	// Lights
	mSceneGraph.AddChild(mDirectionalLightActor);

	// Setting object location
	// --------------------------------------------
	// Objects
	mSceneCamera.SetPosition({ 0.f, 17.f, 3.f });
	mMACube0->SetPosition({ 0.f, 0.f, 0.f }, Actor::TransformSpace::Global);
	mMACube1->SetPosition({ 0.f, 25.f, 0.f }, Actor::TransformSpace::Global);
	//mVAPlane0->SetScale(glm::vec3(10), Actor::TransformSpace::Global);
	//mVAPlane0->SetPosition(glm::vec3(0,-1,0),Actor::TransformSpace::Global);
	GroundPlane->SetPosition({ 0.f, -4.f, 0.f }, Actor::TransformSpace::Global);

	// Lights
	mDirectionalLightActor->SetRotation(glm::angleAxis(glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)), Actor::TransformSpace::Global);


	// Setting Properties & Components
	// --------------------------------------------
	// Objects
	mMACube0->mCollisionProperties.mType = CollisionType::DYNAMIC;
	mMACube0->mCollisionProperties.mBase = CollisionBase::BoundingSphere;
	mMACube1->mCollisionProperties.mType = CollisionType::DYNAMIC;
	mMACube0->AddComponent<PhysicsComponent>("Cube0PhysicsComponent.h");
	// Dirty cast to assign ground plane to physics component..
	dynamic_cast<PhysicsComponent*>(mMACube0->GetComponents()[0])->SetGroundReference(dynamic_cast<VisualActor*>(GroundPlane->GetChildren()[0]->GetChildren()[0]));
	mMACube1->AddComponent<AIComponent>("Cube1AIComponent.h");
	dynamic_cast<AIComponent*>(mMACube1->GetComponents()[0])->SetActivePath(std::move(parametricCurve));


	// Lights


	// Controller
	mActorController = std::shared_ptr<ActorController>(new ActorController(mMACube0, mWindow));
	mCameraController = std::shared_ptr<CameraController>(new CameraController(&mSceneCamera, mWindow));
	mActiveController = mCameraController;

	// ImGui

	// Sets first frame time, could init to 0. but this is safer
	mOldTime = ImGui::GetTime();

	// Testing:
}

void Scene::UnloadContent()
{
	// Scene objects
	delete mShader;
	delete mMACube0;
	delete mMACube1;
	delete mVAPlane0;

	// Scene Lights
	delete mDirectionalLightActor;

	// Other
	Mesh::ClearCache();
	Material::ClearCache();
	Texture::ClearCache();
	TagUnique::ClearCache();
	LOG("Cache Cleard");
}

void Scene::UpdateInputController(float _dt)
{
	if (mActiveController)
		mActiveController->Update(_dt);
}

void Scene::UpdateSceneGraph(Actor* _actor, float _dt, Transform _globalTransform)
{
	// if there is no actor reference end function
	if (!_actor) return;

	// Set transform matrix
	_globalTransform.SetTransformMatrix(_globalTransform.GetTransformMatrix() * _actor->GetTransformMatrix(Actor::TransformSpace::Local));

	// call update for this actor and its components
	_actor->UpdateComponents(_dt);
	_actor->Update(_dt);

	// for each child recursively run through this function
	const auto& children = _actor->GetChildren();
	for (Actor* child : children)
		UpdateSceneGraph(child, _dt, _globalTransform);
}

void Scene::RenderSceneGraph(Actor* _actor, float _dt, Transform _globalTransform)
{
	// if there is no actor reference end function
	if (!_actor) return;

	// Set transform matrix
	_globalTransform.SetTransformMatrix(_globalTransform.GetTransformMatrix() * _actor->GetTransformMatrix(Actor::TransformSpace::Local));

	// Cast to actor to se if they inherit from IRender,
	// if they do call their inherited draw function and bind the model matrix
	if (auto iRender = dynamic_cast<IRender*>(_actor))
	{
		mShader->setMat4("model", _globalTransform.GetTransformMatrix());
		iRender->Draw(mShader);
	}

	// for each child recursively run through this function
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

	// Update the scene graph -> all objects in scene
	UpdateSceneGraph(&mSceneGraph, _dt);

	// Then handle collision for all objects in scene
	HandleCollision();
}

void Scene::Render(float _dt)
{
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Define what shader to use, then bind light and camera
	mShader->use();
	BindDirectionalLights();
	BindPointLights();
	BindCamera();

	// Render the scene graph -> all objects in scene
	RenderSceneGraph(&mSceneGraph, _dt);
	// Render UI over top, should be called last so it displays updated rather than outdated information
	RenderUI();

	glDepthFunc(GL_LEQUAL);
}

void Scene::HandleCollision()
{
	// Get all IBounded Actors of the scene
	std::vector<Actor*> collisionActors;
	mSceneGraph.Query<IBounded>(collisionActors);

	// for each actor that can bound check it against all others
	for (auto i = 0; i < collisionActors.size(); i++)
	{
		// Get the bounding object and set its collision to false
		IBounded* iA = dynamic_cast<IBounded*>(collisionActors[i]);
		iA->SetIsColliding(false);

		for (auto j = i + 1; j < collisionActors.size(); j++)
		{
			// Get the other bounding object and set its collision to false
			IBounded* iB = dynamic_cast<IBounded*>(collisionActors[j]);
			iB->SetIsColliding(false);

			// if both are static, skip collision check
			// Skip intersection if a object ignores collision
			if (iA->GetCollisionProperties()->IsIgnoreResponse() ||
				iB->GetCollisionProperties()->IsIgnoreResponse())
			{
				continue;
			}

			// Skip intersection checks for two static objects
			if (iA->GetCollisionProperties()->IsStatic() &&
				iB->GetCollisionProperties()->IsStatic())
			{
				continue;
			}

			// To determine if there was a collision 
			bool wasCollision = false;
			// the monstrosity:
			// Processes the appropriate collision between AABB and BoundingSphere based on what the current objects are
			if(iA->GetCollisionProperties()->IsAABB() && iB->GetCollisionProperties()->IsAABB())
			{
				auto a = iA->GetAABB();
				auto b = iB->GetAABB();
				if(ProcessCollision(a,b,iA,iB, collisionActors[i], collisionActors[j]))
					wasCollision = true;
				
			} else if (iA->GetCollisionProperties()->IsAABB() && iB->GetCollisionProperties()->IsBoundingSphere()){

				auto a = iA->GetAABB();
				auto b = iB->GetBoundingSphere();
				if(ProcessCollision(a, b, iA, iB, collisionActors[i], collisionActors[j]))
					wasCollision = true;

			} else if (iA->GetCollisionProperties()->IsBoundingSphere() && iB->GetCollisionProperties()->IsAABB()) {

				auto a = iA->GetBoundingSphere();
				auto b = iB->GetAABB();
				if(ProcessCollision(a, b, iA, iB, collisionActors[i], collisionActors[j]))
					wasCollision = true;

			} else if (iA->GetCollisionProperties()->IsBoundingSphere() && iB->GetCollisionProperties()->IsBoundingSphere()) {

				auto a = iA->GetBoundingSphere();
				auto b = iB->GetBoundingSphere();
				if(ProcessCollision(a, b, iA, iB, collisionActors[i], collisionActors[j]))
					wasCollision = true;

			} else{

				LOG_ERROR("ERROR IN THE MONSTROSITY");
			}

			if (wasCollision == true)
			{
				// Sets both objects collision bool to true since there was a collision.
				iA->SetIsColliding(true);
				iB->SetIsColliding(true);
			}
		}
	}
}

template <typename T, typename U>
bool Scene::ProcessCollision(T _a, U _b, IBounded* _iA, IBounded* _iB, Actor* _AA, Actor* _AB)
{
	// Init minimum translation vector and check if there is an intersection.
	// If there is mtv will be updated by intersect logic.
	glm::vec3 mtv(0.f);
	if (_a.IsIntersecting(_b, &mtv))
	{
		// Temp bool to se if either object are dynamic
		bool isADynamic = _iA->GetCollisionProperties()->IsDynamic();
		bool isBDynamic = _iB->GetCollisionProperties()->IsDynamic();

		// mtv vector init for each object
		glm::vec3 mtvA(0.f), mtvB(0.f);

		// If both actors are dynamic, split the MTV between them
		if (isADynamic && isBDynamic) {
			mtvA = -mtv * 0.5f;
			mtvB = mtv * 0.5f;
		}

		// If only actor A is dynamic, apply the full MTV to A
		else if (isADynamic)
			mtvA = -mtv;

		// If only actor B is dynamic, apply the full MTV to B
		else if (isBDynamic)
			mtvB = mtv;

		// No adjustment for static objects
		// Apply MTV adjustments to objects it has effected
		if (isADynamic)
			_AA->SetPosition(_AA->GetPosition(Actor::TransformSpace::Global) + mtvA, Actor::TransformSpace::Global);
		
		if (isBDynamic)
			_AB->SetPosition(_AB->GetPosition(Actor::TransformSpace::Global) + mtvB, Actor::TransformSpace::Global);

		return true;
	}
	return false;
}

void Scene::RenderUI()
{
	// World object settings window logic 
	imgui_WorldObjectSettings();
	// Logger window logic
	imgui_Logger();

	// Demo window for inspiration and explanation
	//ImGui::ShowDemoWindow();
}

void Scene::imgui_WorldObjectSettings()
{
	// Starts window, set to true.
	// If window is closed this becomes false and code inside is not run. 
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
			std::vector<Actor*> tempSceneActors = mSceneGraph.GetChildren();
			std::vector<const char*> tempSceneActorNames;

			for (auto* actor : tempSceneActors) {
				tempSceneActorNames.push_back(actor->GetTag().c_str());
			}

			ImGui::ListBox("##LB", &mMainSelectionIndex, tempSceneActorNames.data(), (int)tempSceneActorNames.size());
			
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
			mSceneGraph.Query<BaseActor>(tempActors);

			for (auto* actor : tempActors)
			{
				BaseActor* mA = dynamic_cast<BaseActor*>(actor);
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
	if (mOldSelectionIndex != mMainSelectionIndex)
		mCanControlActor = false;
	mOldSelectionIndex = mMainSelectionIndex;

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

	const float maxPitch = (float) 89.9; // Maximum pitch angle to avoid gimbal lock
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
	// Handles all collision setting logic with
	// ImGui and shared_ptr of the object selected

	ImGui::Text("Collision Details");
	ImGui::Separator();
	// Shows if actor is colliding 
	// ----------------------------------------------
	ImGui::Text("Is Colliding: "); ImGui::SameLine();
	if (_cptr->GetIsColliding())
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "True");
	else
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "False");

	// Edit collision type
	// ----------------------------------------------
	const char* typeItems[] = { "STATIC","DYNAMIC","KINETIC" };
	int currentTypeItem = 0;

	currentTypeItem = static_cast<int>(_cptr->GetCollisionProperties()->mType);

	ImGui::Text("Collision Type");
	ImGui::Combo("##TB", &currentTypeItem, typeItems, IM_ARRAYSIZE(typeItems));

	_cptr->GetCollisionProperties()->SetCollisionType(static_cast<CollisionType>(currentTypeItem));

	// Edit collision response
	// ----------------------------------------------
	const char* responseItems[] = { "BLOCK","OVERLAP","IGNORE" };
	int currentResponseItem = 0;

	currentResponseItem = static_cast<int>(_cptr->GetCollisionProperties()->mResponse);

	ImGui::Text("Collision Response");
	ImGui::Combo("##RB", &currentResponseItem, responseItems, IM_ARRAYSIZE(responseItems));

	_cptr->GetCollisionProperties()->SetCollisionResponse(static_cast<CollisionResponse>(currentResponseItem));

	// TODO : Add section here for base collision type for baseActors. Either aabb collision or Bounding sphere collision.
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
	if(ImGui::Begin("System Display"))
	{
		// Handles FPS calculating and displaying
		// ---------------------------------------
		imguiSub_FPS();


	}
	ImGui::End();
}

void Scene::imguiSub_FPS()
{
	// Static init
	// in array values the array nums (10) is the x axis, while its content is the y axis.
	// So it remembers and gets the average from the last 10 seconds in this case. 
	static float values[10] = {};
	static int index = 0;

	// counts the amount of frames processed this round
	numFrames++;

	// Gets the current time
	double currentTime = ImGui::GetTime();
	// Calculates the elapsed time 
	double elapsedTime = currentTime - mOldTime;
	// Each second this if function should tick. (0.004 inaccuracy).
	if (elapsedTime >= 1.0)
	{
		// assigns 
		values[index] = (float) numFrames;
		// once max array size is reached offset is reset to 0
		index = (index + 1) % IM_ARRAYSIZE(values);
		// resets frames and mOldTime to continue the loop
		numFrames = 0;
		mOldTime = currentTime;
	}

	// Find the average FPS value over the 10 seconds of sample fps numbers
	float average = 0.0f;
	int divNum = 1;
	for (int n = 0; n < IM_ARRAYSIZE(values); n++)
	{
		// Does not include 0 or high values since they may inflate average before values are filled in.
		if (values[n] > 0 && values[n] < 10000)
		{
			average += values[n];
			divNum++;
		}
	}

	// Calc the average
	average /= (float)divNum;

	// Write average into char array for dynamic text rendering
	char overlay[32];
	sprintf_s(overlay, "Avg fps %f", average);

	// Sets text pos 50 right and draw
	ImGui::SetCursorPosX(50);
	ImGui::Text(overlay);

	// Draws the graph
	ImGui::PlotLines("##FPSPlot", values, IM_ARRAYSIZE(values), (int)mOldTime, "", -1.0f, 1.0f, ImVec2(200, 80.0f));

	// Section to create descriptive lines on graph
	ImGui::Text(" | "); ImGui::SameLine();
	ImGui::SetCursorPosX(100);
	ImGui::Text(" | "); ImGui::SameLine();
	ImGui::SetCursorPosX(200);
	ImGui::Text(" | ");

	ImGui::Text("10 sec"); ImGui::SameLine();
	ImGui::SetCursorPosX(90);
	ImGui::Text("5 sec"); ImGui::SameLine();
	ImGui::SetCursorPosX(190);
	ImGui::Text("now");

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

void Scene::CharCallback(Window* _window, unsigned int _codepoint)
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
	// Gets all directional light actors of the scene
	std::vector<Actor*> directionalLights;
	mSceneGraph.Query<DirectionalLightActor>(directionalLights);

	// Since there should only be one sun, pas its direction, color and ambient to the general shader.
	if (!directionalLights.empty())
	{
		auto dl = dynamic_cast<DirectionalLightActor*>(directionalLights[0]);
		mShader->setVec3("dl.direction", glm::normalize(dl->GetDirection()));
		mShader->setVec3("dl.color", dl->mColor);
		mShader->setVec3("dl.ambient", dl->mAmbient);
	}

	if(directionalLights.size() > 1)
		LOG_WARNING("More than one directional lights are not bound");
}

void Scene::BindPointLights()
{
	// Gets all point light actors in the scene
	std::vector<Actor*> pointLightActors;
	mSceneGraph.Query<PointLightActor>(pointLightActors);

	// Passes the num point lights total to shader + light specific for each point light
	mShader->setInt("numPointLights", static_cast<int>(pointLightActors.size()));
	for (int i = 0; i < pointLightActors.size(); i++)
	{
		auto PLights = dynamic_cast<PointLightActor*>(pointLightActors[i]);

		if (i > MAX_POINT_LIGHTS) { LOG_WARNING("Max point lights reached, no more being processed"); continue; }

		// Creates the correct array index reference based on the num elements of lights 
		std::string pointLightArrayIndex = "pointLights[" + std::to_string(i) + "]";
		// Passes all point light variables to the shader.
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
	// Passes the cameras matrix`s to the shader for positional computation 
	mShader->setMat4("view", mSceneCamera.GetViewMatrix());
	mShader->setMat4("projection", mSceneCamera.GetProjectionMatrix());
	mShader->setVec3("viewPos", mSceneCamera.GetPosition(Actor::TransformSpace::Global));
}
