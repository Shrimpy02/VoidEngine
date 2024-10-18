
// Class Includes
#include <Levels/LevelManager.h>
#include <Levels/Level.h>

#include <LevelActors/SceneGraph.h>
#include <LevelActors/BaseActor.h>
#include <LevelActors/VisualActor.h>
#include <LevelActors/CameraActor.h>
#include <LevelActors/GraphActor.h>
#include <LevelActors/DebugActor.h>
//#include <Material.h>
//#include <Renderer.h>

#include <Components/AISystem.h>
#include <Components/HealthSystem.h>
#include <Components/PhysicsSystem.h>
#include <Components/InventoryComponent.h>

#include <Core/Shader.h>
#include <Core/SSpawner.h>
#include <Controllers/ActorController.h>
#include <Components/PhysicsComponent.h>
#include <Components/AIComponent.h>
#include <ModelLoader/AssimpLoader.h>
#include <Lights/DirectionalLight.h>
#include <Lights/PointLight.h>
#include <Utilities/Defines.h>
#include <Utilities/Logger.h>
#include <RenderElements/Mesh.h>
#include <RenderElements/Material.h>
#include <RenderElements/Texture.h>
#include <UserInterface/UserInterfaceManager.h>
#include <OctTree.h>
//#include <Core/SMath.h>
//#include <SkyBox/Skybox.h>

// Additional Includes
#include <ctime>

void LevelManager::SpawnWave()
{

	for(int i = 0; i < 10 ; i++)
	{
		
		std::shared_ptr<BaseActor> testEnemy = std::make_shared<BaseActor>("Enemy", Mesh::CreateCube(enemyMat), CollisionBase::AABB, glm::vec3(0, 2, -10), glm::vec3(1, 2, 1));
		AddActorToLevel(testEnemy);
		testEnemy->isEnemy = true;
		testEnemy->SetCollisionResponse(CollisionResponse::BLOCK);
		testEnemy->SetCollisionType(CollisionType::DYNAMIC);
		testEnemy->AddComponent<PhysicsComponent>("PhyComp");
		testEnemy->GetPhysicsComponent()->SetSurfaceReference(Surface);

		std::shared_ptr<AIComponent> aiComp = testEnemy->AddComponent<AIComponent>("AIComp");
		mAiSystem->AddComponent(aiComp);
		mAiSystem->SetTarget(aiComp, PLayer);

		std::shared_ptr<HealthComponent> healthComp2 = testEnemy->AddComponent<HealthComponent>("HealthComp");
		mHealthSystem->AddComponent(healthComp2);
		mHealthSystem->SetHealth(healthComp2, 1);

		SSpawner::SetObjectLocationWithinBoundsRandomlyIgnoreY(testEnemy, Surface);

	}

}

LevelManager::LevelManager(std::shared_ptr<ActorController> _inController, std::shared_ptr<UserInterfaceManager> _userIManager)
	: mController(_inController), mUserInterfaceManager(_userIManager)
{
	mApplicationStartTime = time(nullptr);
}

void LevelManager::LoadContent()
{
	mDefaultShader = std::make_shared<Shader>(SOURCE_DIRECTORY("shaderSrc/DefaultShader.vs"), SOURCE_DIRECTORY("shaderSrc/DefaultShader.fs"));
	mGraphShader = std::make_shared<Shader>(SOURCE_DIRECTORY("shaderSrc/GraphShader.vs"), SOURCE_DIRECTORY("shaderSrc/GraphShader.fs"));
	mDebugShader = std::make_shared<Shader>(SOURCE_DIRECTORY("shaderSrc/DebugShader.vs"), SOURCE_DIRECTORY("shaderSrc/DebugShader.fs"));
	mPointCloudShader = std::make_shared<Shader>(SOURCE_DIRECTORY("shaderSrc/PointCloudShader.vs"), SOURCE_DIRECTORY("shaderSrc/PointCloudShader.fs"));
	mSkyboxShader = std::make_shared<Shader>(SOURCE_DIRECTORY("shaderSrc/SkyboxShader.vs"), SOURCE_DIRECTORY("shaderSrc/SkyboxShader.fs"));

	mAllLevels.push_back(std::make_shared<Level>("DefaultLevel"));
	SetActiveLevel(mAllLevels[0]);

	//LoadDefaultLevel();

	//LoadGraphDisplayLevel();

	//LoadPhysicsBoxLevel();

	LoadTestGame();
}

