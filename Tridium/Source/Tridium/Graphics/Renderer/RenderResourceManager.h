#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Containers/UnorderedMap.h>
#include <Tridium/Graphics/Renderer/RenderResource.h>

namespace Tridium {

	//=================================================================================================
	// Render Resource Manager: 
	// Manages common render resources and the link between Assets and their RenderResources.
	//=================================================================================================
	class RenderResourceManager
	{
	public:

		//=============================================================================================
		static RenderResourceStaticMesh GetStaticMesh( AssetID a_AssetID, RenderResourceID a_VariantID = 0 );
		static RenderResourceStaticMesh GetOrCreateStaticMesh( const AssetRef<class StaticMesh>& a_Asset, RenderResourceID a_VariantID = 0 );
		static bool AddStaticMesh( const RenderResourceStaticMesh& a_StaticMesh, bool a_ForceReplace = false );
		static bool RemoveStaticMesh( AssetID a_AssetID, RenderResourceID a_VariantID = InvalidRenderResourceID );

		//=============================================================================================
		static RenderResourceMaterial GetMaterial( AssetID a_AssetID, RenderResourceID a_VariantID = 0 );
		static RenderResourceMaterial GetOrCreateMaterial( const AssetRef<class Material>& a_Asset, RenderResourceID a_VariantID = 0 );
		static bool AddMaterial( const RenderResourceMaterial& a_Material, bool a_ForceReplace = false );
		static bool RemoveMaterial( AssetID a_AssetID, RenderResourceID a_VariantID = InvalidRenderResourceID );

		//=============================================================================================
		static RenderResourceTexture GetTexture( AssetID a_AssetID, RenderResourceID a_VariantID = 0 );
		static RenderResourceTexture GetOrCreateTexture( const AssetRef<class Texture>& a_Asset, RenderResourceID a_VariantID = 0 );
		static bool AddTexture( const RenderResourceTexture& a_Texture, bool a_ForceReplace = false );
		static bool RemoveTexture( AssetID a_AssetID, RenderResourceID a_VariantID = InvalidRenderResourceID );

		//=============================================================================================
		static RenderResourceEnvironmentMap GetEnvironmentMap( AssetID a_AssetID, RenderResourceID a_VariantID = 0 );
		static RenderResourceEnvironmentMap GetOrCreateEnvironmentMap( const AssetRef<class EnvironmentMap>& a_Asset, RenderResourceID a_VariantID = 0 );
		static bool AddEnvironmentMap( const RenderResourceEnvironmentMap& a_EnvMap, bool a_ForceReplace = false );
		static bool RemoveEnvironmentMap( AssetID a_AssetID, RenderResourceID a_VariantID = InvalidRenderResourceID );

		//=============================================================================================
		static const RHITextureRef& GetWhiteTexture2D();
		static const RHITextureRef& GetBlackTexture2D();
		static const RHITextureRef& GetNormalTexture2D();
		static const RHITextureRef& GetBRDFLUTTexture2D();

	private:

		//=============================================================================================
		friend class RendererModule;
		static bool Init();
		static void Shutdown();

	};

}  // namespace Tridium