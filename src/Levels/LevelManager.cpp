
// Class Includes
#include <Levels/LevelManager.h>
#include <Levels/Level.h>

#include <LevelActors/SceneGraph.h>
#include <LevelActors/BaseActor.h>
#include <LevelActors/VisualActor.h>
#include <LevelActors/CameraActor.h>
#include <LevelActors/GraphActor.h>
#include <LevelActors/DebugActor.h>
#include <LevelActors/PartcleSystem/ParticleEmitter.h>
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
#include <RenderElements/MeshTypes/PointCloudMesh.h>
#include <RenderElements/VertexTypes/PointCloudVertex.h>
#include <RenderElements/MeshTypes/DefaultMesh.h>
#include <RenderElements/VertexTypes/DefaultVertex.h>
#include <UserInterface/UserInterfaceManager.h>
#include <OctTree.h>
//#include <Core/SMath.h>
//#include <SkyBox/Skybox.h>

// Additional Includes
#include <ctime>
#include <unordered_map>

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

	//LoadTestGame();

	//LoadFolderLevel();

	LoadFolderLevelGameEngine();
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

	// Component setupp
	std::shared_ptr<AIComponent> aiComp = testEnemy->AddComponent<AIComponent>("AIComp");
	mAiSystem->AddComponent(aiComp);
	mAiSystem->SetTarget(aiComp,player);

	std::shared_ptr<HealthComponent> healthComp2 = testEnemy->AddComponent<HealthComponent>("HealthComp");
	mHealthSystem->AddComponent(healthComp2);
	mHealthSystem->SetHealth(healthComp2, 1);
}

