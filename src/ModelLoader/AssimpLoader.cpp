
#include <ModelLoader/AssimpLoader.h>
#include <ModelLoader/Shared.h>
#include <ModelLoader/AssimpUtility.h>
#include <Vertex.h>
#include <SceneActors.h>
#include <Material.h>
#include <Texture.h>
#include <exception>
#include <Lights/PointLight.h>

#include <iostream>

std::string AssimpLoader::msBasePath = "";
long long AssimpLoader::msNameIndex = 0;
long long AssimpLoader::msFlags = 0;

void AssimpLoader::Load(const std::string& _path, Actor* _staticMeshActor, unsigned _flags)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_path, _flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		//LOG_INFO("AssimpLoader::Load::Failed: %s", importer.GetErrorString());
		throw std::runtime_error("AssimpLoader::Load::Failed");
	}

	msBasePath = _path;
	msFlags = _flags;
	ProcessLights(scene, _staticMeshActor);
	ProcessNode(scene, scene->mRootNode, _staticMeshActor);
}

void AssimpLoader::ProcessLights(const aiScene* _scene, Actor* _parentActor)
{
	//LOG_INFO("AssimpLoader::ProcessLights::NumLights: %i", scene->mNumLights);
	for (size_t i = 0; i < _scene->mNumLights; i++)
	{
		// Skip lights that aren't pointlights
		if (_scene->mLights[i]->mType != aiLightSource_POINT)
		{
			continue;
		}

		PointLightActor* pointLightActor = new PointLightActor(_scene->mLights[i]->mName.C_Str());

		// Transform the pointlight to its world position
		aiNode* lightNode = _scene->mRootNode->FindNode(_scene->mLights[i]->mName);
		aiMatrix4x4 lightTransform = GetTransformationMatrix(lightNode);
		aiVector3D worldPosition = TransformPosition(lightTransform, _scene->mLights[i]->mPosition);

		glm::vec3 pos =
		{
			worldPosition.x,
			worldPosition.y,
			worldPosition.z
		};
		pointLightActor->mAmbient =
		{
			_scene->mLights[i]->mColorAmbient.r,
			_scene->mLights[i]->mColorAmbient.g,
			_scene->mLights[i]->mColorAmbient.b
		};
		pointLightActor->mColor =
		{
			_scene->mLights[i]->mColorDiffuse.r,
			_scene->mLights[i]->mColorDiffuse.g,
			_scene->mLights[i]->mColorDiffuse.b
		};

		pointLightActor->mColor = glm::clamp(pointLightActor->mColor, 0.f, 1.f);
		pointLightActor->SetPosition(pos,Actor::TransformSpace::Global);
		_parentActor->AddChild(pointLightActor);
	}
}

void AssimpLoader::ProcessNode(const aiScene* _scene, aiNode* _node, Actor* parentActor)
{
	Actor* actor = nullptr;

	for (auto i = 0; i < _node->mNumMeshes; ++i)
	{
		aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
		std::string collisionPrefix, lightPrefix;

		// Make a unique name for the mesh
		std::string modelName = RemoveFileExtension(GetFileNameFromPath(msBasePath));
		std::string meshName = mesh->mName.C_Str();
		std::string actorName = modelName + "_" + meshName + std::to_string(msNameIndex++);

		if (HasCollisionPrefix(mesh->mName.C_Str(), collisionPrefix))
		{
			//AABB aabb = ProcessCollisionAABB(mesh, actorName);
			//actor = new AABBActor(actorName, ProcessMesh(mesh), aabb);
			//actor = ProcessCollisionAABB(mesh, actorName);
		}
		else if (HasLightPrefix(mesh->mName.C_Str(), lightPrefix))
		{
			//LOG_INFO("AssimpLoader::ProcessNode::LightPrefix: %s", lightPrefix.c_str());
		}
		else
		{
			//LOG_INFO("AssimpLoader::ProcessMesh::Name: %s", mesh->mName.C_Str());
			Mesh* internalMesh = ProcessMesh(mesh);

			aiMaterial* material = _scene->mMaterials[mesh->mMaterialIndex];

			//LOG_INFO("AssimpLoader::ProcessMaterial::Name: %s", material->GetName().C_Str());
			if (material)
			{
				Material* internalMaterial = ProcessMaterial(material);
				internalMesh->SetMaterial(internalMaterial);
			}

			BaseActor* meshActor = new BaseActor(actorName, internalMesh);
			// Set the collision properties for the actor to ignore. This should be set to BLOCK when culling the scene in a different collision channel.
			meshActor->mCollisionProperties.mResponse = CollisionResponse::IGNORE;
			actor = meshActor;
		}
	}

	if (!actor)
	{
		// This means there is no mesh associated with the node. So a new node actor is created with a unique name.
		actor = new Actor(std::string(_node->mName.C_Str()) + "_index" + std::to_string(msNameIndex++));
	}

	// Apply node transform
	actor->SetLocalTransformMatrix(AiMatrix4x4ToGlm(_node->mTransformation));

	// Add node as a child
	parentActor->AddChild(actor);

	for (auto i = 0; i < _node->mNumChildren; ++i)
	{
		ProcessNode(_scene, _node->mChildren[i], actor);
	}

	//LOG_INFO("----------------------------------------------------\n");
}

