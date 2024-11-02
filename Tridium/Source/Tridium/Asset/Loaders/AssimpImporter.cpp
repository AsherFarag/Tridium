#include "tripch.h"
#include "AssimpImporter.h"

#include <Tridium/Rendering/Buffer.h>
#include <Tridium/Rendering/VertexArray.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Material.h>
#include <Tridium/Rendering/Mesh.h>

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

	AssimpImporter::AssimpImporter( const IO::FilePath& a_FilePath )
		: m_FilePath( a_FilePath )
	{
	}

	SharedPtr<MeshSource> AssimpImporter::ImportMeshSource( const MeshSourceImportSettings& a_ImportSettings )
	{
		SharedPtr<MeshSource> meshSource = MakeShared<MeshSource>();

		Assimp::Importer importer;
		//importer.SetPropertyBool( AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false );
		importer.SetPropertyFloat( AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, a_ImportSettings.Scale );

		const aiScene* scene = importer.ReadFile( m_FilePath.ToString().c_str(), s_AssimpImportFlags);
		if ( !scene )
		{
			TE_CORE_ERROR( "[AssimpImporter] Failed to load mesh source from: {0}", m_FilePath.ToString() );
			return nullptr;
		}

		for ( uint32_t i = 0; i < scene->mNumMeshes; i++ )
		{
			SubMesh submesh = ProcessSubMesh( meshSource, scene, scene->mMeshes[i] );
			submesh.Name = scene->mMeshes[i]->mName.C_Str();
			submesh.Transform = Matrix4( 1.0f );
			submesh.LocalTransform = Matrix4( 1.0f );

			meshSource->m_SubMeshes.push_back( submesh );
		}

		//ProcessNode( meshSource, (void*)scene, scene->mRootNode, Matrix4( 1.0f ) );

		//ExtractMaterials( (void*)scene, meshSource );

		// Create GPU buffers

		meshSource->m_VAO = VertexArray::Create();

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Bitangent" },
			{ ShaderDataType::Float2, "a_UV" },
		};
		
		meshSource->m_VBO = VertexBuffer::Create( (float*)( meshSource->m_Vertices.data() ), (uint32_t)( meshSource->m_Vertices.size() * sizeof( Vertex ) ) );
		meshSource->m_VBO->SetLayout( layout );
		meshSource->m_VAO->AddVertexBuffer( meshSource->m_VBO );

		meshSource->m_IBO = IndexBuffer::Create( meshSource->m_Indices.data(), (uint32_t)( meshSource->m_Indices.size() ) );
		meshSource->m_VAO->SetIndexBuffer( meshSource->m_IBO );

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

			a_MeshSource->m_SubMeshes.push_back( submesh );
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

		// Process Vertices
		for ( uint32_t i = 0; i < a_Mesh->mNumVertices; ++i )
		{
			Vertex vertex;
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

			a_MeshSource->m_Vertices.push_back( vertex );
		}

		// Process Indices
		for ( uint32_t i = 0; i < a_Mesh->mNumFaces; ++i )
		{
			const aiFace& face = a_Mesh->mFaces[i];
			TE_CORE_ASSERT( face.mNumIndices == 3, "Face is not a triangle" );
			a_MeshSource->m_Indices.push_back( face.mIndices[0] );
			a_MeshSource->m_Indices.push_back( face.mIndices[1] );
			a_MeshSource->m_Indices.push_back( face.mIndices[2] );
		}

		submesh.BaseVertex = (uint32_t)a_MeshSource->m_Vertices.size() - a_Mesh->mNumVertices;
		submesh.BaseIndex = (uint32_t)a_MeshSource->m_Indices.size() - ( a_Mesh->mNumFaces * 3 );
		submesh.MaterialIndex = a_Mesh->mMaterialIndex;
		submesh.NumVertices = a_Mesh->mNumVertices;
		submesh.NumIndicies = a_Mesh->mNumFaces * 3;

		return submesh;
	}

