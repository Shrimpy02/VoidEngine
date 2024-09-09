
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

//#include <Core/SMath.h>
//#include <SkyBox/Skybox.h>

// Additional Includes
#include <ctime>

LevelManager::LevelManager(std::shared_ptr<ActorController> _inController)
	: mController(_inController)
{
	mApplicationStartTime = time(nullptr);
}

void LevelManager::LoadContent()
{
	mDefaultShader = std::make_shared<Shader>(SOURCE_DIRECTORY("shaderSrc/DefaultShader.vs"), SOURCE_DIRECTORY("shaderSrc/DefaultShader.fs"));
	mGraphShader = std::make_shared<Shader>(SOURCE_DIRECTORY("shaderSrc/GraphShader.vs"), SOURCE_DIRECTORY("shaderSrc/GraphShader.fs"));
	mDebugShader = std::make_shared<Shader>(SOURCE_DIRECTORY("shaderSrc/DebugShader.vs"), SOURCE_DIRECTORY("shaderSrc/DebugShader.fs"));
	mSkyboxShader = std::make_shared<Shader>(SOURCE_DIRECTORY("shaderSrc/SkyboxShader.vs"), SOURCE_DIRECTORY("shaderSrc/SkyboxShader.fs"));

	mAllLevels.push_back(std::make_shared<Level>("DefaultLevel"));
	SetActiveLevel(mAllLevels[0]);

	//LoadDefaultLevel();

	LoadPhysicsBoxLevel();
}