void LevelManager::LoadFolderLevel()
{
	bool enablePointCloud = true;
	bool enableCustomFileForPointCloud = false;
	bool enablePointCloudVis = false;
	bool enableTriangulation = true;
	bool enableTriangulationVis = false;
	bool enableTriangulationDebug = false;
	bool enableTriangulationConsole = true;
	bool enableBSplineSurface = false;
	bool enableBSplineDebug = false;
	bool enableBallSpawner = true;
	bool enableMassBalls = true;

	LOG_INFO("Loading `Folder Level`");
	std::chrono::time_point<std::chrono::steady_clock> loadingStart = std::chrono::high_resolution_clock::now();

	// Base requirements for level
	BaseLevelRequiredObjects();

	// Folder Assignment 1.1 --------------------------------
	// Custom writing of point cloud to file

	std::string InputHeightData = SOURCE_DIRECTORY("/UserAssets/HightData/Folder/32-1-498-99-22.laz");
	std::string OutputHeightData = SOURCE_DIRECTORY("/UserAssets/HightData/Folder/Output.txt");
	if (enableCustomFileForPointCloud)
		SMath::LASFileToCustomFileOfPoints(InputHeightData.c_str(), OutputHeightData.c_str());
	LOG("Finished Assignment 1.1, custom file written to:");
	LOG("%s", OutputHeightData.c_str());

	// Folder Assignment 1.2 --------------------------------
	// moving point cloud terrain to viewport

	// Init object
	std::shared_ptr<VisualActor> terrainSector;
	if(enablePointCloud)
	{
		terrainSector = std::make_shared<VisualActor>("PointCloudTerrain", Mesh::CreatePointCloudFromLASFileSurface(InputHeightData.c_str(), 0.01f));
		terrainSector->SetShaderObjectType(ShaderObjectType::PointCloud);
		AddActorToLevel(terrainSector);

		// Adjust terrain position
		glm::vec3 centre = terrainSector->GetCentre();
		glm::vec3 diff = glm::vec3(0) - centre;

		// Update the vertex position by applying the offset
		if (std::shared_ptr<PointCloudMesh> pCM = std::dynamic_pointer_cast<PointCloudMesh>(terrainSector->GetMesh()))
		{
			SMath::AdjustVertexCoordinates(pCM, diff);
			pCM->UpdateMesh();
		}
		terrainSector->GetActorVisualMesh()->SetIsVisible(enablePointCloudVis);
	}
	

	LOG("Finished Assignment 1.2, point cloud generated and adjusted to viewport");

	// Folder Assignment 1.3 --------------------------------
	// Make a plane based on the height data.

	// Get extents for pointCloud mesh
	if(terrainSector)
	{
		glm::vec3 minExtent = glm::vec3(0);
		glm::vec3 maxExtent = glm::vec3(0);
		if (std::shared_ptr<PointCloudMesh> pCM = std::dynamic_pointer_cast<PointCloudMesh>(terrainSector->GetMesh()))
		{
			for (PointCloudVertex& vertex : pCM->mVertices)
			{
				minExtent = glm::min(minExtent, vertex.mPosition);
				maxExtent = glm::max(maxExtent, vertex.mPosition);
			}
		}

		if (enableTriangulation)
		{
			std::shared_ptr<Mesh> mesh = CreateTriangulatedMesh(terrainSector, minExtent, maxExtent, enableTriangulationDebug, enableTriangulationConsole);
			mBallTerrain = std::make_shared<VisualActor>("GeneratedPlane", mesh);
			AddActorToLevel(mBallTerrain);
			mBallTerrain->GetActorVisualMesh()->SetIsVisible(enableTriangulationVis);
		}
	}

	LOG("Finished Assignment 1.3, generated mesh from point cloud terrain");

	// Folder Assignment 1.4 --------------------------------
	// Create a B-Spline surface from the generated mesh

	if(enableBSplineSurface)
	{
		// Define degrees 2 = Bi-Quadratic
		int Du = 2;
		int Dv = 2;

		// Define vertex resolution
		int UResolution = 20;
		int VResolution = 20;
		const int gridSize = 3;

		// Get extents of mesh
		glm::vec3 minExtentMesh = glm::vec3(0);
		glm::vec3 maxExtentMesh = glm::vec3(0);
		if (std::shared_ptr<DefaultMesh> gM = std::dynamic_pointer_cast<DefaultMesh>(mBallTerrain->GetMesh()))
		{
			for (Vertex& vertex : gM->mVertices)
			{
				minExtentMesh = glm::min(minExtentMesh, vertex.mPosition);
				maxExtentMesh = glm::max(maxExtentMesh, vertex.mPosition);
			}
		}

		minExtentMesh += glm::vec3(0.1, 0, 0.1);
		maxExtentMesh -= glm::vec3(0.1, 0, 0.1);

		// Compute control points as a grid over the mesh extents
		std::vector<std::vector<glm::vec3>> controlPoints(gridSize, std::vector<glm::vec3>(gridSize));

		// Generate control points
		glm::vec3 step = (maxExtentMesh - minExtentMesh) / static_cast<float>(gridSize - 1);
		for (int i = 0; i < controlPoints.size(); i++)
		{
			for (int j = 0; j < controlPoints[0].size(); j++)
			{
				controlPoints[i][j] = glm::vec3(minExtentMesh.x + step.x * j, 1.0f, minExtentMesh.z + step.z * i);
			}
		}
		// Conform control point y to terrain height
		for (int i = 0; i < controlPoints.size(); i++)
		{
			for (int j = 0; j < controlPoints[0].size(); j++)
			{
				SMath::ConformPointToGeometry(controlPoints[i][j], mBallTerrain);
			}
		}

		// Create debug objects for control points
		if (enableBSplineDebug)
		{
			for (int i = 0; i < controlPoints.size(); i++)
			{
				for (int j = 0; j < controlPoints[0].size(); j++)
				{
					std::shared_ptr<DebugActor> debugControlPoint = std::make_shared<DebugActor>("DebugControlPoint");
					debugControlPoint->SetVisualMesh(std::make_pair(glm::vec3(-0.2f), glm::vec3(0.2f)));
					debugControlPoint->SetGlobalPosition(controlPoints[i][j]);
					debugControlPoint->SetGlobalScale(glm::vec3(1));
					mActiveLevel->AddActorToSceneGraph(debugControlPoint);
				}
			}
		}

		// Define knot vectors
		std::vector<float> uKnot = SMath::GenerateClampedKnotVector(controlPoints.size(), Du);
		std::vector<float> vKnot = SMath::GenerateClampedKnotVector(controlPoints[0].size(), Dv);

		std::shared_ptr<VisualActor> BSplineSurface = std::make_shared<VisualActor>("BSurface", Mesh::CreateBSplineSurface(nullptr, UResolution, VResolution, Du, Dv, uKnot, vKnot, controlPoints));
		AddActorToLevel(BSplineSurface);

		//BSplineSurface->GetActorVisualMesh()->SetIsVisible(false);
	}
	

	LOG("Finished Assignment 1.4, B-Spline Surface from mesh");

	// Folder Assignment 2.1 --------------------------------
	// Ball rolling on terrain

	// Implemented through the physics component, ball object spawned interactively in next section.

	LOG("Finished Assignment 2.1, Ball Physics");


	// Folder Assignment 2.2 --------------------------------
	// Create debug cube for spawning ball interactively

	if(enableTriangulation && enableBallSpawner)
	{
		mDebugSpawnerBall = std::make_shared<DebugActor>("DebugInteractiveBallSpawner");
		mDebugSpawnerBall->SetVisualMesh(std::make_pair(glm::vec3(-0.2), glm::vec3(0.2)));
		mActiveLevel->AddActorToSceneGraph(mDebugSpawnerBall);
		// Click button "Spawn Ball" in the world manager tab on the right side of the UI
	}

	LOG("Finished Assignment 2.2, Interactive ball spawning");

	// Folder Assignment 2.3 --------------------------------
	// Create friction visually and physically

	// The friction is handled in the physics component along with the rest of the physics.
	// Visually the friction zone is created when triangulating the mesh in 1.3

	LOG("Finished Assignment 2.3, Friction visually and physically");

	// Folder Assignment 2.4 --------------------------------
	// Add Collision detection between balls

	// Collision detection is done each frame in the update section of the level manager

	LOG("Finished Assignment 2.4, Collision");

	// Folder Assignment 2.5 --------------------------------
	// Tracking of individual ball path

	// B-Spline complete, spawns a new control point for the b-spline graph each 0.5 second.
	// The Graph actor manages the actual graph construction
	LOG("Finished Assignment 2.5, B-Spline tracking of physics ball");

	// Folder Assignment 2.6 --------------------------------
	// Mass ball physics simulation
	
	// Generates many balls at the same time with tracking but without collision
	// within the extents of the triangulated mesh

	if(enableMassBalls && enableTriangulation)
	{
		for (int i = 0; i < 100; i++)
		{
			// Create the ball that will be rolling on terrain and update its physics component
			std::shared_ptr<BaseActor> rollingBall = std::make_shared<BaseActor>("RollingBall", Mesh::CreateSphere(nullptr), CollisionBase::BoundingSphere, glm::vec3(0), glm::vec3(0.2f));
			AddActorToLevel(rollingBall);
			rollingBall->mCollisionProperties.SetCollisionType(CollisionType::DYNAMIC);
			rollingBall->mCollisionProperties.SetCollisionResponse(CollisionResponse::IGNORE);
			rollingBall->AddComponent<PhysicsComponent>("PhysicsComp");
			rollingBall->GetPhysicsComponent()->SetSurfaceReference(mBallTerrain);
			rollingBall->GetPhysicsComponent()->SetGravityEnabled(true);
			SSpawner::SetObjectLocationWithinBoundsRandomly(rollingBall, mBallTerrain);

			std::shared_ptr<GraphActor> rollingballGraph = std::make_shared<GraphActor>("RollingBallGraphActor");
			AddActorToLevel(rollingballGraph);
			rollingBall->mGraphActor = rollingballGraph;
		}
	}

	LOG("Finished Assignment 2.6, Mass Ball spawing");
	LOG("Finished all proccessign for Visualization and simulation folder");

	// Calculate time diff from start to finish for print
	std::chrono::time_point<std::chrono::steady_clock> loadEnd = std::chrono::high_resolution_clock::now();
	double elapsedTimeMs = std::chrono::duration<double, std::milli>(loadEnd - loadingStart).count();
	double elapsedTimeS = elapsedTimeMs / 1000.0;
	LOG_INFO("Finished loading `Folder Level`, time elapsed `%.2f` seconds", elapsedTimeS);
}

