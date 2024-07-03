
// Includes
#include <ModelLoader/AssimpLoader.h>
#include <ModelLoader/Shared.h>
#include <ModelLoader/AssimpUtility.h>
#include <RenderElements/Vertex.h>
#include <SceneActors.h>
#include <RenderElements/Material.h>
#include <RenderElements/Texture.h>
#include <Lights/PointLight.h>
#include <Utilities/Logger.h>

// Static Variables
std::string AssimpLoader::msBasePath = "";
long long AssimpLoader::msNameIndex = 0;
long long AssimpLoader::msFlags = 0;

void AssimpLoader::Load(const std::string& _path, std::shared_ptr<Actor> _staticMeshActor, unsigned _flags)
{
	LOG_INFO("--------------- Assimp Import Started ---------------\n");

	// uses the assimp importer object to import fbx scene with given flags
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_path, _flags);

	// throws error if no file at file path or no item in scene or flags are unreadable
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG_INFO("AssimpLoader::Load::Failed: %s", importer.GetErrorString());
		throw std::runtime_error("AssimpLoader::Load::Failed");
	}

	// sets the base path and the flags from import
	msBasePath = _path;
	msFlags = _flags;

	// Deals with al scene lights  
	ProcessLights(scene, _staticMeshActor);
	// Then deals with all scene objects
	ProcessNode(scene, scene->mRootNode, _staticMeshActor);

	LOG_INFO("--------------- Assimp Import Finished ---------------\n");
}

void AssimpLoader::ProcessLights(const aiScene* _scene, std::shared_ptr<Actor> _parentActor)
{
	LOG_INFO("AssimpLoader::ProcessingLights::NumLights: %i", _scene->mNumLights);

	for (size_t i = 0; i < _scene->mNumLights; i++)
	{
		// Skip lights that aren't pointlights for now
		if (_scene->mLights[i]->mType != aiLightSource_POINT)
			continue;

		// Create a seb point light actor
		std::shared_ptr<PointLightActor> pointLightActor = std::make_shared<PointLightActor>(_scene->mLights[i]->mName.C_Str());

		// Get the point lights world position
		aiNode* lightNode = _scene->mRootNode->FindNode(_scene->mLights[i]->mName);
		aiMatrix4x4 lightTransform = GetTransformationMatrix(lightNode);
		aiVector3D worldPosition = TransformPosition(lightTransform, _scene->mLights[i]->mPosition);

		// Position
		glm::vec3 pos = { worldPosition.x, worldPosition.y, worldPosition.z };
		// Light Ambiance
		pointLightActor->mAmbient = {_scene->mLights[i]->mColorAmbient.r,
										_scene->mLights[i]->mColorAmbient.g,
										_scene->mLights[i]->mColorAmbient.b};
		// Light color
		pointLightActor->mColor = { _scene->mLights[i]->mColorDiffuse.r,
										_scene->mLights[i]->mColorDiffuse.g,
										_scene->mLights[i]->mColorDiffuse.b};
		// Clamp color values for opengl
		pointLightActor->mColor = glm::clamp(pointLightActor->mColor, 0.f, 1.f);
		// Set the position 
		pointLightActor->SetGlobalPosition(pos);
		// Add this light as a child to scene actor object
		_parentActor->AddChild(pointLightActor);
	}
}

void AssimpLoader::ProcessNode(const aiScene* _scene, aiNode* _node, std::shared_ptr<Actor> parentActor)
{
	// Init actor object
	std::shared_ptr<Actor> actor = nullptr;

	// for each mesh in scene
	for (auto i = 0; i < _node->mNumMeshes; ++i)
	{
		// Get the mesh
		aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
		std::string collisionPrefix, lightPrefix;

		// Make a unique name for the mesh
		std::string modelName = RemoveFileExtension(GetFileNameFromPath(msBasePath));
		std::string meshName = mesh->mName.C_Str();
		std::string actorName = modelName + "_" + meshName + std::to_string(msNameIndex++);

		// If the object has the "_AABBCollision_" prefrix in its name, actor is created as an AABB collision actor
		if (HasCollisionAABBPrefix(mesh->mName.C_Str(), collisionPrefix)){

			actor = std::make_shared<CollisionActor>(actorName, ProcessMesh(mesh), CollisionProperties{ CollisionType::STATIC, CollisionResponse::BLOCK, CollisionBase::AABB });

			// otherwise if the object has the "_BoundingSphereCollision_" prefrix in its name, actor is created as a BoundingSphere collision actor
		} else if (HasCollisionBoundignSphererePrefix(mesh->mName.C_Str(), collisionPrefix)) {

			actor = std::make_shared<CollisionActor>(actorName, ProcessMesh(mesh), CollisionProperties{ CollisionType::STATIC, CollisionResponse::BLOCK, CollisionBase::BoundingSphere });

			// otherwise if the object has the "_Light_" prefrix in its name, actor is handled elsewhere and therefore ignored
		} else if (HasLightPrefix(mesh->mName.C_Str(), lightPrefix)){

			LOG("AssimpLoader::ProcessNode::LightPrefix: %s", lightPrefix.c_str());

			// otherwise the object is considered a visual only actor and is initiated as one
		} else {

			// Creates the seb mesh from the aiMesh
			std::shared_ptr<Mesh> internalMesh = ProcessMesh(mesh);

			// if mesh has a material, get it and set it to the created seb mesh
			aiMaterial* material = _scene->mMaterials[mesh->mMaterialIndex];
			if (material)
			{
				std::shared_ptr<Material> internalMaterial = ProcessMaterial(material);
				internalMesh->SetMaterial(internalMaterial);
			}

			// Sets the actor object to a visual actor with no collision
			actor = std::make_shared<VisualActor>(actorName, internalMesh);
		}
	}

	if (!actor)
	{
		// This means there is no mesh associated with the node. So a new node actor is created with a unique name.
		actor = std::make_shared<Actor>(std::string(_node->mName.C_Str()) + "_index" + std::to_string(msNameIndex++));
	}

	// Apply node transform
	actor->SetLocalTransformMatrix(AiMatrix4x4ToGlm(_node->mTransformation));

	// Add node as a child to current parent
	parentActor->AddChild(actor);

	// for each of the children do this function again recursively 
	for (auto i = 0; i < _node->mNumChildren; ++i)
		ProcessNode(_scene, _node->mChildren[i], actor);
}

