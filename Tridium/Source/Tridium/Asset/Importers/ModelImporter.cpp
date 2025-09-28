#include "tripch.h"

#if USE_ASSET_IMPORTERS

#include "ModelImporter.h"
#include <Tridium/Asset/MeshAsset.h>
#include <Tridium/Asset/MaterialAsset.h>
#include <Tridium/Asset/TextureAsset.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include <stb_image.h>

namespace std {
	template<>
	struct hash<aiString>
	{
		size_t operator()( const aiString& a_Str ) const
		{
			return hash<string_view>()( string_view( a_Str.C_Str(), a_Str.length ) );
		}
	};
}

#define MESH_LOG(...) LOG( LogCategory::Asset, Info, __VA_ARGS__ )

namespace Tridium {

	REGISTER_ASSET_IMPORTER( ModelImporter );

	// Importer setting for Assimp
	static constexpr uint32_t s_AssimpImportFlags =
		aiProcessPreset_TargetRealtime_MaxQuality | // We only import assets in the editor, so import-time performance doesn't matter
		aiProcess_ConvertToLeftHanded             | // DirectX style
		aiProcess_RemoveComponent                 | // Removes unneeded components for better performance
		aiProcess_GlobalScale;

	// Components to remove from the imported scene for better performance
	static constexpr uint32_t s_AssimpRemoveComponents =
		aiComponent_CAMERAS         |
		aiComponent_LIGHTS          |
		aiComponent_COLORS		    | // We don't support vertex colors
		aiComponent_TEXCOORDSn( 1 ) | // We only support one UV channel for now
		aiComponent_TEXCOORDSn( 2 ) |
		aiComponent_TEXCOORDSn( 3 ) |
		aiComponent_TEXCOORDSn( 4 ) |
		aiComponent_TEXCOORDSn( 5 ) |
		aiComponent_TEXCOORDSn( 6 ) |
		aiComponent_TEXCOORDSn( 7 );

	static Matrix4 Mat4FromAIMatrix4x4( const aiMatrix4x4& matrix )
	{
		Matrix4 result;
		result[ 0 ][ 0 ] = ( float )matrix.a1; result[ 0 ][ 1 ] = ( float )matrix.b1;  result[ 0 ][ 2 ] = ( float )matrix.c1; result[ 0 ][ 3 ] = ( float )matrix.d1;
		result[ 1 ][ 0 ] = ( float )matrix.a2; result[ 1 ][ 1 ] = ( float )matrix.b2;  result[ 1 ][ 2 ] = ( float )matrix.c2; result[ 1 ][ 3 ] = ( float )matrix.d2;
		result[ 2 ][ 0 ] = ( float )matrix.a3; result[ 2 ][ 1 ] = ( float )matrix.b3;  result[ 2 ][ 2 ] = ( float )matrix.c3; result[ 2 ][ 3 ] = ( float )matrix.d3;
		result[ 3 ][ 0 ] = ( float )matrix.a4; result[ 3 ][ 1 ] = ( float )matrix.b4;  result[ 3 ][ 2 ] = ( float )matrix.c4; result[ 3 ][ 3 ] = ( float )matrix.d4;
		return result;
	}

	class StaticMeshAccessor : public StaticMesh
	{
		friend struct AssimpProcessor;
	};

	class TextureAccessor : public Texture
	{
		friend struct AssimpProcessor;
	};

	struct AssimpProcessor
	{
		void Process( AssetImportContext& a_Context, const aiScene* a_Scene )
		{
			ProcessMaterials( a_Context, a_Scene );
			ProcessNode( a_Context, Matrix4( 1.0f ), a_Scene, a_Scene->mRootNode );

			// Clear cached data
			Clear();
		}

	private:

		Array<AssetRef<Material>> m_Materials;

		void Clear()
		{
			m_Materials.Clear();
		}

		void ProcessNode( AssetImportContext& a_Context, const Matrix4& a_ParentTransform, const aiScene* a_Scene, const aiNode* a_Node );
		void ProcessMaterials( AssetImportContext& a_Context, const aiScene* a_Scene );

		bool ProcessStaticMesh( AssetImportContext& a_Context, const aiScene* a_Scene, const aiNode* a_Node, const Matrix4& a_Transform );

	};

