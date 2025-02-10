#include "tripch.h"
#include "AssimpImporter.h"

#include <Tridium/Graphics/Rendering/Buffer.h>
#include <Tridium/Graphics/Rendering/VertexArray.h>
#include <Tridium/Graphics/Rendering/Texture.h>
#include <Tridium/Graphics/Rendering/Material.h>
#include <Tridium/Graphics/Rendering/Mesh.h>

#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Asset/Loaders/TextureLoader.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"

namespace Tridium {

	namespace Util {

		Matrix4 Mat4FromAIMatrix4x4( const aiMatrix4x4& matrix )
		{
			Matrix4 result;
			result[0][0] = (float)matrix.a1; result[0][1] = (float)matrix.b1;  result[0][2] = (float)matrix.c1; result[0][3] = (float)matrix.d1;
			result[1][0] = (float)matrix.a2; result[1][1] = (float)matrix.b2;  result[1][2] = (float)matrix.c2; result[1][3] = (float)matrix.d2;
			result[2][0] = (float)matrix.a3; result[2][1] = (float)matrix.b3;  result[2][2] = (float)matrix.c3; result[2][3] = (float)matrix.d3;
			result[3][0] = (float)matrix.a4; result[3][1] = (float)matrix.b4;  result[3][2] = (float)matrix.c4; result[3][3] = (float)matrix.d4;
			return result;
		}
	}

	
	static const uint32_t s_AssimpImportFlags =
		aiProcess_CalcTangentSpace          
		| aiProcess_Triangulate             
		| aiProcess_SortByPType             
		| aiProcess_GenSmoothNormals
		| aiProcess_GenUVCoords
		| aiProcess_OptimizeGraph
		| aiProcess_OptimizeMeshes          
		| aiProcess_JoinIdenticalVertices
		| aiProcess_LimitBoneWeights        
		| aiProcess_ValidateDataStructure   
		| aiProcess_GlobalScale
		;

	AssimpImporter::AssimpImporter( const FilePath& a_FilePath )
		: m_FilePath( a_FilePath )
	{
	}

	SharedPtr<MeshSource> AssimpImporter::ImportMeshSource( const MeshSourceImportSettings& a_ImportSettings )
	{
		SharedPtr<MeshSource> meshSource = MakeShared<MeshSource>();

		Assimp::Importer importer;
		importer.SetPropertyFloat( AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, a_ImportSettings.Scale );

		const aiScene* scene = importer.ReadFile( m_FilePath.ToString().c_str(), s_AssimpImportFlags);
		if ( !scene )
		{
			LOG( LogCategory::Asset, Error, "[AssimpImporter] Failed to load mesh source from: {0}", m_FilePath.ToString() );
			return nullptr;
		}

		ProcessNode( meshSource, (void*)scene, scene->mRootNode, Matrix4( 1.0f ) );
		ExtractMaterials( (void*)scene, meshSource );

		// Calculate bounding box for the mesh source
		for ( const auto& submesh : meshSource->m_SubMeshes )
		{
			// Convert submesh bounding box to world space
			AABB submeshAABB = submesh.BoundingBox.Transform( submesh.Transform );
			meshSource->m_BoundingBox.Expand( submeshAABB );
		}

		return meshSource;
	}

	void AssimpImporter::ProcessNode( SharedPtr<MeshSource>& a_MeshSource, const void* a_AssimpScene, void* a_AssimpNode, const Matrix4& a_ParentTransform )
	{
		const aiScene* a_Scene = static_cast<const aiScene*>( a_AssimpScene );
		const aiNode* a_Node = static_cast<aiNode*>( a_AssimpNode );

		Matrix4 localTransform = Util::Mat4FromAIMatrix4x4( a_Node->mTransformation );
		Matrix4 transform = a_ParentTransform * localTransform;

		// Process submeshes
		for ( uint32_t i = 0; i < a_Node->mNumMeshes; i++ )
		{
			uint32_t submeshIndex = a_Node->mMeshes[i];
			SubMesh submesh = ProcessSubMesh( a_MeshSource, a_Scene, a_Scene->mMeshes[submeshIndex] );
			submesh.Name = a_Node->mName.C_Str();
			submesh.Transform = transform;
			submesh.LocalTransform = localTransform;
			submesh.GenerateMeshCollider();

			a_MeshSource->m_SubMeshes.emplace_back( std::move( submesh ) );
		}

		// Recurse
		// Process children
		for ( uint32_t i = 0; i < a_Node->mNumChildren; i++ )
		{
			ProcessNode( a_MeshSource, a_Scene, a_Node->mChildren[i], transform );
		}
	}