std::shared_ptr<Mesh> AssimpLoader::ProcessMesh(aiMesh* _mesh)
{
	// Init vectors for vertices and indices
	std::vector<Vertex> vertices;
	std::vector<Index> indices;

	// convert aiMesh vertices to seb mesh vertices
	for (auto i = 0; i < _mesh->mNumVertices; i++)
	{
		glm::vec3 pos{ 0.f };
		pos.x = _mesh->mVertices[i].x;
		pos.y = _mesh->mVertices[i].y;
		pos.z = _mesh->mVertices[i].z;

		glm::vec3 normal{ 0.f, 1.f, 0.f };
		if (_mesh->HasNormals())
		{
			normal.x = _mesh->mNormals[i].x;
			normal.y = _mesh->mNormals[i].y;
			normal.z = _mesh->mNormals[i].z;
		}

		glm::vec2 texCoords{ 0.f };
		if (_mesh->HasTextureCoords(0))
		{
			texCoords.x = _mesh->mTextureCoords[0][i].x;
			texCoords.y = _mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back({ pos, normal, texCoords });
	}

	// convert aiMesh indices to seb mesh indices
	for (auto i = 0; i < _mesh->mNumFaces; i++)
	{
		aiFace face = _mesh->mFaces[i];
		for (auto j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Create a new mesh and return it.
	return std::make_shared<Mesh>(std::string(_mesh->mName.C_Str()), std::move(vertices), std::move(indices), nullptr);
}

std::shared_ptr<Material> AssimpLoader::ProcessMaterial(aiMaterial* _material)
{
	// Creates a new material and a default white and black texture set to diffuse and specular.
	std::shared_ptr<Material> internalMaterial = Material::Load(std::string(_material->GetName().C_Str()), {}, {});
	internalMaterial->SetTexture(Material::DIFFUSE, Texture::LoadWhiteTexture());
	internalMaterial->SetTexture(Material::SPECULAR, Texture::LoadBlackTexture());

	// Diffuse
	if (0 < _material->GetTextureCount(aiTextureType_DIFFUSE))
	{
		aiString str;
		_material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		std::string texturePath = GetDirectoryPath(msBasePath) + std::string(str.C_Str());
		internalMaterial->SetTexture(Material::DIFFUSE, Texture::Load(texturePath));
	}
	// Specular
	if (0 < _material->GetTextureCount(aiTextureType_SPECULAR))
	{
		aiString str;
		_material->GetTexture(aiTextureType_SPECULAR, 0, &str);
		std::string texturePath = GetDirectoryPath(msBasePath) +  std::string(str.C_Str());
		internalMaterial->SetTexture(Material::SPECULAR, Texture::Load(texturePath));
	}
	// Normal
	if (0 < _material->GetTextureCount(aiTextureType_NORMALS))
	{
		aiString str;
		_material->GetTexture(aiTextureType_NORMALS, 0, &str);
		std::string texturePath = GetDirectoryPath(msBasePath) +  std::string(str.C_Str());
		internalMaterial->SetTexture(Material::NORMAL, Texture::Load(texturePath));
	}
	// Opacity
	if (0 < _material->GetTextureCount(aiTextureType_OPACITY))
	{
		aiString str;
		_material->GetTexture(aiTextureType_OPACITY, 0, &str);
		std::string texturePath = GetDirectoryPath(msBasePath) +  std::string(str.C_Str());
		internalMaterial->SetTexture(Material::ALPHA, Texture::Load(texturePath));
	}

	// Create material properties
	Material::MaterialProperties materialProperties;
	aiColor3D color(0.f, 0.f, 0.f);

	// To get a material property. Inspect the AI_MATKEY_COLOR_DIFFUSE define to see other properties
	if (AI_SUCCESS == _material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		materialProperties.mColor = { color.r, color.g, color.b };

	float shininess = 64.f;
	if (_material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
		materialProperties.mShininess = shininess;
	
	// Sets the properties, replacing default with import values
	internalMaterial->SetProperties(materialProperties);

	// returns the seb material now with textures and properties correctly configured
	return internalMaterial;
}