	bool ModelImporter::OnImport( AssetImportContext& a_Context )
	{
		Assimp::Importer importer;
		importer.SetPropertyFloat( AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.0f );
		importer.SetPropertyInteger( AI_CONFIG_PP_RVC_FLAGS, s_AssimpRemoveComponents );

		const Span<const byte_t> fileData = a_Context.FileData();
		const aiScene* assimpScene = fileData.empty()
			? importer.ReadFile( a_Context.AssetPath().ToString().c_str(), s_AssimpImportFlags )
			: importer.ReadFileFromMemory( fileData.data(), fileData.size(), s_AssimpImportFlags, a_Context.AssetPath().GetExtension().ToString().c_str() );

		if ( !assimpScene )
		{
			a_Context.FailImport( importer.GetErrorString() );
			return false;
		}

		static thread_local AssimpProcessor processor{};
		processor.Process( a_Context, assimpScene );

		return a_Context.ImportFailed() == false;
	}

	//=============================================================================================
	// Assimp processing
	//=============================================================================================

	static String GetAssimpNodeName( const aiNode* a_Node )
	{
		if ( a_Node && a_Node->mName.length > 0 )
		{
			return a_Node->mName.C_Str();
		}

		return FilePath::UniquePath().ToString(); // Fallback to a unique name if the node has no name
	}

	void AssimpProcessor::ProcessNode( AssetImportContext& a_Context, const Matrix4& a_ParentTransform, const aiScene* a_Scene, const aiNode* a_Node )
	{
		// A node in Assimp is a scene graph node that can contain meshes and child nodes.
		// If a node contains meshes, we treat the node as a StaticMesh asset.
		// To process a node, we need to:
		// 1. Apply the node's transformation to the parent transformation to get the global transform.
		// 2. Check if the node has meshes, and if so, create a StaticMesh asset for it.
		// 3. Recurse for each child node.
		// Note: A node can have multiple meshes, which we will treat as SubMeshes of the StaticMesh asset.

		if ( a_Node == nullptr )
		{
			a_Context.FailImport( "Assimp node is null." );
			return;
		}

		const Matrix4 localTransform = Mat4FromAIMatrix4x4( a_Node->mTransformation );
		const Matrix4 transform = a_ParentTransform * localTransform;

		ProcessStaticMesh( a_Context, a_Scene, a_Node, transform );

		// Recurse for each child node and process their meshes
		for ( uint32_t i = 0; i < a_Node->mNumChildren; ++i )
		{
			ProcessNode( a_Context, transform, a_Scene, a_Node->mChildren[ i ] );

			if ( a_Context.ImportFailed() )
			{
				return; // Stop processing if import failed
			}
		}
	}