void LevelManager::BaseLevelRequiredObjects()
{
	// Camera
	std::shared_ptr<CameraActor> cam1 = std::make_shared<CameraActor>("Camera1");
	cam1->SetGlobalPosition(glm::vec3(0, 0, 5));
	mActiveLevel->AddActorToSceneGraph(cam1);
	mActiveLevel->mActiveCamera = cam1;

	// Controller
	mController->SetActorToControl(cam1);

	// Lights
	std::shared_ptr<DirectionalLightActor> dla = std::make_shared<DirectionalLightActor>("DirLight1");
	mActiveLevel->AddActorToSceneGraph(dla);
	dla->SetGlobalPosition(glm::vec3(0, 10, 0));
	dla->SetGlobalRotation(glm::angleAxis(glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
}

void LevelManager::LoadDefaultLevel()
{
	BaseLevelRequiredObjects();

	std::shared_ptr<BaseActor> defaultCube1 = std::make_shared<BaseActor>("DefaultCube", Mesh::CreateCube(nullptr));
	mActiveLevel->AddActorToSceneGraph(defaultCube1);

	// Objects
	//std::shared_ptr<Texture> ground1Diffuse = Texture::LoadWhiteTexture();
	//std::shared_ptr<Material> ground1Mat = Material::Load("ground1Mat", { ground1Diffuse }, { {glm::vec3(1.0f,1.0f,1.0f)}, {16} });
	//std::shared_ptr<VisualActor> SceneGround = std::make_shared<VisualActor>("SceneGround", Mesh::CreatePlane(ground1Mat));
	//mActiveLevel->AddActorToSceneGraph(SceneGround);
	//SceneGround->SetGlobalPosition(glm::vec3(0.f, -1.f, 0.f));
	//SceneGround->SetGlobalRotation(glm::quat(glm::angleAxis(60.f, glm::vec3(0.f, 0.f, 1.f))));
	//SceneGround->SetGlobalScale(glm::vec3(10.f));
	//SceneGround->UpdateExtent();
	//
	//std::shared_ptr<Texture> cube1Diffuse = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerDiffuse.jpg"));
	//std::shared_ptr<Texture> cube1Specular = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerSpecular.jpg"));
	//std::shared_ptr<Material> cube1mat = Material::Load("cube1mat", { cube1Diffuse,cube1Specular }, { {glm::vec3(1.0f,1.0f,1.0f)}, {64} });
	//std::shared_ptr<BaseActor> defaultCube1 = std::make_shared<BaseActor>("DefaultCube1", Mesh::CreateCube(cube1mat));
	//mActiveLevel->AddActorToSceneGraph(defaultCube1);
	//defaultCube1->SetGlobalPosition(glm::vec3(1.2f,0.9f,0.9f));
	//defaultCube1->mCollisionProperties.SetCollisionBase(CollisionBase::BoundingSphere);
	//defaultCube1->mCollisionProperties.SetCollisionType(CollisionType::DYNAMIC);
	//defaultCube1->AddComponent<PhysicsComponent>("PhysicsComp");
	//defaultCube1->GetPhysicsComponent()->SetSurfaceReference(SceneGround);
	//defaultCube1->GetPhysicsComponent()->SetGravityEnabled(true);
	//defaultCube1->AddComponent<AIComponent>("AiComp");
	////defaultCube1->SetShaderObjectType(ShaderObjectType::Default);
	//
	//std::shared_ptr<Texture> cube2Diffuse = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerDiffuse.jpg"));
	//std::shared_ptr<Texture> cube2Specular = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerSpecular.jpg"));
	//std::shared_ptr<Material> cube2mat = Material::Load("cube2mat", { cube2Diffuse,cube2Specular }, { {glm::vec3(1.0f,1.0f,1.0f)}, {64} });
	//std::shared_ptr<BaseActor> defaultCube2 = std::make_shared<BaseActor>("DefaultCube2", Mesh::CreateCube(cube2mat));
	//mActiveLevel->AddActorToSceneGraph(defaultCube2);
	//defaultCube2->SetGlobalPosition(glm::vec3(1.f, 1.f,1.f));
	//defaultCube2->mCollisionProperties.SetCollisionBase(CollisionBase::AABB);
	//defaultCube2->mCollisionProperties.SetCollisionType(CollisionType::DYNAMIC);
	//defaultCube2->mCollisionProperties.SetCollisionResponse(CollisionResponse::BLOCK);

	//std::shared_ptr<GraphActor> graphActor = std::make_shared<GraphActor>("GraphActor1");
	//mActiveLevel->AddActorToSceneGraph(graphActor);
	//graphActor->SetGlobalPosition(glm::vec3(0.f, 5.f, 0.f));
	//graphActor->SetPoints(SMath::DeCastParametricCurveFromPoints(glm::vec3(0, 0, 0), glm::vec3(3, 0, 0), glm::vec3(3, 0, 3)));
	//SMath::ConformCurveToGeometry(graphActor->GetChildren(), SceneGround, 3);
	//defaultCube1->AddChild(graphActor);

	//std::shared_ptr<Actor> Model = std::make_shared<Actor>("DefaultModel");
	//AssimpLoader::Load(SOURCE_DIRECTORY("assets/Models/Ground/UneavenPlane.fbx"), Model);
	//mActiveLevel->AddActorToSceneGraph(Model);

	std::shared_ptr<CameraActor> cam2 = std::make_shared<CameraActor>("Camera2");
	cam2->SetGlobalPosition(glm::vec3(2, 2, 5));
	mActiveLevel->AddActorToSceneGraph(cam2);

	//defaultCube1->SetGlobalRotation(glm::quat( glm::angleAxis(45.f,glm::vec3(0.f,1.f,0.f))));
}

void LevelManager::LoadPhysicsBoxLevel()
{
	BaseLevelRequiredObjects();

	// Scene objects ------------------------
	mConformBox = std::make_shared<VisualActor>("Conform box", Mesh::CreateCube(nullptr), glm::vec3(0), glm::vec3(10.f));
	mActiveLevel->AddActorToSceneGraph(mConformBox);
	mConformBox->mVisualMesh->SetIsVisible(false);

	mActiveLevel->mOctTreeRootNode = std::make_shared<OctTree_Node>(NodeBounds(-mConformBox->mExtent, mConformBox->mExtent), 0, nullptr, mActiveLevel);
	if(!mActiveLevel->mOctTreeRootNode)
	{
		std::shared_ptr<DebugActor> debugActor = std::make_shared<DebugActor>("OctTree_debugBounds");
		std::pair<glm::vec3, glm::vec3> temp = std::make_pair(-mConformBox->mExtent, mConformBox->mExtent);
		debugActor->SetVisualMesh(temp);
		mActiveLevel->AddActorToSceneGraph(debugActor);
	}

	// Objects ------------------------
	// Since instancing is enabled  we can initalize the material once and assign it to the first of the instance
	std::shared_ptr<Texture> diffuseTex = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerDiffuse.jpg"));
	std::shared_ptr<Texture> specularTex = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerSpecular.jpg"));
	std::shared_ptr<Material> mat = Material::Load("cube1mat", { diffuseTex, specularTex }, { {glm::vec3(1.0f,1.0f,1.0f)}, {64} });

	int numSpheres = 1;
	for(int i = 1; i <= numSpheres;i++)
	{
		float randomSize = SMath::GetRandomFloatBetweenMinMax(0.2, 0.8);
		std::shared_ptr<BaseActor> obj = std::make_shared<BaseActor>("Sphere", Mesh::CreateSphere(mat, 2, true), CollisionBase::BoundingSphere, glm::vec3(0.0f), glm::vec3(randomSize));
		mActiveLevel->AddActorToSceneGraph(obj);
		obj->AddComponent<PhysicsComponent>("PhysicsComp");
		obj->GetPhysicsComponent()->SetGravityEnabled(false);
		obj->GetPhysicsComponent()->SetMass(randomSize);
		obj->SetCollisionType(CollisionType::DYNAMIC);
		obj->SetCollisionResponse(CollisionResponse::BLOCK);
		SSpawner::SetObjectLocationWithinBoundsRandomly(obj, mConformBox);
		obj->UpdateExtent();
		if(mActiveLevel->mOctTreeRootNode)
			mActiveLevel->mOctTreeRootNode->InsertObject(obj);
	}
}

void LevelManager::LoadGraphDisplayLevel()
{
	LOG_INFO("Loading `Graph Display Level`");
	std::chrono::time_point<std::chrono::steady_clock> loadingStart = std::chrono::high_resolution_clock::now();
	BaseLevelRequiredObjects();

	// --------------- General Graph object ----------------------

	LOG("Loading `Graph Actor` example");
	std::vector<glm::vec3> controlPointsGraph = {
		glm::vec3(4, -2, 0),
		glm::vec3(0, 2, 0),
		glm::vec3(-4, -2, 0)
	};
	
	std::shared_ptr<GraphActor> graphActor = std::make_shared<GraphActor>("GraphActor", glm::vec3(0), glm::vec3(0.6f));
	mActiveLevel->AddActorToSceneGraph(graphActor);
	graphActor->SetControlPoints(controlPointsGraph);
	graphActor->CreateGraph(GraphMethod::DeCasteljau, GraphType::Approximated);
	LOG("Finished loading `Graph Actor` example");

	// --------------- Bi-Quadratic B-spline tensor product surface ----------------------

	LOG("Loading `Bi-Quadratic B-spline tensor product surface` example");
	// Define degrees 2 = Bi-Quadratic
	int Du = 2;
	int Dv = 2;

	// Define resolution
	int UResolution = 20;
	int VResolution = 20;

	// Define knot vectors
	std::vector<float> uKnot = { 0.0, 0.0, 0.0, 1.0, 2.0, 2.0, 2.0 };
	std::vector<float> vKnot = { 0.0, 0.0, 0.0, 1.0, 2.0, 2.0, 2.0 }; 

	// Define control points (2D grid)
	std::vector<std::vector<glm::vec3>> controlPoints = {
		{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f)},
		{glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 2.0f, 1.0f), glm::vec3(2.0f, 1.0f, 1.0f)},
		{glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 2.0f), glm::vec3(2.0f, 0.0f, 2.0f)}
	};

	std::shared_ptr<VisualActor> BSplineSurface = std::make_shared<VisualActor>("BSurface", Mesh::CreateBSplineSurface(nullptr, UResolution, VResolution, Du, Dv, uKnot, vKnot, controlPoints));
	AddActorToLevel(BSplineSurface);
	LOG("Finished loading `Bi-Quadratic B-spline tensor product surface` example");

	// -------------------- Point cloud from LAS to render --------------------------------

	LOG("Loading `Point Cloud` example, might take some time...");
	// Creating base actor as point cloud root 
	std::shared_ptr<Actor> terrainRoot = std::make_shared<Actor>("Terrain");
	AddActorToLevel(terrainRoot);

	const std::string sourceFile = SOURCE_DIRECTORY("UserAssets/HightData/Hove");
	std::vector<FileItem> regionDirectories = mUserInterfaceManager->GetDirectoryContents(sourceFile);

	LOG("Starting point cloud conversion to custom file");
	// Reading and converting points into custom txt file 
	std::string fullPathToFileForConversionInn = sourceFile + "/" + regionDirectories[0].mName;
	std::string fullPathToFileForConversionOut = SOURCE_DIRECTORY("/Output") + std::string("/CustomOutputFile.txt");
	//SMath::LASFileToCustomFileOfPoints(fullPathToFileForConversionInn.c_str(), fullPathToFileForConversionOut.c_str());
	LOG("Finished point cloud conversion to custom `txt` file found in /output", regionDirectories.size());

	LOG("Starting point cloud import of `%i` files", regionDirectories.size());
	// Reading all height data files for complete terrain
	bool doOnce = true;
	glm::vec3 centre = glm::vec3(0);
	glm::vec3 diff = glm::vec3(0);
	for(int i = 0; i < regionDirectories.size(); i++)
	{
		std::string fullPath = sourceFile + "/" + regionDirectories[i].mName;
		const char* regionFileName = fullPath.c_str();

		std::string objectName = terrainRoot->GetTag() + "_" + std::to_string(i);
		std::shared_ptr<VisualActor> terrainSector = std::make_shared<VisualActor>(objectName, Mesh::CreatePointCloudFromLASFileSurface(regionFileName,0.01f));
		terrainSector->SetShaderObjectType(ShaderObjectType::PointCloud);
		terrainRoot->AddChild(terrainSector);

		if(doOnce == true)
		{
			centre = terrainSector->GetCentre();
			diff = glm::vec3(0) - centre;
			doOnce = false;
		}
	
		terrainSector->SetGlobalPosition(diff);
		//if (i >= 0) break;
	}
	LOG("Finished importing all point cloud files");
	LOG("Finished loading `Point Cloud` example");


	// Calculate time diff from start to finish for print
	std::chrono::time_point<std::chrono::steady_clock> loadEnd = std::chrono::high_resolution_clock::now();
	double elapsedTimeMs = std::chrono::duration<double, std::milli>(loadEnd - loadingStart).count();
	double elapsedTimeS = elapsedTimeMs / 1000.0;
	LOG_INFO("Finished loading `Graph Display Level`, time elapsed `%.2f` seconds", elapsedTimeS);
}