void LevelManager::LoadFolderLevelGameEngine()
{
	// Game engine folder logic
	LOG_INFO("Loading `Folder Level for GameEngine`");
	std::chrono::time_point<std::chrono::steady_clock> loadingStart = std::chrono::high_resolution_clock::now();

	// Bools for toggle-ing folder functionality
	bool enableSceneGraphExample = false;
	bool enableParticleSystem1 = true;

	// Base requirements for level
	BaseLevelRequiredObjects();

	// Folder Assignment 2.1 --------------------------------
	// Scene Graph Example

	if(enableSceneGraphExample)
	{
		// Using a actors to show parent child scene graph relationship
		std::shared_ptr<BaseActor> CuboeRoot = std::make_shared<BaseActor>("CubeRoot", Mesh::CreateCube(nullptr), CollisionBase::AABB, glm::vec3(0, 2, 0));
		mActiveLevel->AddActorToSceneGraph(CuboeRoot);
		std::shared_ptr<BaseActor> CubeChild1 = std::make_shared<BaseActor>("CubeChild1", Mesh::CreateCube(nullptr), CollisionBase::AABB, glm::vec3(0, -2, 0));
		CuboeRoot->AddChild(CubeChild1);
		std::shared_ptr<BaseActor> CubeChild2 = std::make_shared<BaseActor>("CubeChild2", Mesh::CreateCube(nullptr), CollisionBase::AABB, glm::vec3(0, -2, 0));
		CubeChild1->AddChild(CubeChild2);
	}

	LOG("Finished Assignment 2.1, SceneGraph Example");

	// Folder Assignment 2.2 --------------------------------
	// Particle system

	if(enableParticleSystem1)
	{
		std::shared_ptr<VisualActor> ground = std::make_shared<VisualActor>("Plane", Mesh::CreatePlane(nullptr), glm::vec3(0,-1,0), glm::vec3(10));
		mActiveLevel->AddActorToSceneGraph(ground);

		std::shared_ptr<ParticleEmitter> particleEmitter = std::make_shared<ParticleEmitter>("ParticleEmitter", Mesh::CreateSphere(nullptr,1,true,"ParticleSnow"), glm::vec3(0,5,0));
		particleEmitter->SetSurfaceReference(ground);
		mActiveLevel->AddActorToSceneGraph(particleEmitter);
	}

	LOG("Finished Assignment 2.2, particle system");
	
	// Folder Assignment 3.1 --------------------------------
	// Collision System Example

	// Completed and written to rapport

	LOG("Finished Assignment 3.1, Collision System");

	// Folder Assignment 3.2 --------------------------------
	// ECS and DOD of engine systems
	

	LOG("Finished Assignment 3.2, ECS and DoD of engine systems");

	// Folder Assignment 3.3 --------------------------------
	// Lua Scripting
	

	LOG("Finished Assignment 3.3, Lua scripting");
	LOG("Finished all proccessign for GameEngine Arcitecture folder");

	// Calculate time diff from start to finish for print
	std::chrono::time_point<std::chrono::steady_clock> loadEnd = std::chrono::high_resolution_clock::now();
	double elapsedTimeMs = std::chrono::duration<double, std::milli>(loadEnd - loadingStart).count();
	double elapsedTimeS = elapsedTimeMs / 1000.0;
	LOG_INFO("Finished loading `Folder Level for GameEngine`, time elapsed `%.2f` seconds", elapsedTimeS);
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
	//mAiSystem->Update(_dt);
	//mHealthSystem->Update(_dt);
	//mPhysicsSystem->Update(_dt);

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
				// check if both actors have physics components, if true do physics collision
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

void LevelManager::AddDebugActor(std::vector<std::shared_ptr<DebugActor>>& _debActorStorage, const std::pair<glm::vec3, glm::vec3>& _extents, const glm::vec3& _position, const glm::vec3& _color)
{
	std::shared_ptr<DebugActor> debug = std::make_shared<DebugActor>("DebugSector");
	debug->SetVisualMesh(_extents);
	debug->SetGlobalPosition(_position);
	_debActorStorage.push_back(debug);
}

std::shared_ptr<Mesh> LevelManager::CreateTriangulatedMesh(std::shared_ptr<VisualActor> _poinCloud, const glm::vec3& _min, const glm::vec3& _max, bool _enableDebug, bool _enableConsole)
{
	// Changeable variables for mesh generation
	constexpr int chunkResolution = 2;
	constexpr float sectorOffsetSizePercent = 0.05f;
	constexpr int vertexQuadResolution = 8;
	constexpr float vertexQuadOffsetSizePercent = 0.10f;
	glm::vec3 frictionAreaOffset = glm::vec3();
	constexpr float frictionRadius = 1.0f;

	// Initialize Storage vectors
	std::vector<glm::vec3> vertexPositions;
	std::vector<Index> indices;
	std::vector<std::shared_ptr<DebugActor>> debugActors;

	// Initialize extent division based on resolution and position
	glm::vec3 sectorFar = _max;
	glm::vec3 sectorClose;
	glm::vec3 sectorExtent;
	glm::vec3 extent = (_max - _min);
	glm::vec3 frictionAreaCentre = ((_min + _max) * 0.5f) + _poinCloud->GetGlobalPosition() + frictionAreaOffset;
	float sectorDivX = extent.x / chunkResolution;
	float sectorDivZ = extent.z / chunkResolution;
	float sectorFarZ = sectorFar.z;

	// 1. Divide the point cloud extent into chunks, each chunk contains the point cloud points
	// ----------------------------------------------------------------------------------------
	for (int i = 1; i <= chunkResolution; i++)
	{
		// Reset Z spacing each chunk-x iteration
		sectorFar.z = sectorFarZ;

		for (int j = 1; j <= chunkResolution; j++)
		{
			// 1.1 ------------------------------------
			// Find the chunks specific extent based on chunk resolution

			// Update sector close for each z
			sectorClose = sectorFar - glm::vec3(sectorDivX, sectorFar.y * 2, sectorDivZ);
			sectorExtent = (sectorFar - sectorClose);
			// Calc sector extent with offset for future vertex querying
			glm::vec3 sectorOffsetFar = glm::vec3(sectorFar.x + sectorDivX * sectorOffsetSizePercent, sectorFar.y, sectorFar.z + sectorDivZ * sectorOffsetSizePercent);
			glm::vec3 sectorOffsetClose = glm::vec3(sectorClose.x - sectorDivX * sectorOffsetSizePercent, sectorClose.y, sectorClose.z - sectorDivZ * sectorOffsetSizePercent);

			if (_enableDebug)
			{
				// Optionally creates debug actors for visualization
				std::pair<glm::vec3, glm::vec3> pairExtent = std::make_pair(sectorOffsetFar, sectorOffsetClose);
				AddDebugActor(debugActors, pairExtent, _poinCloud->GetGlobalPosition(), glm::vec3(1, 0, 0));
			}

			// 1.2 ------------------------------------
			// Add all point cloud points that are contained by this chunk to its specific vector
			std::vector<glm::vec3> pointsWithinSection;
			if (std::shared_ptr<PointCloudMesh> pCM = std::dynamic_pointer_cast<PointCloudMesh>(_poinCloud->GetMesh()))
			{
				for (PointCloudVertex& vertex : pCM->mVertices)
				{
					glm::vec3 localExtent = (sectorOffsetFar - sectorOffsetClose) * glm::vec3(0.5);
					glm::vec3 localCentre = (sectorOffsetClose + sectorOffsetFar) * glm::vec3(0.5);
					glm::vec3 localdiff = vertex.mPosition - localCentre;
					bool isWithinSection = true;

					// Check each axis for non intersection, uses early exit
					for (int i = 0; i < 3; i++)
					{
						if (abs(localdiff[i]) > localExtent[i])
						{
							// Vertex not within section
							isWithinSection = false;
							break;
						}
					}
					// Add vertex to vector if within section
					if (isWithinSection)
						pointsWithinSection.push_back(vertex.mPosition);
				}
			}

			// 1.3 ------------------------------------
			// Divide current chunk into quads based on vertex resolution
			glm::vec3 vertexQuadFar = sectorFar;
			glm::vec3 vertexQuadClose;
			vertexQuadFar.y = 0;
			vertexQuadClose.y = 0;
			float vertexDivX = sectorExtent.x / vertexQuadResolution;
			float vertexDivZ = sectorExtent.z / vertexQuadResolution;
			float vertexFarZ = vertexQuadFar.z;

			for (int k = 1; k <= vertexQuadResolution; k++)
			{
				// Reset Z spacing each vertexQuad-x iteration
				vertexQuadFar.z = vertexFarZ;

				for (int l = 1; l <= vertexQuadResolution; l++)
				{
					// Update vertexQuad close for each z
					vertexQuadClose = vertexQuadFar - glm::vec3(vertexDivX, 0, vertexDivZ);

					if (_enableDebug)
					{
						// Optionally creates debug actors for visualization
						std::pair<glm::vec3, glm::vec3> pairExtent = std::make_pair(vertexQuadFar, vertexQuadClose);
						AddDebugActor(debugActors, pairExtent, _poinCloud->GetGlobalPosition(), glm::vec3(0, 1, 0));
					}

					// 1.3.1 ------------------------------------
					// Create offset extents for each vertex to uniformly check chunk point cloud points for average height.
					glm::vec3 vertexQuadCentre = (vertexQuadClose + vertexQuadFar) * glm::vec3(0.5);
					glm::vec3 vertexQuadOffsetFar = glm::vec3(vertexQuadFar.x, 0, vertexQuadFar.z);
					glm::vec3 vertexQuadOffsetClose = glm::vec3(vertexQuadClose.x, 0, vertexQuadClose.z);
					glm::vec3 vertexQuadExtent = (vertexQuadFar - vertexQuadClose);
					float vertexOffsetDivX = vertexQuadExtent.x / 2;
					float vertexOffsetDivZ = vertexQuadExtent.z / 2;
					float vertexOffsetZ = vertexQuadOffsetFar.z;
					int vertexIt = 0;

					// Storage for each vertex of a quad for indexing
					glm::vec3 vert1 = glm::vec3(0);
					glm::vec3 vert2 = glm::vec3(0);
					glm::vec3 vert3 = glm::vec3(0);
					glm::vec3 vert4 = glm::vec3(0);

					for (int q = 1; q <= 2; q++)
					{
						// Reset Z spacing each vertexQuadOffset-x iteration
						vertexQuadOffsetFar.z = vertexOffsetZ;

						for (int w = 1; w <= 2; w++)
						{
							// Update vertexQuadOffset close for each z
							vertexQuadOffsetClose = vertexQuadOffsetFar - glm::vec3(vertexOffsetDivX, 0, vertexOffsetDivZ);

							// The offset offsets for exact uniform distribution of vertex query zones
							glm::vec3 vertexOffsetOffsetFar = glm::vec3();
							glm::vec3 vertexOffsetOffsetClose = glm::vec3();

							// edge checks for x and z in
							// close
							if (vertexQuadOffsetClose.z < vertexQuadCentre.z)
								vertexOffsetOffsetClose += glm::vec3(0, 0, vertexDivZ * vertexQuadOffsetSizePercent);
							if (vertexQuadOffsetClose.x < vertexQuadCentre.x)
								vertexOffsetOffsetClose += glm::vec3(vertexDivX * vertexQuadOffsetSizePercent, 0, 0);
							// far
							if (vertexQuadOffsetFar.z > vertexQuadCentre.z)
								vertexOffsetOffsetFar += glm::vec3(0, 0, vertexDivZ * vertexQuadOffsetSizePercent);
							if (vertexQuadOffsetFar.x > vertexQuadCentre.x)
								vertexOffsetOffsetFar += glm::vec3(vertexDivX * vertexQuadOffsetSizePercent, 0, 0);

							if (_enableDebug)
							{
								// Optionally creates debug actors for visualization
								std::pair<glm::vec3, glm::vec3> pairExtent = std::make_pair(vertexQuadOffsetFar + vertexOffsetOffsetFar, vertexQuadOffsetClose - vertexOffsetOffsetClose);
								AddDebugActor(debugActors, pairExtent, _poinCloud->GetGlobalPosition(), glm::vec3(0, 0, 1));
							}

							// 1.3.2 ------------------------------------
							// Calculate vertex position based grid x,z and average chunk-pointCloud positions y 
							float vertexHeight = 0;
							std::vector<float> pointHeightWithinVertexSection;
							for (glm::vec3& pointPosition : pointsWithinSection)
							{
								// Get the extent around current vertex
								glm::vec3 vertFarExtent = (vertexQuadOffsetFar + vertexOffsetOffsetFar);
								glm::vec3 vertCloseExtent = (vertexQuadOffsetClose - vertexOffsetOffsetClose);

								glm::vec3 vertLocalExtent = (vertFarExtent - vertCloseExtent) * glm::vec3(0.5);
								vertLocalExtent.y = sectorExtent.y;
								glm::vec3 vertCentre = (vertCloseExtent + vertFarExtent) * glm::vec3(0.5);
								glm::vec3 localdiff = pointPosition - vertCentre;
								bool isWithinVertexSection = true;

								// Check each axis for non intersection, uses early exit
								for (int e = 0; e < 3; e++)
								{
									if (abs(localdiff[e]) > vertLocalExtent[e])
									{
										// Vertex not within section
										isWithinVertexSection = false;
										break;
									}
								}

								// If point is within vertex extent, add to vector
								if (isWithinVertexSection)
									pointHeightWithinVertexSection.push_back(pointPosition.y);
							}

							// Calculate the average point height in vertex sector
							for (float pointHeight : pointHeightWithinVertexSection)
								vertexHeight += pointHeight;
							vertexHeight /= pointHeightWithinVertexSection.size();

							// 1.3.3 ------------------------------------
							// Create vertices based on what part of the for loop we are in
							vertexIt++;
							glm::vec3 vertPos = glm::vec3(0);

							switch (vertexIt) {
							case 1: vertPos = glm::vec3(vertexQuadFar.x, vertexHeight, vertexQuadFar.z); break; // top right
							case 2: vertPos = glm::vec3(vertexQuadFar.x, vertexHeight, vertexQuadClose.z); break; // top left
							case 3: vertPos = glm::vec3(vertexQuadClose.x, vertexHeight, vertexQuadFar.z); break; // bottom right
							case 4: vertPos = glm::vec3(vertexQuadClose.x, vertexHeight, vertexQuadClose.z); break; // bottom left
							default: break;
							}

							// 1.3.4 ------------------------------------
							// Make sure there is no duplicate vertices in any grid position
							bool overlaps = false;
							if (!vertexPositions.empty())
							{
								// Check if the new vertex is within the x an z bounds of any other vertex already created
								for (glm::vec3& preExistingVertex : vertexPositions)
								{
									if (SMath::PointXZOverlapsWithVertex(preExistingVertex, vertPos))
									{
										// The new vertex is within bounds of a vertex
										// Find the average height and update the existing vertex
										float newY = (preExistingVertex.y + vertPos.y) / 2;
										preExistingVertex.y = newY;
										overlaps = true;

										// Update quad vertices for indexing 
										switch (vertexIt) {
										case 1: vert1 = preExistingVertex; break;
										case 2: vert2 = preExistingVertex; break;
										case 3: vert3 = preExistingVertex; break;
										case 4: vert4 = preExistingVertex; break;
										default: break;
										}
									}
								}
							}

							// If new vertex does not overlap with any preexisting vertices, update the quad vertices
							if (!overlaps)
							{
								switch (vertexIt) {
								case 1: vert1 = vertPos; break;
								case 2: vert2 = vertPos; break;
								case 3: vert3 = vertPos; break;
								case 4: vert4 = vertPos; break;
								default: break;
								}
							}

							// Update quad offset far for each z
							vertexQuadOffsetFar.z = vertexQuadOffsetClose.z;
						}

						// Update quad offset far for each x
						vertexQuadOffsetFar.x = vertexQuadOffsetClose.x;
					}

					// 1.4 ------------------------------------
					// Index and update each quads vertices
					// Helper function processes each quad vertex and adds its position if there is no overlap
					// Additionally it manages indexing of each quad at a time
					SMath::UpdateIndex(vertexPositions, indices, vert1, vert2, vert3, vert4);

					// Update quad far for each z
					vertexQuadFar.z = vertexQuadClose.z;
				}
				// Update quad far for each x
				vertexQuadFar.x = vertexQuadClose.x;
			}

			// Update sector far for each z
			sectorFar.z = sectorClose.z;

			// Console out progress
			if (_enableConsole)
				std::cout << "Finished Iteration X:" << i << "/" << chunkResolution << " Y:" << j << "/" << chunkResolution << "\n";
		}

		// Update sector far for each x
		sectorFar.x = sectorClose.x;
	}

	// 2. Update the calculated vertex positions into proper Vertex format for rendering
	// ----------------------------------------------------------------------------------------
	std::vector<Vertex> vertices;
	for (glm::vec3 pos : vertexPositions)
	{
		// Calculate friction value between 0.1 and 0.9 based on distance away from friction centre
		glm::vec3 diff = pos - frictionAreaCentre;
		float distanceSquared = glm::dot(diff, diff);
		float frictionCoef = 0.1f;
		if (distanceSquared <= (frictionRadius * frictionRadius))
		{
			// Normalize the distance to [0, 1] based on the friction radius
			float distance = sqrt(distanceSquared);
			float normalizedDistance = distance / frictionRadius;

			// Interpolate friction coefficient between 0.9 (center) and 0.1 (edge)
			frictionCoef = 0.9f - (normalizedDistance * 0.8f);
		}
		
		vertices.push_back(Vertex(pos, glm::vec3(0), glm::vec2(0), frictionCoef));
	}

	// 3. Update the all vertices to include normals for shading
	// ----------------------------------------------------------------------------------------
	SMath::UpdateVerticesNormal(vertices, indices);

	// 4. Add Debug functionality
	// 
	// ----------------------------------------------------------------------------------------

	// Makes the dubug actors for each section
	if (_enableDebug)
	{
		// Init root debug actor
		std::shared_ptr<DebugActor> rootDebug = std::make_shared<DebugActor>("RootDebugSector");
		AddActorToLevel(rootDebug);

		// Iterate through all stored actors and add them as children,
		// workaround for the ability to have optional debug visualization
		for (std::shared_ptr<DebugActor> debugActor : debugActors)
		{
			rootDebug->AddChild(debugActor);
		}
		debugActors.clear();
	}

	// 5. Create the object and mesh for the level
	// ----------------------------------------------------------------------------------------
	return Mesh::CreatePlane(vertices, indices, nullptr);
}

std::shared_ptr<CameraActor> LevelManager::GetActiveCamera()
{
	return mActiveLevel->mActiveCamera;
}
