
// Class Includes
#include <UserInterface/UserInterfaceManager.h>
#include <Core/Shader.h>
#include <Levels/Level.h>
#include <LevelActors/BaseActor.h>
#include <LevelActors/VisualActor.h>
#include <LevelActors/CameraActor.h>
#include <LevelActors/GraphActor.h>
#include <LevelActors/DebugActor.h>
#include <RenderElements/Mesh.h>
#include <Actor.h>
#include <Lights/DirectionalLight.h>
#include <Lights/PointLight.h>
#include <Controllers/Controller.h>
#include <Utilities/Logger.h>

// Additional Includes
#include <ImGUi/imgui.h>
#include <glad/glad.h>

UserInterfaceManager::UserInterfaceManager(std::shared_ptr<Shader> _shader) : mShader(_shader)
{
}

UserInterfaceManager::~UserInterfaceManager()
{
}

void UserInterfaceManager::UpdateUI()
{
}

void UserInterfaceManager::RenderUI()
{

	ui_WorldObjects();

	ui_ObjectProperties();


	// old ------------
	// World object settings window logic 
	//imgui_WorldObjectSettings();
	// Logger window logic
	//imgui_Logger();

	//imgui_ContentBrowser();

	//imgui_SceneItems();

	// Demo window for inspiration and explanation
	//ImGui::ShowDemoWindow();
}

void UserInterfaceManager::ui_WorldObjects()
{
	if (ImGui::Begin("Level Actors"))
	{
		ImGui::Text("Level Actors");
		
	}
	ImGui::End();
}

void UserInterfaceManager::ui_ObjectProperties()
{

	if (ImGui::Begin("Properties"))
	{
		ImGui::Text("item properties");

	}
	ImGui::End();

}

void UserInterfaceManager::ui_docktest()
{

}

void UserInterfaceManager::imgui_WorldObjectSettings()
{
	// Starts window, set to true.
	// If window is closed this becomes false and code inside is not run. 
	if(ImGui::Begin("WorldSettings") && mShader && mActiveLevel)
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

		if(ImGui::CollapsingHeader("Level Actors"))
		{

			// Shows all actors in scene, based on selection can take control
			// Note only shows the parent actors so to speak as you cannot control children
			// ----------------------------------------------------------------------------
			std::vector<std::shared_ptr<Actor>> tempSceneActors;
			mActiveLevel->mSceneGraph->Query<BaseActor>(tempSceneActors);
			mActiveLevel->mSceneGraph->Query<VisualActor>(tempSceneActors);
			mActiveLevel->mSceneGraph->Query<DirectionalLight>(tempSceneActors);
			mActiveLevel->mSceneGraph->Query<PointLight>(tempSceneActors);
			mActiveLevel->mSceneGraph->Query<CameraActor>(tempSceneActors);
			mActiveLevel->mSceneGraph->Query<GraphActor>(tempSceneActors);

			//mActiveLevel->mSceneGraph->Query<DebugActor>(tempSceneActors);
			//mActiveLevel->mSceneGraph->Query<Actor>(tempSceneActors);

			std::vector<std::shared_ptr<Actor>> tempSceneCameras;
			mActiveLevel->mSceneGraph->Query<CameraActor>(tempSceneCameras);

			std::vector<const char*> tempSceneActorNames;
			for (std::shared_ptr<Actor> actor : tempSceneActors) {
				tempSceneActorNames.push_back(actor->GetTag().c_str());
			}

			std::vector<const char*> tempSceneCameraNames;
			for (std::shared_ptr<Actor> camActor : tempSceneCameras) {
				tempSceneCameraNames.push_back(camActor->GetTag().c_str());
			}

			ImGui::ListBox("##LB", &mMainSelectionIndex, tempSceneActorNames.data(), (int)tempSceneActorNames.size());

			std::shared_ptr<Actor> currentActor = tempSceneActors[mMainSelectionIndex];

			// Decides actor control
			// ----------------------------------------------
			if (currentActor == mController->GetRefToControlledActor())
				mCanControlActor = true;

			ImGui::Checkbox("Control Selected Actor", &mCanControlActor);
			if (mCanControlActor)
			{
				// If new actor is camera, set it to the level camera
				if(std::shared_ptr<CameraActor> newCamActor = std::dynamic_pointer_cast<CameraActor>(tempSceneActors[mMainSelectionIndex]))
				{
					mActiveLevel->mActiveCamera = newCamActor;
				}

				// Decides how fast the actor that is controlled moves
				// ----------------------------------------------
				float tempActorMoveSpeed = mController->GetMovementSpeed();
				ImGui::Text("Actor Movement Speed"); ImGui::SameLine(); ImGui::SetNextItemWidth(mItemWidth); ImGui::InputFloat("##AMS", &tempActorMoveSpeed);
				mController->SetMovementSpeed(tempActorMoveSpeed);
				mController->SetNewActorToControl(currentActor);

				// Decides if a camera should snap to selected actor
				// ----------------------------------------------
				ImGui::Checkbox("Snap camera to actor", &mSnapCameraToActor);
				if (mSnapCameraToActor && currentActor != mActiveLevel->mActiveCamera)
				{
					// Decides what camera should snap
					// ----------------------------------------------
					mActiveLevel->mActiveCamera->mSnappedToActor = currentActor;
					mActiveLevel->mActiveCamera->SetGlobalPosition(currentActor->GetGlobalPosition() + glm::vec3(0, 2, 7));
					mController->SetCameraForSnap(mActiveLevel->mActiveCamera);
					
				} else {

					mSnapCameraToActor = false;
					mActiveLevel->mActiveCamera->mSnappedToActor = nullptr;
					mController->SetCameraForSnap(nullptr);
				}
				// Reset control to level camera when changeing object
			} else {
				if (mActiveLevel->mActiveCamera)
					mController->SetNewActorToControl(mActiveLevel->mActiveCamera);
				mSnapCameraToActor = false;
			}

			// Resets Actor control back to camera when changing selected item
			if (mOldSelectionIndex != mMainSelectionIndex)
			{
				mCanControlActor = false;
				if (mActiveLevel->mActiveCamera)
					mController->SetNewActorToControl(mActiveLevel->mActiveCamera);
			}
			mOldSelectionIndex = mMainSelectionIndex;

			// Handles all local Sub UI for Actor world settings
			// ----------------------------------------------
			if(currentActor)
				imguiSub_WorldDetails(currentActor);

			std::shared_ptr<CameraActor> cameraPtr = std::dynamic_pointer_cast<CameraActor>(currentActor);
			if (cameraPtr)
				imguiSub_Camera(cameraPtr);

			// Handles all local Sub UI for Actor Collision settings
			// ----------------------------------------------
			std::shared_ptr<IBounded> collisionPtr = std::dynamic_pointer_cast<IBounded>(currentActor);
			if (collisionPtr)
				imguiSub_Collision(collisionPtr);

			// Handles all local Sub UI for Actor Light settings
			// ----------------------------------------------
			std::shared_ptr<Light> lightptr = std::dynamic_pointer_cast<Light>(currentActor);
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
			std::vector<std::shared_ptr<Actor>> tempActors;
			mActiveLevel->mSceneGraph->Query<BaseActor>(tempActors);

			for (std::shared_ptr<Actor> actor : tempActors)
			{
				std::shared_ptr<BaseActor> mA = std::dynamic_pointer_cast<BaseActor>(actor);
				if(mA)
				{
					if (mShouldDrawCollisionDebugMesh)
						mA->mCollisionMesh->SetIsVisible(true);
					else
						mA->mCollisionMesh->SetIsVisible(false);
				}
			}
		}
	}
	ImGui::End();
}