void LevelManager::LoadTestGame()
{
	BaseLevelRequiredObjects();

	// ECS Implementation:
	// ECS is implemented by adding components to whatever actor is created.
	// It is also added to each component system to so that their data is managed in a DOD method.
	// Some components does not have DOD or a system to managed it like the physics component, this comes from me wanting
	// to experament and compate the two approaches side by side.
	// The UI for the game is done through the UI Interface, the collision detection and managing of the level and its
	// Entities is done here in the level manager.
	// The comments above code sections are sparse because the functions are self explanitory, if there are any questions consult the design document.

	// Creates textures and materials for world objects
	std::shared_ptr<Texture> enemyTex = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/DefaultTextures/RedTexture.jpg"));
	enemyMat = Material::Load("EnemyTexture", { enemyTex }, { {glm::vec3(1.0f,1.0f,1.0f)}, {16} });

	std::shared_ptr<Texture> groundTex = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Fabric/GrayFabic.jpg"));
	std::shared_ptr<Material> groundMat = Material::Load("GroundTexture", { groundTex }, { {glm::vec3(1.0f,1.0f,1.0f)}, {16} });

	// Systems
	mAiSystem = std::make_shared<AISystem>(AiFT_FollowObject);
	mHealthSystem = std::make_shared<HealthSystem>();
	mPhysicsSystem = std::make_shared<PhysicsSystem>();

	// The surface of the game
	Surface = std::make_shared<VisualActor>("Surface", Mesh::CreatePlane(groundMat),glm::vec3(0,-4,0),glm::vec3(40,40,40));
	AddActorToLevel(Surface);

	// The player
	std::shared_ptr<BaseActor> player = std::make_shared<BaseActor>("Player", Mesh::CreateCube(nullptr), CollisionBase::AABB, glm::vec3(0,2,0), glm::vec3(1, 2, 1));
	PLayer = player;
	AddActorToLevel(player);
	player->isPlayer = true;
	player->SetCollisionResponse(CollisionResponse::BLOCK);
	player->SetCollisionType(CollisionType::DYNAMIC);
	player->AddComponent<InventoryComponent>("PhyComp");
	player->AddComponent<PhysicsComponent>("PhyComp");
	player->GetPhysicsComponent()->SetSurfaceReference(Surface);
	std::shared_ptr<HealthComponent> healthComp = player->AddComponent<HealthComponent>("HealthComp");
	mHealthSystem->AddComponent(healthComp);
	mHealthSystem->SetHealth(healthComp, 3);

	// Player weapon
	std::shared_ptr<BaseActor> weapon = std::make_shared<BaseActor>("Player-Weapon", Mesh::CreateCube(nullptr), CollisionBase::AABB, glm::vec3(0.5, 0.2, -1.0), glm::vec3(0.1, 0.1, 2));
	player->AddChild(weapon);
	weapon->SetCollisionResponse(CollisionResponse::IGNORE);

	// Default item
	std::shared_ptr<BaseActor> testItem = std::make_shared<BaseActor>("testItem", Mesh::CreatePyramid(nullptr), CollisionBase::AABB, glm::vec3(-3, 0.5, -3), glm::vec3(0.5, 0.5, 0.5));
	AddActorToLevel(testItem);
	testItem->isItem = true;

	// Default Enemy
	std::shared_ptr<BaseActor> testEnemy = std::make_shared<BaseActor>("Enemy", Mesh::CreateCube(enemyMat), CollisionBase::AABB, glm::vec3(0, 2, -10), glm::vec3(1, 2, 1));
	AddActorToLevel(testEnemy);
	testEnemy->isEnemy = true;
	testEnemy->SetCollisionResponse(CollisionResponse::BLOCK);
	testEnemy->SetCollisionType(CollisionType::DYNAMIC);
	testEnemy->AddComponent<PhysicsComponent>("PhyComp");
	testEnemy->GetPhysicsComponent()->SetSurfaceReference(Surface);

	// Component settupp
	std::shared_ptr<AIComponent> aiComp = testEnemy->AddComponent<AIComponent>("AIComp");
	mAiSystem->AddComponent(aiComp);
	mAiSystem->SetTarget(aiComp,player);

	std::shared_ptr<HealthComponent> healthComp2 = testEnemy->AddComponent<HealthComponent>("HealthComp");
	mHealthSystem->AddComponent(healthComp2);
	mHealthSystem->SetHealth(healthComp2, 1);
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
	// Update the scene graph -> all objects in scene
	UpdateLevelSceneGraph(mActiveLevel->mSceneGraph, _dt);

	if(mActiveLevel->mOctTreeRootNode)
		mActiveLevel->mOctTreeRootNode->OctTreeUpdate();

	// Handle collision within bounding box
	if(mConformBox)
		CheckLevelCollisionWithinBoxBounds(mConformBox);

	// Component System Updates
	mAiSystem->Update(_dt);
	mHealthSystem->Update(_dt);
	mPhysicsSystem->Update(_dt);

	// Then handle collision for all objects in scene
	CheckLevelCollision();

	// Handles lifetime of level objects
	mActiveLevel->LifeTimeUpdate();
}