	void AssimpProcessor::ProcessMaterials( AssetImportContext& a_Context, const aiScene* a_Scene )
	{
		if ( !a_Scene->HasMaterials() )
		{
			return;
		}

		MESH_LOG( "Processing {} materials...", a_Scene->mNumMaterials );

		m_Materials.Resize( a_Scene->mNumMaterials );

		UnorderedMap<aiString, AssetRef<Texture>> loadedTextures;
		loadedTextures.reserve( Cast<size_t>( a_Scene->mNumMaterials ) );

		// Will check if the requested texture has already been loaded,
		// and if not, will load it and store it in the loadedTextures map.
		const auto GetOrLoadTexture = [&]( const aiMaterial* a_AssimpMat, aiTextureType a_TextureType ) -> AssetRef<Texture>
		{
			// Get the texture path from the material
			aiString path;
			if ( a_AssimpMat->GetTexture( a_TextureType, 0, &path ) != aiReturn_SUCCESS )
			{
				// No texture of this type
				return nullptr;
			}

			// Check if we've already loaded this texture
			auto it = loadedTextures.find( path );
			if ( it != loadedTextures.end() )
			{
				// Already loaded, return it
				return it->second;
			}

			// Load the texture

			String textureName{};
			Array<byte_t> textureData;
			size_t textureWidth = 0;
			size_t textureHeight = 0;
			ERHIFormat textureFormat = ERHIFormat::Unknown;
			bool isEmbedded = false;

			// Is the texture embedded in the model file?
			if ( const aiTexture* aiTexEmbedded = a_Scene->GetEmbeddedTexture( path.C_Str() ) )
			{
				TODO( "We can't just mem copy. We need to decode the texture data based on the format hint." );
				NOT_IMPLEMENTED;
				// We can just copy the raw data from Assimp
				textureName = FilePath( aiTexEmbedded->mFilename.C_Str() ).GetFilenameWithoutExtension();
				textureData.Resize( aiTexEmbedded->mWidth * ( aiTexEmbedded->mHeight == 0 ? 1 : aiTexEmbedded->mHeight ) );
				memcpy( textureData.Data(), aiTexEmbedded->pcData, textureData.Size() );

				isEmbedded = true;
			}
			// Or is it an external texture file?
			else
			{
				// We need to load the texture from file using stb_image
				// A texture is either HDR (float) or standard (uint8_t)
				// Which must be loaded differently.

				// We assume the texture path is relative to the model file
				const FilePath texturePath = a_Context.AssetPath().GetParentPath() / FilePath( path.C_Str() );
				textureName = texturePath.GetFilenameWithoutExtension();

				if ( texturePath.Exists() && texturePath.IsFile() )
				{
					String texturePathStr = texturePath.ToString();

					// Get info about the texture
					int width, height, channels;
					stbi_info( texturePathStr.c_str(), &width, &height, &channels );

					if ( stbi_is_hdr( texturePathStr.c_str() ) )
					{
						float* data = stbi_loadf( texturePath.ToString().c_str(), &width, &height, &channels, 0 );

						if ( !data )
						{
							LOG( LogCategory::Asset, Error, "Failed to load HDR texture file '{}': {}", texturePathStr, stbi_failure_reason() );
							return nullptr;
						}

						textureWidth = Cast<size_t>( width );
						textureHeight = Cast<size_t>( height );
						switch ( channels )
						{
							case 1: textureFormat = ERHIFormat::R32_FLOAT; break;
							case 2: textureFormat = ERHIFormat::RG32_FLOAT; break;
							case 3: textureFormat = ERHIFormat::RGB32_FLOAT; break;
							case 4: textureFormat = ERHIFormat::RGBA32_FLOAT; break;
							default: ASSERT( false, "Unsupported number of channels in HDR texture: {}", channels ); return nullptr;
						}

						const size_t dataSize = textureWidth * textureHeight * channels * sizeof( float );
						textureData.Resize( dataSize );
						memcpy( textureData.Data(), data, dataSize );

						stbi_image_free( data );
					}
					// Otherwise, assume it's a standard 8-bit texture
					else
					{
						// We don't support 3-channel textures, so we convert them to 4-channel
						const int desiredChannels = channels == 3 ? 4 : channels;
						channels = desiredChannels;
						uint8_t* data = stbi_load( texturePath.ToString().c_str(), &width, &height, nullptr, desiredChannels );

						if ( !data )
						{
							LOG( LogCategory::Asset, Error, "Failed to load texture file '{}': {}", texturePathStr, stbi_failure_reason() );
							return nullptr;
						}

						textureWidth = Cast<size_t>( width );
						textureHeight = Cast<size_t>( height );
						switch ( channels )
						{
							case 1: textureFormat = ERHIFormat::R8_UNORM; break;
							case 2: textureFormat = ERHIFormat::RG8_UNORM; break;
							case 4: textureFormat = ERHIFormat::RGBA8_UNORM; break;
							default: ASSERT( false, "Unsupported number of channels in texture: {}", channels ); return nullptr;
						}

						const size_t dataSize = textureWidth * textureHeight * channels * sizeof( uint8_t );
						textureData.Resize( dataSize );
						memcpy( textureData.Data(), data, dataSize );

						stbi_image_free( data );
					}
				}
				else
				{
					LOG( LogCategory::Asset, Error, "Texture file '{}' does not exist.", texturePath.ToString() );
					return nullptr;
				}
			}

			MESH_LOG( "Loaded texture '{}'", textureName );

			AssetRef<Texture> textureRef;
			{
				// Create the texture asset
				const FilePath assetFileName = FilePath::UniquePath( std::format( "{}_{}-{}.{}",
																				  a_Context.AssetPath().GetFilenameWithoutExtension(),
																				  textureName,
																				  "%%%%%", /* This is where the unique characters will be placed */
																				  AssetExtensionName ) ); // e.g. Model_TextureName-0001.tasset

				const FilePath assetPath = a_Context.DestinationPath() / assetFileName;

				textureRef = loadedTextures[ path ] = a_Context.CreateAsset<Texture>(
					std::move( textureName ),
					assetPath.ToString(),
					!isEmbedded ? ( a_Context.AssetPath().GetParentPath() / FilePath( path.C_Str() ) ).ToString() : String{}
				);
			}

			TextureAccessor* texture = Cast<TextureAccessor*>( textureRef.get() );

			// Now we can set the data on the texture asset
			texture->m_PixelData = std::move( textureData );
			texture->m_Width = Cast<uint32_t>( textureWidth );
			texture->m_Height = Cast<uint32_t>( textureHeight );
			texture->m_DepthOrArraySize = 1;
			texture->m_Dimension = ERHITextureDimension::Texture2D;
			texture->m_Format = textureFormat;

			return textureRef;
		};

		m_Materials.Resize( a_Scene->mNumMaterials );
		for ( uint32_t i = 0; i < a_Scene->mNumMaterials; ++i )
		{
			const aiMaterial* aiMat = a_Scene->mMaterials[ i ];

			AssetRef<Material>& material = m_Materials[ i ];
			{
				String materialName;
				FilePath assetPath;

				TODO( "I don't this method of creating unique material names. Ideally at the end of the import process we should check for duplicate names and rename them accordingly?" );
				if ( aiString aiMatName; aiMat->Get( AI_MATKEY_NAME, aiMatName ) == aiReturn_SUCCESS && aiMatName.length > 0 )
				{
					materialName = aiMatName.C_Str();
				}
				else
				{
					materialName = FilePath::UniquePath().ToString(); // Fallback to a unique name if the material has no name
				}

				materialName = std::format( "{}_{}", a_Context.AssetPath().GetFilenameWithoutExtension(), materialName );
				assetPath = a_Context.DestinationPath() / std::format( "{}.{}", materialName, AssetExtensionName );

				MESH_LOG( "Processing material '{}'...", materialName );

				material = a_Context.CreateAsset<Material>( std::move( materialName ), assetPath.ToString() );
			}

			// Albedo (Diffuse)
			{
				// Color
				aiColor3D color( 1.0f, 1.0f, 1.0f );
				if ( aiMat->Get( AI_MATKEY_COLOR_DIFFUSE, color ) == aiReturn_SUCCESS )
				{
					TODO( "Do we need to convert from linear to sRGB?" );
					material->SetAlbedoColor( Color( color.r, color.g, color.b, 1.0f ) );
				}

				// Texture
				material->SetAlbedoMap( GetOrLoadTexture( aiMat, aiTextureType_DIFFUSE ) );
			}

			// Normal
			{
				// Texture
				material->SetNormalMap( GetOrLoadTexture( aiMat, aiTextureType_NORMALS ) );
			}

			// Metallic (Reflectivity)
			{
				// Intensity
				float metalness = 0.0f;
				aiMat->Get( AI_MATKEY_REFLECTIVITY, metalness );
				metalness = metalness < 0.5f ? 0.0f : 1.0f; // Convert to binary metalness (for now?)
				material->SetMetallicIntensity( metalness );

				// Texture
				material->SetMetallicMap( GetOrLoadTexture( aiMat, aiTextureType_REFLECTION ) );
			}

			// Roughness (Shininess)
			{
				// Intensity
				float roughness = 0.4f;
				aiMat->Get( AI_MATKEY_SHININESS_STRENGTH, roughness );
				material->SetRoughnessIntensity( roughness );

				// Texture
				TODO( "We only have a shininess map in assimp, need to invert it to get roughness" );
				material->SetRoughnessMap( GetOrLoadTexture( aiMat, aiTextureType_SHININESS ) );
			}

			// Emission
			{
				// Intensity
				aiColor3D aiEmission( 0.0f, 0.0f, 0.0f );
				if ( aiMat->Get( AI_MATKEY_COLOR_EMISSIVE, aiEmission ) == aiReturn_SUCCESS )
				{
					// We only support grayscale emission intensity (for now?)
					material->SetEmissiveIntensity( aiEmission.r );
				}

				// Texture
				material->SetEmissiveMap( GetOrLoadTexture( aiMat, aiTextureType_EMISSIVE ) );
			}

			// Ambient Occlusion (Lightmap)
			{
				// Texture
				material->SetAmbientOcclusionMap( GetOrLoadTexture( aiMat, aiTextureType_LIGHTMAP ) );
			}

			// Opacity
			{
				// Texture
				material->SetOpacityMap( GetOrLoadTexture( aiMat, aiTextureType_OPACITY ) );
			}

			// Material Flags
			{
				EMaterialFlags matFlags = EMaterialFlags::None;

				// Two-sided
				int32_t twoSided = 0;
				if ( aiMat->Get( AI_MATKEY_TWOSIDED, twoSided ) == aiReturn_SUCCESS && twoSided != 0 )
				{
					matFlags |= EMaterialFlags::TwoSided;
				}

				// Transparent
				float opacity = 1.0f;
				if ( aiMat->Get( AI_MATKEY_OPACITY, opacity ) == aiReturn_SUCCESS && opacity < 1.0f )
				{
					matFlags |= EMaterialFlags::Transparent;
				}

				// Shading Mode
				if ( int32_t shadingModel = 0; aiMat->Get( AI_MATKEY_SHADING_MODEL, shadingModel ) == aiReturn_SUCCESS )
				{
					switch (shadingModel )
					{
						case aiShadingMode_NoShading:
						case aiShadingMode_Flat:
							material->SetShaderFamily( String{ DefaultShaderFamilies::Unlit } );
							break;
						case aiShadingMode_Gouraud:
						case aiShadingMode_Phong:
						case aiShadingMode_Blinn:
							material->SetShaderFamily( String{ DefaultShaderFamilies::LitDefault } );
							break;
						case aiShadingMode_Toon:
							material->SetShaderFamily( String{ DefaultShaderFamilies::Toon } );
							break;
						case aiShadingMode_OrenNayar:
						case aiShadingMode_Minnaert:
						case aiShadingMode_CookTorrance:
						default:
							material->SetShaderFamily( String{ DefaultShaderFamilies::LitDefault } );
							break;
					}
				}
				else
				{
					// Default to Lit shader if no shading model is specified
					material->SetShaderFamily( String{ DefaultShaderFamilies::LitDefault } );
				}

				material->SetFlags( matFlags );
			}

			// Register the dependencies for the material to its textures
			if ( material->AlbedoMap() ) a_Context.RegisterDependency( material->ID(), material->AlbedoMap()->ID() );
			if ( material->NormalMap() ) a_Context.RegisterDependency( material->ID(), material->NormalMap()->ID() );
			if ( material->MetallicMap() ) a_Context.RegisterDependency( material->ID(), material->MetallicMap()->ID() );
			if ( material->RoughnessMap() ) a_Context.RegisterDependency( material->ID(), material->RoughnessMap()->ID() );
			if ( material->EmissiveMap() ) a_Context.RegisterDependency( material->ID(), material->EmissiveMap()->ID() );
			if ( material->AmbientOcclusionMap() ) a_Context.RegisterDependency( material->ID(), material->AmbientOcclusionMap()->ID() );
			if ( material->OpacityMap() ) a_Context.RegisterDependency( material->ID(), material->OpacityMap()->ID() );
		}
	}