	SubMesh AssimpImporter::ProcessSubMesh( SharedPtr<MeshSource>& a_MeshSource, const void* a_AssimpScene, void* a_AssimpMesh )
	{
		const aiScene* a_Scene = static_cast<const aiScene*>( a_AssimpScene );
		const aiMesh* a_Mesh = static_cast<aiMesh*>( a_AssimpMesh );

		SubMesh submesh;

		submesh.MaterialIndex = a_Mesh->mMaterialIndex;

		// Process Vertices
		for ( uint32_t i = 0; i < a_Mesh->mNumVertices; ++i )
		{
			Vertex& vertex = submesh.Vertices.emplace_back();
			vertex.Position = { a_Mesh->mVertices[i].x, a_Mesh->mVertices[i].y, a_Mesh->mVertices[i].z };
			vertex.Normal = { a_Mesh->mNormals[i].x, a_Mesh->mNormals[i].y, a_Mesh->mNormals[i].z };

			if ( a_Mesh->HasTangentsAndBitangents() )
			{
				vertex.Tangent = { a_Mesh->mTangents[i].x, a_Mesh->mTangents[i].y, a_Mesh->mTangents[i].z };
				vertex.Bitangent = { a_Mesh->mBitangents[i].x, a_Mesh->mBitangents[i].y, a_Mesh->mBitangents[i].z };
			}

			// Only support one set of UVs ( for now? )
			if ( a_Mesh->HasTextureCoords( 0 ) )
			{
				vertex.UV = { a_Mesh->mTextureCoords[0][i].x, a_Mesh->mTextureCoords[0][i].y };
			}

			// Update bounding box
			submesh.BoundingBox.Expand( vertex.Position );
		}

		// Process Indices
		for ( uint32_t i = 0; i < a_Mesh->mNumFaces; ++i )
		{
			const aiFace& face = a_Mesh->mFaces[i];
			ASSERT_LOG( face.mNumIndices == 3, "Face is not a triangle" );

			submesh.Indices.push_back( face.mIndices[0] );
			submesh.Indices.push_back( face.mIndices[1] );
			submesh.Indices.push_back( face.mIndices[2] );
		}

		CalculateTangents( submesh.Vertices, submesh.Indices );

		submesh.VAO = VertexArray::Create();
		BufferLayout layout =
		{
			{ EShaderDataType::Float3, "a_Position" },
			{ EShaderDataType::Float3, "a_Normal" },
			{ EShaderDataType::Float3, "a_Tangent" },
			{ EShaderDataType::Float3, "a_Bitangent" },
			{ EShaderDataType::Float2, "a_UV" },
		};

		submesh.VBO = VertexBuffer::Create( (float*)( submesh.Vertices.data() ), (uint32_t)( submesh.Vertices.size() * sizeof( Vertex ) ) );
		submesh.VBO->SetLayout( layout );
		submesh.VAO->AddVertexBuffer( submesh.VBO );

		submesh.IBO = IndexBuffer::Create( submesh.Indices.data(), (uint32_t)( submesh.Indices.size() ) );
		submesh.VAO->SetIndexBuffer( submesh.IBO );

		return submesh;
	}

	void AssimpImporter::ExtractMaterials( void* a_AssimpScene, SharedPtr<MeshSource>& a_MeshSource )
	{
		const aiScene* a_Scene = static_cast<aiScene*>( a_AssimpScene );
		if ( !a_Scene->HasMaterials() )
			return;

		a_MeshSource->m_Materials.resize( a_Scene->mNumMaterials );

		for ( uint32_t i = 0; i < a_Scene->mNumMaterials; i++ )
		{
			aiMaterial* aiMat = a_Scene->mMaterials[i];

			auto material = MakeShared<Material>();

			// Albedo
			material->AlbedoTexture = ExtractTexture( (void*)a_Scene, aiMat, aiTextureType_DIFFUSE );
			// Normal
			material->NormalTexture = ExtractTexture( (void*)a_Scene, aiMat, aiTextureType_NORMALS );
			// Metallic
			//material->MetallicTexture = ExtractTexture( (void*)a_Scene, aiMat, aiTextureType_SPECULAR );
			// Roughness
			//material->RoughnessTexture = ExtractTexture( (void*)a_Scene, aiMat, aiTextureType_SHININESS );
			// Opacity
			material->OpacityTexture = ExtractTexture( (void*)a_Scene, aiMat, aiTextureType_OPACITY );
			// Ambient Occlusion
			material->AOTexture = ExtractTexture( (void*)a_Scene, aiMat, aiTextureType_AMBIENT );
			// Emissive
			material->EmissiveTexture = ExtractTexture( (void*)a_Scene, aiMat, aiTextureType_EMISSIVE );


			AssetHandle matAssetHandle = AssetHandle::Create();
			AssetManager::AddMemoryOnlyAsset( matAssetHandle, material );
			a_MeshSource->m_Materials[i] = matAssetHandle;
		}
	}

	TextureHandle AssimpImporter::ExtractTexture( void* a_AssimpScene, void* a_AssimpMaterial, int a_AssimpTextureType )
	{
		SharedPtr<TextureLoader> textureLoader = SharedPtrCast<TextureLoader>( AssetFactory::GetAssetLoader( EAssetType::Texture ) );
		aiScene* aiScn = static_cast<aiScene*>( a_AssimpScene );
		aiMaterial* aiMat = static_cast<aiMaterial*>( a_AssimpMaterial );
		aiTextureType aiTexType = static_cast<aiTextureType>( a_AssimpTextureType );
		aiString aiTexturePath;

		if ( aiMat->GetTexture( aiTexType, 0, &aiTexturePath ) == AI_FAILURE )
			return AssetHandle::InvalidID;

		TextureHandle texHandle = AssetHandle::Create();

		if ( auto aiEmbeddedTexture = aiScn->GetEmbeddedTexture( aiTexturePath.C_Str() ) )
		{
			TextureSpecification spec;
			spec.TextureFormat = ETextureFormat::RGBA32F;
			spec.Width = aiEmbeddedTexture->mWidth;
			spec.Height = aiEmbeddedTexture->mHeight;
			AssetManager::AddMemoryOnlyAsset( texHandle, SharedPtr<Texture>( Texture::Create( spec, aiEmbeddedTexture->pcData ) ) );
			return texHandle;
		}
		else
		{
			auto parentPath = m_FilePath.GetParentPath();
			auto texturePath = parentPath / aiTexturePath.C_Str();
			if ( !texturePath.Exists() )
				texturePath = parentPath / texturePath.GetFilename();

			if ( !texturePath.Exists() )
				return TextureHandle::InvalidID;

			if ( SharedPtr<Texture> texture = textureLoader->LoadTexture( texturePath ) )
			{
				AssetManager::AddMemoryOnlyAsset( texHandle, texture );
				return texHandle;
			}
		}

		return TextureHandle::InvalidID;
	}
}