void LevelManager::Render()
{
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Define what shader to use, then bind light and camera
	BindDirectionalLights(mDefaultShader);
	BindPointLights(mDefaultShader);
	BindCamera(mDefaultShader);
	BindCamera(mGraphShader);
	BindCamera(mDebugShader);
	BindCamera(mPointCloudShader);
	BindCamera(mSkyboxShader);

	RenderLevelSceneGraph(mActiveLevel->mSceneGraph);

	//mSkybox->Render(&mSceneCamera);

	glDepthFunc(GL_LEQUAL);
}

void LevelManager::CheckLevelCollision()
{
	// Get all IBounded Actors of the active level
	std::vector<std::shared_ptr<Actor>> collisionActors;
	mActiveLevel->mSceneGraph->Query<IBounded>(collisionActors);

	// For each actor that can bound check it against all others
	for (int i = 0; i < collisionActors.size(); i++)
	{
		// Get first objects collision
		std::shared_ptr<IBounded> actorColliderA = std::dynamic_pointer_cast<IBounded>(collisionActors[i]);
			bool actorACollided = false;

		for (int j = i + 1; j < collisionActors.size(); j++)
		{
			// Get second objects collision
			std::shared_ptr<IBounded> actorColliderB = std::dynamic_pointer_cast<IBounded>(collisionActors[j]);
			bool actorBCollided = false;

			// Skip intersection
			// ------------------------------------------------

			if(actorColliderA->isBullet && actorColliderB->isPlayer || actorColliderA->isPlayer && actorColliderB->isBullet)
			{
				continue;
			}


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
			if(actorColliderA->IsIntersecting(actorColliderB, &mtv))
			{
				// Eneables Physics component
				//// check if both actors have physics components, if true do physics collision
				//if(collisionActors[i]->GetPhysicsComponent() && collisionActors[j]->GetPhysicsComponent())
				//{
				//	if(!actorACollided && !actorBCollided)
				//	{
				//		IBounded::BoundingSpherex2PhysicsCollision(collisionActors[i], collisionActors[j]);
				//		actorACollided = actorBCollided = true;
				//		continue;
				//	}
				//}

				if(actorColliderA->isBullet && actorColliderB->isEnemy || actorColliderA->isEnemy && actorColliderB->isBullet)
				{
					if (collisionActors[i]->GetHealthComponent())
					{
						mHealthSystem->DoDamage(collisionActors[i]->GetHealthComponent(), 1);

						if(mHealthSystem->IsDead(collisionActors[i]))
						{
							int randomInt = SMath::GetRandomInt(0, 3);
							if (randomInt == 2)
							{
								// Test item
								std::shared_ptr<BaseActor> item = std::make_shared<BaseActor>("ItemDrop", Mesh::CreatePyramid(nullptr), CollisionBase::AABB, collisionActors[i]->GetGlobalPosition(), glm::vec3(0.5, 0.5, 0.5));
								AddActorToLevel(item);
								item->isItem = true;
							}
							RemoveActorFromLevel(collisionActors[i]);
						}
					}

					if (collisionActors[j]->GetHealthComponent())
					{
						mHealthSystem->DoDamage(collisionActors[j]->GetHealthComponent(), 1);

						if (mHealthSystem->IsDead(collisionActors[j]))
						{
				

							int randomInt = SMath::GetRandomInt(0, 3);;
							if(randomInt == 3)
							{
								// Test item
								std::shared_ptr<BaseActor> item = std::make_shared<BaseActor>("ItemDrop", Mesh::CreatePyramid(nullptr), CollisionBase::AABB, collisionActors[j]->GetGlobalPosition(), glm::vec3(0.5, 0.5, 0.5));
								AddActorToLevel(item);
								item->isItem = true;
							}

							RemoveActorFromLevel(collisionActors[j]);
						}
					}
					continue;
				}


				if (actorColliderA->isPlayer && actorColliderB->isEnemy)
				{
					mHealthSystem->DoDamage(collisionActors[i]->GetHealthComponent(), 1);

					if (mHealthSystem->IsDead(collisionActors[i]))
					{
						RemoveActorFromLevel(collisionActors[i]);
					}
					RemoveActorFromLevel(collisionActors[j]);
				}


				if (actorColliderA->isPlayer && actorColliderB->isItem)
				{
					collisionActors[i]->GetInventoryComponent()->AddItem(std::make_shared<Item>(1, "HealthPotion"));
					RemoveActorFromLevel(collisionActors[j]);
				}

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

				actorACollided = true;
				actorBCollided = true;
				continue;
			}
			actorColliderA->SetIsColliding(actorACollided);
			actorColliderB->SetIsColliding(actorBCollided);
		}
	}


}