void LevelManager::LoadDefaultLevel()
{
	// Objects
	std::shared_ptr<Texture> ground1Diffuse = Texture::LoadWhiteTexture();
	std::shared_ptr<Material> ground1Mat = Material::Load("ground1Mat", { ground1Diffuse }, { {glm::vec3(1.0f,1.0f,1.0f)}, {16} });
	std::shared_ptr<VisualActor> SceneGround = std::make_shared<VisualActor>("SceneGround", Mesh::CreatePlane(ground1Mat));
	mActiveLevel->AddActorToSceneGraph(SceneGround);
	SceneGround->SetGlobalPosition(glm::vec3(0.f, -1.f, 0.f));
	SceneGround->SetGlobalRotation(glm::quat(glm::angleAxis(60.f, glm::vec3(0.f, 0.f, 1.f))));
	SceneGround->SetGlobalScale(glm::vec3(10.f));
	SceneGround->UpdateExtent();

	std::shared_ptr<Texture> cube1Diffuse = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerDiffuse.jpg"));
	std::shared_ptr<Texture> cube1Specular = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerSpecular.jpg"));
	std::shared_ptr<Material> cube1mat = Material::Load("cube1mat", { cube1Diffuse,cube1Specular }, { {glm::vec3(1.0f,1.0f,1.0f)}, {64} });
	std::shared_ptr<BaseActor> defaultCube1 = std::make_shared<BaseActor>("DefaultCube1", Mesh::CreateCube(cube1mat));
	mActiveLevel->AddActorToSceneGraph(defaultCube1);
	defaultCube1->SetGlobalPosition(glm::vec3(1.2f,0.9f,0.9f));
	defaultCube1->mCollisionProperties.SetCollisionBase(CollisionBase::BoundingSphere);
	defaultCube1->mCollisionProperties.SetCollisionType(CollisionType::DYNAMIC);
	defaultCube1->AddComponent<PhysicsComponent>("PhysicsComp");
	defaultCube1->GetPhysicsComponent()->SetSurfaceReference(SceneGround);
	defaultCube1->GetPhysicsComponent()->SetGravityEnabled(true);
	defaultCube1->AddComponent<AIComponent>("AiComp");
	//defaultCube1->SetShaderObjectType(ShaderObjectType::Default);

	std::shared_ptr<Texture> cube2Diffuse = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerDiffuse.jpg"));
	std::shared_ptr<Texture> cube2Specular = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerSpecular.jpg"));
	std::shared_ptr<Material> cube2mat = Material::Load("cube2mat", { cube2Diffuse,cube2Specular }, { {glm::vec3(1.0f,1.0f,1.0f)}, {64} });
	std::shared_ptr<BaseActor> defaultCube2 = std::make_shared<BaseActor>("DefaultCube2", Mesh::CreateCube(cube2mat));
	mActiveLevel->AddActorToSceneGraph(defaultCube2);
	defaultCube2->SetGlobalPosition(glm::vec3(1.f, 1.f,1.f));
	defaultCube2->mCollisionProperties.SetCollisionBase(CollisionBase::AABB);
	defaultCube2->mCollisionProperties.SetCollisionType(CollisionType::DYNAMIC);
	defaultCube2->mCollisionProperties.SetCollisionResponse(CollisionResponse::BLOCK);

	std::shared_ptr<GraphActor> graphActor = std::make_shared<GraphActor>("GraphActor1");
	mActiveLevel->AddActorToSceneGraph(graphActor);
	graphActor->SetGlobalPosition(glm::vec3(0.f, 5.f, 0.f));
	graphActor->SetPoints(SMath::DeCastParametricCurveFromPoints(glm::vec3(0, 0, 0), glm::vec3(3, 0, 0), glm::vec3(3, 0, 3)));
	SMath::ConformCurveToGeometry(graphActor->GetChildren(), SceneGround, 3);
	//defaultCube1->AddChild(graphActor);

	//std::shared_ptr<Actor> Model = std::make_shared<Actor>("DefaultModel");
	//AssimpLoader::Load(SOURCE_DIRECTORY("assets/Models/Ground/UneavenPlane.fbx"), Model);
	//mActiveLevel->AddActorToSceneGraph(Model);

	// Camera
	std::shared_ptr<CameraActor> cam1 = std::make_shared<CameraActor>("Camera1");
	cam1->SetGlobalPosition(glm::vec3(0, 2, 5));
	mActiveLevel->AddActorToSceneGraph(cam1);
	mActiveLevel->mActiveCamera = cam1;

	std::shared_ptr<CameraActor> cam2 = std::make_shared<CameraActor>("Camera2");
	cam2->SetGlobalPosition(glm::vec3(2, 2, 5));
	mActiveLevel->AddActorToSceneGraph(cam2);

	// Lights
	std::shared_ptr<DirectionalLightActor> dla = std::make_shared<DirectionalLightActor>("DirLight1");
	mActiveLevel->AddActorToSceneGraph(dla);
	dla->SetGlobalPosition(glm::vec3(0,10,0));
	dla->SetGlobalRotation(glm::angleAxis(glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

	mController->SetActorToControl(cam1);
	//defaultCube1->SetGlobalRotation(glm::quat( glm::angleAxis(45.f,glm::vec3(0.f,1.f,0.f))));
}

void LevelManager::LoadPhysicsBoxLevel()
{
	// Camera ------------------------
	std::shared_ptr<CameraActor> cam1 = std::make_shared<CameraActor>("Camera1");
	cam1->SetGlobalPosition(glm::vec3(0, 2, 15));
	mActiveLevel->AddActorToSceneGraph(cam1);
	mActiveLevel->mActiveCamera = cam1;

	// Controller ------------------------
	mController->SetActorToControl(cam1);

	// Light ------------------------
	std::shared_ptr<DirectionalLightActor> dla = std::make_shared<DirectionalLightActor>("DirLight1");
	mActiveLevel->AddActorToSceneGraph(dla);
	dla->SetGlobalPosition(glm::vec3(0, 10, 0));
	dla->SetGlobalRotation(glm::angleAxis(glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

	mConformBox = std::make_shared<VisualActor>("Conform box", Mesh::CreateCube(nullptr), glm::vec3(0), glm::vec3(10.f));
	mActiveLevel->AddActorToSceneGraph(mConformBox);
	mConformBox->mVisualMesh->SetIsVisible(false);

	std::shared_ptr<DebugActor> debugConformSquare = std::make_shared<DebugActor>("ConformDebug");
	debugConformSquare->UpdateVisualMesh(mConformBox->GetMesh());
	mConformBox->AddChild(debugConformSquare);

	// Objects ------------------------
	// Since instancing is enabled  we can initalize the material once and assign it to the first of the instance
	std::shared_ptr<Texture> diffuseTex = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerDiffuse.jpg"));
	std::shared_ptr<Texture> specularTex = Texture::Load(SOURCE_DIRECTORY("UserAssets/Textures/Container/ContainerSpecular.jpg"));
	std::shared_ptr<Material> mat = Material::Load("cube1mat", { diffuseTex, specularTex }, { {glm::vec3(1.0f,1.0f,1.0f)}, {64} });

	std::vector<std::shared_ptr<BaseActor>> temp;
	int numSpheres = 1000;
	for(int i = 0; i < numSpheres;i++)
	{
		std::shared_ptr<BaseActor> obj = std::make_shared<BaseActor>("Sphere", Mesh::CreateSphere(mat, 2, true), CollisionBase::BoundingSphere, glm::vec3(0), glm::vec3(0.3f));
		mActiveLevel->AddActorToSceneGraph(obj);
		obj->AddComponent<PhysicsComponent>("PhysicsComp");
		//obj->SetGlobalPosition(glm::vec3(i * 2,0,0));
		obj->GetPhysicsComponent()->SetGravityEnabled(false);
		obj->SetCollisionType(CollisionType::DYNAMIC);
		obj->SetCollisionResponse(CollisionResponse::BLOCK);
		temp.push_back(obj);
	}
	SSpawner::SetAllObjectLocationWithinBoundsRandomly(temp,mConformBox);

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
	//UpdateLevelSceneGraph(mActiveLevel->mSceneGraph, _dt);

	// Handle collision within bounding box
	//if(mConformBox)
	//	ProcessCollisionWithinBoxBounds(mConformBox);
	
	// Then handle collision for all objects in scene
	//ProcessCollision();

	// Handels lifetime of tempDebug actors
	mActiveLevel->TempDebugTimerManager(difftime(time(0), mApplicationStartTime));
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
	BindCamera(mSkyboxShader);

	RenderLevelSceneGraph(mActiveLevel->mSceneGraph);

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
			if(actorColliderA->IsIntersecting(actorColliderB, &mtv))
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

void LevelManager::ProcessCollisionWithinBoxBounds(std::shared_ptr<VisualActor> _conformBox)
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
		if (actorColliderA->IsIntersectingConstrictingBoxGeometry(_conformBox, &mtv))
		{
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
	_globalTransform.SetTransformMatrix(_globalTransform.GetTransformMatrix() * _actor->GetGlobalTransformMatrix());

	// Cast to actor to se if they inherit from IRender,
	// if they do call their inherited draw function and bind the model matrix
	if (std::shared_ptr<IRender> iRender = std::dynamic_pointer_cast<IRender>(_actor))
	{
		if(iRender->GetShaderObjectType() == ShaderObjectType::Default){

			mDefaultShader->use();
			mDefaultShader->setMat4("model", _globalTransform.GetTransformMatrix());
			iRender->Draw(mDefaultShader);

		} else if(iRender->GetShaderObjectType() == ShaderObjectType::Graph) {

			mGraphShader->use();
			mGraphShader->setMat4("model", _globalTransform.GetTransformMatrix());
			iRender->Draw(mGraphShader);

		} else if (iRender->GetShaderObjectType() == ShaderObjectType::Debug) {

			mDebugShader->use();
			mDebugShader->setMat4("model", _globalTransform.GetTransformMatrix());
			iRender->Draw(mDebugShader);

		} else if (iRender->GetShaderObjectType() == ShaderObjectType::Skybox) {

			mSkyboxShader->use();
			mDebugShader->setMat4("model", _globalTransform.GetTransformMatrix());
			iRender->Draw(mSkyboxShader);
		}
	}

	// For each child recursively run this function
	const auto& children = _actor->GetChildren();
	for (std::shared_ptr<Actor> child : children)
	{
		RenderLevelSceneGraph(child, _globalTransform);
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
	std::shared_ptr<DebugActor> actor = std::make_shared<DebugActor>("LineTrace", true);
	actor->UpdateVisualMesh(_points);
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
