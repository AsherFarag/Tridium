#include "tripch.h"
#include "EnvironmentMap.h"
#include <Tridium/Graphics/Rendering/Renderer.h>
#include <Platform/OpenGL/OpenGLEnvironmentMap.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <stb_image.h>

namespace Tridium {

	SharedPtr<EnvironmentMap> EnvironmentMap::Create( const SharedPtr<Texture>& a_EquirectangularTexture )
    {
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return MakeShared<OpenGLEnvironmentMap>( a_EquirectangularTexture );
		}

		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
    }

	SharedPtr<EnvironmentMap> EnvironmentMap::Create( const IO::FilePath& a_Path )
	{
		std::string path = a_Path.ToString();
		if ( !stbi_is_hdr( path.c_str() ) )
		{
			TE_CORE_ERROR( "[EnvironmentMap::Create] {0} is not HDR!", path );
			return nullptr;
		}

		int width, height, channels;
		stbi_set_flip_vertically_on_load( false );
		float* data = stbi_loadf( path.c_str(), &width, &height, &channels, 0 );

		if ( !data )
		{
			TE_CORE_ERROR( "[EnvironmentMap::Create] Failed to load HDR image: {0}", path );
			return nullptr;
		}

		TextureSpecification textureSpecification =
		{
			.Width = static_cast<uint32_t>( width ),
			.Height = static_cast<uint32_t>( height ),
			.Depth = 0u,
			.TextureFormat = ETextureFormat::RGB16F,
			.GenerateMips = false,
		};

		SharedPtr<Texture> equirectangularTexture( Texture::Create( textureSpecification, data ) );
		return Create( equirectangularTexture );
	}

	SharedPtr<EnvironmentMap> EnvironmentMap::Create( AssetHandle a_Handle )
	{
		TODO( "Editor only!" );
		auto assetManager = AssetManager::Get<Editor::EditorAssetManager>();
		const AssetMetaData& metaData = assetManager->GetAssetMetaData( a_Handle );
		if ( metaData.IsValid() )
		{
			IO::FilePath path = assetManager->GetAbsolutePath( metaData.Path );
			return Create( path );
		}

		return nullptr;
	}

}