	bool AssimpProcessor::ProcessStaticMesh( AssetImportContext& a_Context, const aiScene* a_Scene, const aiNode* a_Node, const Matrix4& transform )
	{
		// To create a StaticMesh asset from an Assimp node, we need to:
		// 1. Create a new StaticMesh asset.
		// 2. For each mesh in the node, extract its vertex and index data and add it to the StaticMesh. Then create a SubMesh for it with the appropriate offset and count.
		// 3. Update the StaticMesh's bounding box to encompass all its SubMeshes.

		// Create a unique asset file name for this node's mesh ( e.g. Model_NodeName-0001.tasset )
		const FilePath assetFileName = FilePath::UniquePath( std::format( "{}_{}-{}.{}",
																		  a_Context.AssetPath().GetFilenameWithoutExtension(),
																		  GetAssimpNodeName( a_Node ),
																		  "%%%%%", /* This is where the unique characters will be placed */
																		  AssetExtensionName ) );

		// Now we create the final file path by combining the destination path with the unique file name
		const FilePath assetPath = a_Context.DestinationPath() / assetFileName;

		// Create the static mesh asset for this node
		AssetRef<StaticMesh> staticMeshRef = a_Context.CreateAsset<StaticMesh>( a_Node->mName.C_Str(), assetPath.ToString() );
		if ( !staticMeshRef )
		{
			a_Context.FailImport( "Failed to create StaticMesh asset." );
			return false;
		}

		// StaticMeshAccessor allows us to modify the protected members of StaticMesh
		auto staticMesh = Cast<StaticMeshAccessor*>( staticMeshRef.get() );

		UnorderedMap<uint32_t, uint32_t> remapMaterialIndices; 
		remapMaterialIndices.reserve( m_Materials.Size() );

		const auto GetRemappedMaterialIndex = [&]( uint32_t a_OriginalIndex ) -> uint32_t
		{
			if ( a_OriginalIndex >= m_Materials.Size() )
			{
				return InvalidMaterialIndex;
			}

			auto it = remapMaterialIndices.find( a_OriginalIndex );
			if ( it != remapMaterialIndices.end() )
			{
				return it->second;
			}

			const uint32_t newIndex = staticMesh->m_Materials.Size();
			staticMesh->m_Materials.PushBack( m_Materials[ a_OriginalIndex ] );
			remapMaterialIndices[ a_OriginalIndex ] = newIndex;

			a_Context.RegisterDependency( staticMesh->ID(), m_Materials[a_OriginalIndex]->ID() );

			return newIndex;
		};

		// Iterate through all the meshes in this node and process them,
		// adding them to the static mesh asset.
		StaticMesh::LOD& lod = staticMesh->m_LODs.Empty() ? staticMesh->m_LODs.EmplaceBack() : staticMesh->m_LODs[ 0 ];
		for ( uint32_t i = 0; i < a_Node->mNumMeshes; ++i )
		{
			const aiMesh* assimpMesh = a_Scene->mMeshes[ a_Node->mMeshes[ i ] ];

			SubMesh& submesh = staticMesh->m_SubMeshes.EmplaceBack();
			submesh.Name = assimpMesh->mName.C_Str();
			submesh.Transform = transform;
			submesh.MaterialIndex = GetRemappedMaterialIndex( assimpMesh->mMaterialIndex );
			submesh.BaseVertexIndex = lod.Vertices.Size();
			submesh.BaseIndex = lod.Indices.Size();
			submesh.VertexCount = assimpMesh->mNumVertices;

			if ( submesh.MaterialIndex == InvalidMaterialIndex )
			{
				a_Context.LogWarning( "Mesh '{}' uses material index {} which is out of bounds (max {}). Setting to InvalidMaterialIndex.", submesh.Name, assimpMesh->mMaterialIndex, m_Materials.Size() - 1 );
			}

			// Process Vertices
			lod.Vertices.Reserve( lod.Vertices.Size() + assimpMesh->mNumVertices );
			for ( uint32_t i = 0; i < assimpMesh->mNumVertices; ++i )
			{
				Vertex& vertex = lod.Vertices.EmplaceBack();

				// Extract Position
				vertex.Position = { assimpMesh->mVertices[ i ].x, assimpMesh->mVertices[ i ].y, assimpMesh->mVertices[ i ].z };
				submesh.BoundingBox.Expand( vertex.Position );

				// Extract Normal (if present)
				if ( assimpMesh->HasNormals() )
				{
					vertex.Normal = { assimpMesh->mNormals[ i ].x, assimpMesh->mNormals[ i ].y, assimpMesh->mNormals[ i ].z };
				}

				// Extract Tangent (if present)
				if ( assimpMesh->HasTangentsAndBitangents() )
				{
					// We calculate the bitangents in the shader, so we only need to store the tangent and its handedness
					const aiVector3D& tangent = assimpMesh->mTangents[i];
					const aiVector3D& bitangent = assimpMesh->mBitangents[i];
					const aiVector3D& normal = assimpMesh->mNormals[i];
					const float handedness = ( normal ^ tangent ) * bitangent < 0.0f ? -1.0f : 1.0f;

					vertex.Tangent = { tangent.x, tangent.y, tangent.z, handedness };
				}

				TODO( "Should we support multiple UV channels?" );
				// Extract TexCoord (if present)
				if ( assimpMesh->HasTextureCoords( 0 ) )
				{
					vertex.TexCoord.X = assimpMesh->mTextureCoords[ 0 ][ i ].x;
					vertex.TexCoord.Y = assimpMesh->mTextureCoords[ 0 ][ i ].y;
				}
			}

			// Process Indices
			lod.Indices.Reserve( lod.Indices.Size() + assimpMesh->mNumFaces * 3 );
			for ( uint32_t i = 0; i < assimpMesh->mNumFaces; ++i )
			{
				const aiFace& face = assimpMesh->mFaces[ i ];

				if ( face.mNumIndices != 3 )
				{
					a_Context.FailImport( "Mesh contains non-triangular faces. Ensure the model is triangulated." );
					return false;
				}

				lod.Indices.PushBack( face.mIndices[ 0 ] );
				lod.Indices.PushBack( face.mIndices[ 1 ] );
				lod.Indices.PushBack( face.mIndices[ 2 ] );
			}

			submesh.IndexCount = lod.Indices.Size() - submesh.BaseIndex;
		}

		// The static mesh has successfully been created and populated with submeshes.
		// Now we can finally calculate its bounding box.
		staticMesh->UpdateBoundingBox();

		return true;
	}

} // namespace Tridium

#endif // USE_ASSET_IMPORTERS