#include "tripch.h"
#include "Texture.h"

#include <Tridium/Rendering/Renderer.h>
#include <Platform/OpenGL/OpenGLTexture.h>
#include <Tridium/Asset/Loaders/TextureLoader.h>
#include <Tridium/Core/Application.h>
#include <Tridium/Asset/AssetManager.h>

namespace Tridium {

	Texture* Texture::Create( const TextureSpecification& a_Specification )
	{
		switch ( Renderer::GetAPI() )
		{
			using enum RendererAPI::API;
		case OpenGL: 
			return new OpenGLTexture( a_Specification );
		}

		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
	}

	Texture* Texture::Create( const TextureSpecification& a_Specification, void* a_TextureData )
	{
		switch ( Renderer::GetAPI() )
		{
			using enum RendererAPI::API;
		case OpenGL:
			return new OpenGLTexture( a_Specification, a_TextureData );
		}

		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
	}

	CubeMap* CubeMap::Create( const TextureSpecification& a_Specification, const SharedPtr<Texture>& a_Texture )
	{
		switch ( Renderer::GetAPI() )
		{
			using enum RendererAPI::API;
		case OpenGL:
			return new OpenGLCubeMap( a_Specification, a_Texture );
		}
		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
	}

	CubeMap* CubeMap::Create( const TextureSpecification& a_Specification, const std::array<float*, 6>& a_CubeMapData )
	{
		switch ( Renderer::GetAPI() )
		{
			using enum RendererAPI::API;
		case OpenGL:
			return new OpenGLCubeMap( a_Specification, a_CubeMapData );
		}
		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
	}

	void TextureFactory::Init()
	{
		// Load the default textures
		GetWhiteTexture();
		GetBlackTexture();
		GetNormalTexture();
	}

	AssetHandle TextureFactory::GetWhiteTexture()
	{
		static SharedPtr<Texture> s_WhiteTexture = TextureLoader::LoadTexture( Application::GetEngineAssetsDirectory() / "Textures/White.tga" );
		static AssetHandle s_WhiteTextureHandle = ( s_WhiteTexture->SetHandle(30), 30 );
		static bool s_TextureInitialized = AssetManager::AddMemoryOnlyAsset( s_WhiteTextureHandle, s_WhiteTexture );

		return s_WhiteTextureHandle;
	}

	AssetHandle TextureFactory::GetBlackTexture()
	{
		static SharedPtr<Texture> s_BlackTexture = TextureLoader::LoadTexture( Application::GetEngineAssetsDirectory() / "Textures/Black.tga" );
		static AssetHandle s_BlackTextureHandle = ( s_BlackTexture->SetHandle( 31 ), 31 );
		static bool s_TextureInitialized = AssetManager::AddMemoryOnlyAsset( s_BlackTextureHandle, s_BlackTexture );

		return s_BlackTextureHandle;
	}

	AssetHandle TextureFactory::GetNormalTexture()
	{
		static SharedPtr<Texture> s_NormalTexture = TextureLoader::LoadTexture( Application::GetEngineAssetsDirectory() / "Textures/Normal.tga" );
		static AssetHandle s_NormalTextureHandle = ( s_NormalTexture->SetHandle( 32 ), 32 );
		static bool s_TextureInitialized = AssetManager::AddMemoryOnlyAsset( s_NormalTextureHandle, s_NormalTexture );

		return s_NormalTextureHandle;
	}
}