void UserInterfaceManager::imguiSub_WorldDetails(std::shared_ptr<Actor> _aptr)
{
	// Actor specific details
	// ----------------------
	ImGui::Text("Actor Details");
	ImGui::Separator();

	// Can edit the x, y, z position for selected actor
	// ----------------------------------------------
	glm::vec3 currentActorsPosition = _aptr->GetGlobalPosition();

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

	_aptr->SetGlobalPosition(currentActorsPosition);

	// Can edit the x, y, z rotation for selected actor
	// ----------------------------------------------
	glm::quat currentActorsRotation = _aptr->GetGlobalRotation();

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
	_aptr->SetGlobalRotation(glm::quat(backActorsEulerAngles));

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
			_aptr->SetGlobalScale(mActorOriginalScale * mCurrentUniformScale);

			mOldUniformScale = mCurrentUniformScale;
		}

	}
	else {

		glm::vec3 currentActorsScale = _aptr->GetGlobalScale();
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

		_aptr->SetGlobalScale(glm::vec3(currentActorsScale.x, currentActorsScale.y, currentActorsScale.z));
		mActorOriginalScale = _aptr->GetGlobalScale();
	}

	// Debug visualize barycentric coordinates 
	// -----------------------------------
	if (_aptr->GetPhysicsComponent() && _aptr->GetPhysicsComponent()->HasSurfaceReference()) {

		ImGui::Checkbox("Show barycentric location", &mShowBarycentricLocation);

		// If checkbox enabled
		if(mShowBarycentricLocation)
		{
			std::vector<glm::vec3> debugMeshPoints = _aptr->GetPhysicsComponent()->GetDebugSurfaceBarycentricPoints();

			// If debugActor does not exist create new, otherwise update its visual mesh
			if (!mBarycentricDebugActor)
			{
				std::string name = _aptr->GetTag() + "_BarycentricDebug";

				mBarycentricDebugActor = std::make_shared<DebugActor>(name, debugMeshPoints);
				mActiveLevel->AddActorToSceneGraph(mBarycentricDebugActor);
			}
			else {

				mBarycentricDebugActor->UpdateVisualMesh(debugMeshPoints);

			}

			// If checkbox disabled remove debug actor reference
		} else {

			if (mBarycentricDebugActor)
			{
				mActiveLevel->RemoveActorFromSceneGraph(mBarycentricDebugActor);
				mBarycentricDebugActor = nullptr;
				mShowBarycentricLocation = false;
			}
		}

		// Same if switched selection
	} else {

		if (mBarycentricDebugActor)
		{
			mActiveLevel->RemoveActorFromSceneGraph(mBarycentricDebugActor);
			mBarycentricDebugActor = nullptr;
			mShowBarycentricLocation = false;
		}

	}

	// Component display
	// -----------------------------------
	ImGui::Text("Actor Components: ");
	std::vector<std::shared_ptr<Component>> actorComponents(_aptr->GetComponents());
	std::vector<const char*> componentNames;
	for (std::shared_ptr<Component> Component : actorComponents) {
		componentNames.push_back(Component->GetTag().c_str());
	}

	ImGui::ListBox("##LBC", &mComponentSelectionIndex, componentNames.data(), (int)componentNames.size());

}