#if 0

	MeshSource::MeshNode* AssimpImporter::ProcessNode( void* a_AssimpScene, void* a_AssimpNode, SharedPtr<MeshSource>& a_MeshSource, uint32_t a_ParentNodeIndex, const Matrix4& a_ParentTransform )
	{
		const aiScene* a_Scene = static_cast<aiScene*>( a_AssimpScene );
		const aiNode* a_Node = static_cast<aiNode*>( a_AssimpNode );

		MeshSource::MeshNode node;
		node.Name = a_Node->mName.C_Str();
		node.LocalTransform = Util::Mat4FromAIMatrix4x4( a_Node->mTransformation );

		// Process submeshes
		Matrix4 transform = a_ParentTransform * node.LocalTransform;
		for ( uint32_t i = 0; i < a_Node->mNumMeshes; i++ )
		{
			uint32_t submeshIndex = a_Node->mMeshes[i];
			SubMesh submesh = ProcessSubMesh( a_AssimpScene, a_Scene->mMeshes[submeshIndex], a_MeshSource );
			submesh.Name = a_Node->mName.C_Str();
			submesh.Transform = transform;
			submesh.LocalTransform = node.LocalTransform;

			a_MeshSource->m_SubMeshes.push_back( submesh );
		}

		// Recurse
		// Add children
		uint32_t parentNodeIndex = (uint32_t)a_MeshSource->m_MeshNodes.size() - 1;
		node.Children.resize( a_Node->mNumChildren );
		for ( uint32_t i = 0; i < a_Node->mNumChildren; i++ )
		{
			MeshSource::MeshNode* child = ProcessNode( a_AssimpScene, a_Node->mChildren[i], a_MeshSource, parentNodeIndex, transform );
			uint32_t childIndex = static_cast<uint32_t>( a_MeshSource->m_MeshNodes.size() ) - 1;
			child->Parent = parentNodeIndex;
			a_MeshSource->m_MeshNodes[parentNodeIndex].Children[i] = childIndex;
		}

		return &a_MeshSource->m_MeshNodes.emplace_back( node );

	}

	SubMesh AssimpImporter::ProcessSubMesh( void* a_AssimpScene, void* a_AssimpMesh, SharedPtr<MeshSource>& a_MeshSource )
	{
		const aiScene* a_Scene = static_cast<aiScene*>( a_AssimpScene );
		const aiMesh* a_Mesh = static_cast<aiMesh*>( a_AssimpMesh );

		// Process Vertices
		for ( uint32_t i = 0; i < a_Mesh->mNumVertices; ++i )
		{
			Vertex vertex;
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

			a_MeshSource->m_Vertices.push_back( vertex );
		}

		// Process Indices
		for ( uint32_t i = 0; i < a_Mesh->mNumFaces; ++i )
		{
			const aiFace& face = a_Mesh->mFaces[i];
			TE_CORE_ASSERT( face.mNumIndices == 3, "Face is not a triangle" );
			a_MeshSource->m_Indices.push_back( face.mIndices[0] );
			a_MeshSource->m_Indices.push_back( face.mIndices[1] );
			a_MeshSource->m_Indices.push_back( face.mIndices[2] );
		}

		TODO( "Transforms" );
		SubMesh submesh =
		{
			.BaseVertex = (uint32_t)a_MeshSource->m_Vertices.size() - a_Mesh->mNumVertices,
			.BaseIndex = (uint32_t)a_MeshSource->m_Indices.size() - ( a_Mesh->mNumFaces * 3 ),
			.MaterialIndex = a_Mesh->mMaterialIndex,
			.NumVertices = a_Mesh->mNumVertices,
			.NumIndicies = a_Mesh->mNumFaces * 3,
			.Transform = Matrix4( 1.0f ),
			.LocalTransform = Matrix4( 1.0f ),
			.Name = a_Mesh->mName.C_Str()
		};

		return submesh;
	}

	void AssimpImporter::ExtractSubmeshes( void* a_AssimpScene, SharedPtr<MeshSource>& a_MeshSource )
	{
		const aiScene* a_Scene = static_cast<aiScene*>( a_AssimpScene );
		if ( !a_Scene->HasMeshes() )
			return;

		uint32_t numVertices = 0;
		uint32_t numIndices = 0;

		a_MeshSource->m_SubMeshes.reserve( a_Scene->mNumMeshes );

		for ( uint32_t meshIndex = 0; meshIndex < a_Scene->mNumMeshes; ++meshIndex )
		{
			const aiMesh* mesh = a_Scene->mMeshes[meshIndex];

			if ( !mesh->HasPositions() )
			{
				TE_CORE_WARN( "[AssimpImporter] Mesh Index {0} with name {1} has no positions", meshIndex, mesh->mName.C_Str() );
			}
			if ( !mesh->HasNormals() )
			{
				TE_CORE_WARN( "[AssimpImporter] Mesh Index {0} with name {1} has no normals", meshIndex, mesh->mName.C_Str() );
			}

			bool skip = !mesh->HasPositions() || !mesh->HasNormals();

			SubMesh& submesh = a_MeshSource->m_SubMeshes.emplace_back();
			submesh.BaseVertex = numVertices;
			submesh.BaseIndex = numIndices;
			submesh.MaterialIndex = mesh->mMaterialIndex;
			submesh.NumVertices = skip ? 0 : mesh->mNumVertices;
			submesh.NumIndicies = skip ? 0 : mesh->mNumFaces * 3;
			submesh.Name = mesh->mName.C_Str();

			if ( skip ) continue;

			numVertices += mesh->mNumVertices;
			numIndices += mesh->mNumFaces * 3;

			// Handle Vertices
			for ( uint32_t i = 0; i < mesh->mNumVertices; ++i )
			{
				Vertex vertex;
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

				if ( mesh->HasTangentsAndBitangents() )
				{
					vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
					vertex.Bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
				}

				// Only support one set of UVs ( for now? )
				if ( mesh->HasTextureCoords( 0 ) )
				{
					vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
				}

				a_MeshSource->m_Vertices.push_back( vertex );
			}

			// Handle Indices
			for ( uint32_t i = 0; i < mesh->mNumFaces; ++i )
			{
				const aiFace& face = mesh->mFaces[i];
				TE_CORE_ASSERT( face.mNumIndices == 3, "Face is not a triangle" );
				a_MeshSource->m_Indices.push_back( face.mIndices[0] );
				a_MeshSource->m_Indices.push_back( face.mIndices[1] );
				a_MeshSource->m_Indices.push_back( face.mIndices[2] );
			}
		}

		MeshSource::MeshNode& rootNode = a_MeshSource->m_MeshNodes.emplace_back();
		TraverseNodes( a_MeshSource, a_Scene->mRootNode, 0 );
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

			AssetHandle matAssetHandle = AssetHandle::Create();
			AssetManager::AddMemoryOnlyAsset( matAssetHandle, material );
			a_MeshSource->m_Materials[i] = matAssetHandle;
		}
	}

	AssetHandle AssimpImporter::ExtractTexture( void* a_AssimpScene, void* a_AssimpMaterial, int a_AssimpTextureType )
	{
		SharedPtr<TextureLoader> textureLoader = SharedPtrCast<TextureLoader>( AssetFactory::GetAssetLoader( EAssetType::Texture ) );
		aiScene* aiScn = static_cast<aiScene*>( a_AssimpScene );
		aiMaterial* aiMat = static_cast<aiMaterial*>( a_AssimpMaterial );
		aiTextureType aiTexType = static_cast<aiTextureType>( a_AssimpTextureType );
		aiString aiTexturePath;

		if ( aiMat->GetTexture( aiTexType, 0, &aiTexturePath ) == AI_FAILURE )
			return AssetHandle::InvalidGUID;

		AssetHandle texHandle = AssetHandle::Create();

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
				return AssetHandle::InvalidGUID;

			if ( SharedPtr<Texture> texture = textureLoader->LoadTexture( texturePath ) )
			{
				AssetManager::AddMemoryOnlyAsset( texHandle, texture );
				return texHandle;
			}
		}

		return AssetHandle::InvalidGUID;
	}

	void AssimpImporter::TraverseNodes( SharedPtr<MeshSource> a_MeshSource, void* a_AssimpNode, uint32_t a_NodeIndex, const Matrix4& a_ParentTransform, uint32_t a_Level )
	{
		using MeshNode = MeshSource::MeshNode;

		const aiNode* a_Node = static_cast<aiNode*>( a_AssimpNode );
		MeshNode& node = a_MeshSource->m_MeshNodes[a_NodeIndex];
		node.Name = a_Node->mName.C_Str();
		node.LocalTransform = Util::Mat4FromAIMatrix4x4( a_Node->mTransformation );

		// Process submeshes
		Matrix4 transform = a_ParentTransform * node.LocalTransform;
		for ( uint32_t i = 0; i < a_Node->mNumMeshes; i++ )
		{
			uint32_t submeshIndex = a_Node->mMeshes[i];
			SubMesh& submesh = a_MeshSource->m_SubMeshes[submeshIndex];
			submesh.Name = a_Node->mName.C_Str();
			submesh.Transform = transform;
			submesh.LocalTransform = node.LocalTransform;

			node.SubMeshes.push_back( submeshIndex );
		}

		// Recurse
		// Add children
		uint32_t parentNodeIndex = (uint32_t)a_MeshSource->m_MeshNodes.size() - 1;
		node.Children.resize( a_Node->mNumChildren );
		for ( uint32_t i = 0; i < a_Node->mNumChildren; i++ )
		{
			MeshNode& child = a_MeshSource->m_MeshNodes.emplace_back();
			uint32_t childIndex = static_cast<uint32_t>( a_MeshSource->m_MeshNodes.size() ) - 1;
			child.Parent = parentNodeIndex;
			a_MeshSource->m_MeshNodes[a_NodeIndex].Children[i] = childIndex;
			TraverseNodes( a_MeshSource, a_Node->mChildren[i], childIndex, transform, a_Level + 1 );
		}
	}

#endif
}