void LevelManager::CheckLevelCollisionWithinBoxBounds(std::shared_ptr<VisualActor> _conformBox)
{
	// Get all IBounded Actors of the active level
	std::vector<std::shared_ptr<Actor>> collisionActors;
	mActiveLevel->mSceneGraph->Query<IBounded>(collisionActors);

	// For each actor that can bound check it against all others
	for (int i = 0; i < collisionActors.size(); i++)
	{
		std::shared_ptr<IBounded> actorColliderA = std::dynamic_pointer_cast<IBounded>(collisionActors[i]);

		// Skip intersection if object ignore response
		if (actorColliderA->mCollisionProperties.IsIgnoreResponse())
			continue;

		// Skip intersection if object is static
		if (actorColliderA->mCollisionProperties.IsStatic())
			continue;

		// Check intersection
		// ------------------------------------------------

		glm::vec3 mtv{ 0.f };
		glm::vec3 boundaryNormal{ 0.f };
		if (actorColliderA->IsIntersectingConstrictingBoxGeometry(_conformBox, &mtv, &boundaryNormal))
		{
			// check if both actors have physics components, if true do physics collision
			if (collisionActors[i]->GetPhysicsComponent())
			{
				IBounded::BoundingSpherexBoundryPhysicsCollision(collisionActors[i], _conformBox, -boundaryNormal);
				continue;
			}
			// mtv vector init for each object
			glm::vec3 mtvA(0.f);

			// If only actor A is dynamic, apply the full MTV to A
			if (actorColliderA->mCollisionProperties.IsDynamic())
				mtvA = -mtv;

			// No adjustment for static objects
			// Apply MTV adjustments to objects it has effected
			if (actorColliderA->mCollisionProperties.IsDynamic())
				collisionActors[i]->SetGlobalPosition(collisionActors[i]->GetGlobalPosition() + mtv);

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

void LevelManager::RenderLevelSceneGraph(std::shared_ptr<Actor> _actor, Transform _globalTransform)
{
	// If there is no actor reference end function
	if (!_actor) return;

	// Set transform matrix
	//_globalTransform.SetTransformMatrix(_globalTransform.GetTransformMatrix() * _actor->GetGlobalTransformMatrix());

	Transform actorGlobalTransform;
	actorGlobalTransform.SetTransformMatrix(_globalTransform.GetTransformMatrix() * _actor->GetLocalTransformMatrix());


	// Cast to actor to se if they inherit from IRender,
	// if they do call their inherited draw function and bind the model matrix
	if (std::shared_ptr<IRender> iRender = std::dynamic_pointer_cast<IRender>(_actor))
	{
		if(iRender->GetShaderObjectType() == ShaderObjectType::Default){

			mDefaultShader->use();
			mDefaultShader->setMat4("model", actorGlobalTransform.GetTransformMatrix());
			iRender->Draw(mDefaultShader);

		} else if(iRender->GetShaderObjectType() == ShaderObjectType::Graph) {

			mGraphShader->use();
			mGraphShader->setMat4("model", actorGlobalTransform.GetTransformMatrix());
			iRender->Draw(mGraphShader);

		} else if (iRender->GetShaderObjectType() == ShaderObjectType::Debug) {

			mDebugShader->use();
			mDebugShader->setMat4("model", actorGlobalTransform.GetTransformMatrix());
			iRender->Draw(mDebugShader);

		} else if (iRender->GetShaderObjectType() == ShaderObjectType::PointCloud) {

			mPointCloudShader->use();
			mPointCloudShader->setMat4("model", actorGlobalTransform.GetTransformMatrix());
			iRender->Draw(mPointCloudShader);

		} else if (iRender->GetShaderObjectType() == ShaderObjectType::Skybox) {

			mSkyboxShader->use();
			mDebugShader->setMat4("model", actorGlobalTransform.GetTransformMatrix());
			iRender->Draw(mSkyboxShader);
		}
	}

	// For each child recursively run this function
	const auto& children = _actor->GetChildren();
	for (std::shared_ptr<Actor> child : children)
	{
		RenderLevelSceneGraph(child, actorGlobalTransform);
	}
}

void LevelManager::FrameBufferSizeCallback(int _width, int _height)
{
	mActiveLevel->mActiveCamera->SetAspectRatio(static_cast<float>(_height) / static_cast<float>(_width));
}

void LevelManager::AddActorToLevel(std::shared_ptr<Actor> _inActor)
{
	mActiveLevel->AddActorToSceneGraph(_inActor);
}

void LevelManager::RemoveActorFromLevel(std::shared_ptr<Actor> _inActor)
{
	mActiveLevel->RemoveActorFromSceneGraph(_inActor);
}


void LevelManager::ShadersDrawWireFrame(bool _b)
{
	if (_b)
	{
		mDefaultShader->use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mDefaultShader->setBool("ShouldDisableTexture", _b);
		mGraphShader->use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mDebugShader->use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mSkyboxShader->use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	}
	else
	{
		mDefaultShader->use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mDefaultShader->setBool("ShouldDisableTexture", _b);
		mGraphShader->use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mDebugShader->use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mSkyboxShader->use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
}

std::shared_ptr<Actor> LevelManager::LineTrace(glm::vec3 _startPos, glm::vec3 _endPosition, float _resolution, bool _createDebugActor)
{
	// Calculate direction from start to end
	glm::vec3 startToEndDirection = _endPosition - _startPos;
	// N = numPoints, cant be 0
	float n = _resolution;
	if (n == 0)
		n = 1;

	// Vector for debug visualization
	std::vector <glm::vec3> tracePoints;

	// Get all IBounded Actors of the active level
	std::vector<std::shared_ptr<Actor>> collisionActors;
	mActiveLevel->mSceneGraph->Query<IBounded>(collisionActors);

	// initalize return actor
	std::shared_ptr<Actor> collidedActor{nullptr};
	for(int i = 0; i <= n ; i += 1)
	{
		// calculate step
		float step = i / n;

		// Calculate point in direction
		glm::vec3 pointInDirection = _startPos + (step * startToEndDirection);
		// populate debug vector
		tracePoints.push_back(pointInDirection);

		collidedActor = LineTraceCollision(collisionActors, pointInDirection);
	}

	if (_createDebugActor)
		CreateLineTraceDebugActor(tracePoints);


	return collidedActor;
}

std::shared_ptr<Actor> LevelManager::LineTrace(glm::vec3 _startPos, glm::vec3 _direction,float _resolution, float length, bool _createDebugActor)
{
	// N = numPoints, cant be 0
	float n = _resolution;
	if (n == 0)
		n = 1;

	// Vector for debug visualization
	std::vector <glm::vec3> tracePoints;

	// Get all IBounded Actors of the active level
	std::vector<std::shared_ptr<Actor>> collisionActors;
	mActiveLevel->mSceneGraph->Query<IBounded>(collisionActors);

	// initalize return actor
	std::shared_ptr<Actor> collidedActor{ nullptr };
	for (int i = 0; i <= n; i += 1)
	{
		// calculate step
		float step = i / n;

		// Calculate point in direction
		glm::vec3 pointInDirection = _startPos + (step * (_direction * length));
		// populate debug vector
		tracePoints.push_back(pointInDirection);

		if(!collidedActor)
			collidedActor = LineTraceCollision(collisionActors, pointInDirection);
	}

	if (_createDebugActor)
		CreateLineTraceDebugActor(tracePoints);

	return collidedActor;
}

std::shared_ptr<Actor> LevelManager::LineTraceCollision(std::vector<std::shared_ptr<Actor>>& _collisionActors, glm::vec3 _point)
{
	// Check if point collides with anything in level
	for (int i = 0; i < _collisionActors.size(); i++)
	{
		std::shared_ptr<IBounded> actorColliderA = std::dynamic_pointer_cast<IBounded>(_collisionActors[i]);

		// Skip intersection if actor ignores response
		if (actorColliderA->mCollisionProperties.IsIgnoreResponse())
			continue;

		if (actorColliderA->IsIntersectingLineTrace(_point))
			return _collisionActors[i];
	}
	return nullptr;
}

void LevelManager::CreateLineTraceDebugActor(std::vector<glm::vec3> _points)
{
	std::shared_ptr<DebugActor> actor = std::make_shared<DebugActor>("LineTrace",true);
	actor->SetVisualMesh(_points);
	AddActorToLevel(actor);
}

void LevelManager::BindDirectionalLights(std::shared_ptr<Shader> _bindShader)
{
	_bindShader->use();
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
			_bindShader->setVec3("dl.direction", dir);
			_bindShader->setVec3("dl.color", dl->mColor);
			_bindShader->setVec3("dl.ambient", dl->mAmbient);
		} else {
			LOG_ERROR("Cast for directioanl lighting faild");
		}
	} else {

		_bindShader->setVec3("dl.direction", glm::vec3(0));
		_bindShader->setVec3("dl.color", glm::vec3(0));
		_bindShader->setVec3("dl.ambient", glm::vec3(0));
	}
	
	if (directionalLights.size() > 1)
		LOG_WARNING("More than one directional lights are not bound");
}

void LevelManager::BindPointLights(std::shared_ptr<Shader> _bindShader)
{
	_bindShader->use();
	// Gets all point light actors in the scene
	std::vector<std::shared_ptr<Actor>> pointLightActors;
	mActiveLevel->mSceneGraph->Query<PointLightActor>(pointLightActors);
	
	// Passes the num point lights total to shader + light specific for each point light
	_bindShader->setInt("numPointLights", static_cast<int>(pointLightActors.size()));
	for (int i = 0; i < pointLightActors.size(); i++)
	{
		std::shared_ptr<PointLightActor> pl = std::dynamic_pointer_cast<PointLightActor>(pointLightActors[i]);

		if(pl)
		{
			if (i > MAX_POINT_LIGHTS) { LOG_WARNING("Max point lights reached, no more being processed"); continue; }
			// Creates the correct array index reference based on the num elements of lights 
			std::string pointLightArrayIndex = "pointLights[" + std::to_string(i) + "]";
			// Passes all point light variables to the shader.
			_bindShader->setVec3(pointLightArrayIndex + ".ambient", pl->mAmbient);
			_bindShader->setVec3(pointLightArrayIndex + ".color", pl->mColor);
			_bindShader->setVec3(pointLightArrayIndex + ".position", pl->GetLightPosition());
			_bindShader->setFloat(pointLightArrayIndex + ".constant", pl->constantVar);
			_bindShader->setFloat(pointLightArrayIndex + ".linear", pl->linearVar);
			_bindShader->setFloat(pointLightArrayIndex + ".quadratic", pl->quadraticVar);
		} else {
			LOG_ERROR("Cast for point lighting faild");
		}
	}

	if(pointLightActors.size() < 1)
	{
		_bindShader->setVec3("pointLights[0].ambient", glm::vec3(0));
			_bindShader->setVec3("pointLights[0].color", glm::vec3(0));
			_bindShader->setVec3("pointLights[0].position", glm::vec3(0));
			_bindShader->setFloat("pointLights[0].constant", 0.0f);
			_bindShader->setFloat("pointLights[0].linear", 0.0f);
			_bindShader->setFloat("pointLights[0].quadratic", 0.0f);
	}

}

void LevelManager::BindCamera(std::shared_ptr<Shader> _bindShader)
{
	_bindShader->use();
	// Passes the cameras matrix`s to the shader for positional computation 
	_bindShader->setMat4("view", mActiveLevel->mActiveCamera->GetViewMatrix());
	_bindShader->setMat4("projection", mActiveLevel->mActiveCamera->GetProjectionMatrix());
	_bindShader->setVec3("viewPos", mActiveLevel->mActiveCamera->GetGlobalPosition());

}

std::shared_ptr<CameraActor> LevelManager::GetActiveCamera()
{
	return mActiveLevel->mActiveCamera;
}