void UserInterfaceManager::imguiSub_Collision(std::shared_ptr<IBounded> _cptr)
{
	// Handles all collision setting logic with
	// ImGui and shared_ptr of the object selected
	if (ImGui::CollapsingHeader("Collision Details"))
	{
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

		currentTypeItem = static_cast<int>(_cptr->mCollisionProperties.mType);

		ImGui::Text("Collision Type");
		ImGui::Combo("##TB", &currentTypeItem, typeItems, IM_ARRAYSIZE(typeItems));

		_cptr->mCollisionProperties.SetCollisionType(static_cast<CollisionType>(currentTypeItem));

		// Edit collision response
		// ----------------------------------------------
		const char* responseItems[] = { "BLOCK","OVERLAP","IGNORE" };
		int currentResponseItem = 0;

		currentResponseItem = static_cast<int>(_cptr->mCollisionProperties.mResponse);

		ImGui::Text("Collision Response");
		ImGui::Combo("##RB", &currentResponseItem, responseItems, IM_ARRAYSIZE(responseItems));

		_cptr->mCollisionProperties.SetCollisionResponse(static_cast<CollisionResponse>(currentResponseItem));

		// Edit collision base
		// ----------------------------------------------
		const char* baseItems[] = { "AABB","BoundingSphere" };
		int currentbaseItem = 0;

		currentbaseItem = static_cast<int>(_cptr->mCollisionProperties.mBase);

		ImGui::Text("Collision Base");
		ImGui::Combo("##BB", &currentbaseItem, baseItems, IM_ARRAYSIZE(baseItems));

		_cptr->mCollisionProperties.SetCollisionBase(static_cast<CollisionBase>(currentbaseItem));

		// Edit collision sub base
		// ----------------------------------------------
		const char* subBaseItems[] = { "NoSubBase","ConvexHul" };
		int currentSubBaseItem = 0;

		currentSubBaseItem = static_cast<int>(_cptr->mCollisionProperties.mSubBase);

		ImGui::Text("Collision SubBase");
		ImGui::Combo("##SB", &currentSubBaseItem, subBaseItems, IM_ARRAYSIZE(subBaseItems));

		_cptr->mCollisionProperties.SetCollisionSubBase(static_cast<CollisionSubBase>(currentSubBaseItem));

	}
}

void UserInterfaceManager::imguiSub_Light(std::shared_ptr<Light> _lptr)
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

void UserInterfaceManager::imguiSub_Camera(std::shared_ptr<CameraActor> _captr)
{

}

void UserInterfaceManager::imgui_Logger()
{
	if(ImGui::Begin("System Display"))
	{
		// Handles FPS calculating and displaying
		// ---------------------------------------
		imguiSub_FPS();


	}
	ImGui::End();
}

void UserInterfaceManager::imguiSub_FPS()
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

void UserInterfaceManager::imgui_ContentBrowser()
{
	if (ImGui::Begin("ContentBrowser") && mShader && mActiveLevel)
	{

		if (ImGui::BeginTabBar("##TabBar"))
		{
			if (ImGui::BeginTabItem("Primitives"))
			{
				ImGui::Text("ta");


				ImGui::EndTabItem();
			}
			

			if (ImGui::BeginTabItem("OtherPrimitives"))
			{
				ImGui::Text("da");


				ImGui::EndTabItem();
			}
			
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void UserInterfaceManager::imgui_SceneItems()
{
	if (ImGui::Begin("Scene items") && mShader && mActiveLevel)
	{

		if (ImGui::BeginTabBar("##TabBar"))
		{
			if (ImGui::BeginTabItem("Primitives"))
			{
				ImGui::Text("ta");


				ImGui::EndTabItem();
			}


			if (ImGui::BeginTabItem("OtherPrimitives"))
			{
				ImGui::Text("da");


				ImGui::EndTabItem();
			}

		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