Mesh* AssimpLoader::ProcessMesh(aiMesh* _mesh)
{
	std::vector<Vertex> vertices;
	std::vector<Index> indices;

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

	for (auto i = 0; i < _mesh->mNumFaces; i++)
	{
		aiFace face = _mesh->mFaces[i];
		for (auto j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	return new Mesh(std::string(_mesh->mName.C_Str()), std::move(vertices), std::move(indices), nullptr);
}

Material* AssimpLoader::ProcessMaterial(aiMaterial* _material)
{
	// Creates a new material and a default white and black texture set to diffuse and specular.
	Material* internalMaterial = Material::Load(std::string(_material->GetName().C_Str()), {}, {});
	internalMaterial->SetTexture(Material::DIFFUSE, Texture::LoadWhiteTexture());
	internalMaterial->SetTexture(Material::SPECULAR, Texture::LoadBlackTexture());

	// Diffuse
	if (0 < _material->GetTextureCount(aiTextureType_DIFFUSE))
	{
		aiString str;
		_material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		std::string texturePath = GetDirectoryPath(msBasePath) + std::string(str.C_Str());
		internalMaterial->SetTexture(Material::DIFFUSE, Texture::Load(texturePath));
		//LOG_INFO("AssimpLoader::ProcessMaterial::Texture::Diffuse::Path: %s", texturePath.c_str());
	}
	// Specular
	if (0 < _material->GetTextureCount(aiTextureType_SPECULAR))
	{
		aiString str;
		_material->GetTexture(aiTextureType_SPECULAR, 0, &str);
		std::string texturePath = GetDirectoryPath(msBasePath) +  std::string(str.C_Str());
		internalMaterial->SetTexture(Material::SPECULAR, Texture::Load(texturePath));
		//LOG_INFO("AssimpLoader::ProcessMaterial::Texture::Specular::Path: %s", texturePath.c_str());
	}
	// Normal
	if (0 < _material->GetTextureCount(aiTextureType_NORMALS))
	{
		aiString str;
		_material->GetTexture(aiTextureType_NORMALS, 0, &str);
		std::string texturePath = GetDirectoryPath(msBasePath) +  std::string(str.C_Str());
		internalMaterial->SetTexture(Material::NORMAL, Texture::Load(texturePath));
		//LOG_INFO("AssimpLoader::ProcessMaterial::Texture::Normal::Path: %s", texturePath.c_str());
	}
	// Opacity
	if (0 < _material->GetTextureCount(aiTextureType_OPACITY))
	{
		aiString str;
		_material->GetTexture(aiTextureType_OPACITY, 0, &str);
		std::string texturePath = GetDirectoryPath(msBasePath) +  std::string(str.C_Str());
		internalMaterial->SetTexture(Material::ALPHA, Texture::Load(texturePath));
		//LOG_INFO("AssimpLoader::ProcessMaterial::Texture::Opacity::Path: %s", texturePath.c_str());
	}


	Material::MaterialProperties materialProperties;
	aiColor3D color(0.f, 0.f, 0.f);

	// To get a material property. Inspect the AI_MATKEY_COLOR_DIFFUSE define to see other properties
	if (AI_SUCCESS == _material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
	{
		materialProperties.mColor = { color.r, color.g, color.b };
	}

	float shininess = 64.f;
	if (_material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
	{
		materialProperties.mShininess = shininess;
	}

	internalMaterial->SetProperties(materialProperties);

	return internalMaterial;
}

AABBActor* AssimpLoader::ProcessCollisionAABB(aiMesh* _mesh, std::string _name)
{
	glm::vec3 maxExtent(0);
	glm::vec3 minExtent(0);
	glm::vec3 center(0);

	//for (auto j = 0; j < _mesh->mNumVertices; j++)
	//{
	//	// X
	//	if (_mesh->mVertices[j].x > maxExtent.x)
	//		maxExtent.x = _mesh->mVertices[j].x;

	//	else if (_mesh->mVertices[j].x < minExtent.x)
	//		minExtent.x = _mesh->mVertices[j].x;

	//	// Y
	//	if (_mesh->mVertices[j].y > maxExtent.y)
	//		maxExtent.y = _mesh->mVertices[j].y;

	//	else if (_mesh->mVertices[j].y < minExtent.y)
	//		minExtent.y = _mesh->mVertices[j].y;

	//	// Z
	//	if (_mesh->mVertices[j].z > maxExtent.z)
	//		maxExtent.z = _mesh->mVertices[j].z;

	//	else if (_mesh->mVertices[j].z < minExtent.z)
	//		minExtent.z = _mesh->mVertices[j].z;

	//	glm::vec3 pos = glm::vec3(_mesh->mVertices[j].x, _mesh->mVertices[j].y, _mesh->mVertices[j].z);
	//	center += pos;
	//}
	//AABB aabb{ {},{} };
	//for (auto i = 0; i < _mesh->mNumVertices; i++)
	//{
	//	glm::vec3 pos{ 0.f };
	//	pos.x = _mesh->mVertices[i].x;
	//	pos.y = _mesh->mVertices[i].y;
	//	pos.z = _mesh->mVertices[i].z;

	//	aabb.center += pos;
	//}
	//aabb.center /= _mesh->mNumVertices;

	//for (auto i = 0; i < _mesh->mNumVertices; i++)
	//{
	//	glm::vec3 pos{ 0.f };
	//	pos.x = _mesh->mVertices[i].x;
	//	pos.y = _mesh->mVertices[i].y;
	//	pos.z = _mesh->mVertices[i].z;

	//	//LOG_INFO("aabb pos: (%f, %f, %f)", pos.x, pos.y, pos.z);
	//	// Expands the AABB to fit the vertices
	//	aabb.Expand(pos);
	//}

	//return aabb;

	for (auto j = 0; j < _mesh->mNumVertices; j++)
	{
		// X
		if (_mesh->mVertices[j].x > maxExtent.x)
			maxExtent.x = _mesh->mVertices[j].x;

		else if (_mesh->mVertices[j].x < minExtent.x)
			minExtent.x = _mesh->mVertices[j].x;

		// Swap Z and Y for import from blender
		// Y
		if (_mesh->mVertices[j].y > maxExtent.z)
			maxExtent.z = _mesh->mVertices[j].y;

		else if (_mesh->mVertices[j].y < minExtent.z)
			minExtent.z = _mesh->mVertices[j].y;

		// Z
		if (_mesh->mVertices[j].z > maxExtent.y)
			maxExtent.y = _mesh->mVertices[j].z;

		else if (_mesh->mVertices[j].z < minExtent.y)
			minExtent.y = _mesh->mVertices[j].z;

		glm::vec3 pos = glm::vec3(_mesh->mVertices[j].x, _mesh->mVertices[j].z, _mesh->mVertices[j].y);
		center += pos;
	}

	center /= static_cast<float>(_mesh->mNumVertices);

	return nullptr;

	//return new AABBActor(_name, ProcessMesh(_mesh), maxExtent, minExtent, center);
}
