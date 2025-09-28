#include "tripch.h"
#include "RenderResourceManager.h"
#include <Tridium/Asset/MeshAsset.h>
#include <Tridium/Asset/MaterialAsset.h>
#include <Tridium/Asset/TextureAsset.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/Renderer/PipelineStateCache.h>
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>

namespace Tridium {
	
	//=============================================================================================
	template<Concepts::Derived<RenderResource> T>
	using ResourceVariants = SmallArray<T, 8>;

	template<Concepts::Derived<RenderResource> T>
	using ResourceMap = UnorderedMap<AssetID, ResourceVariants<T>>;

	//=============================================================================================
	static ResourceMap<RenderResourceStaticMesh> s_StaticMeshes;
	static ResourceMap<RenderResourceMaterial> s_Materials;
	static ResourceMap<RenderResourceTexture> s_Textures;


	//=============================================================================================
	// Common default textures used for rendering.
	// These are 1x1 textures with solid colors or normals.
	static RHITextureRef s_WhiteTex2D;
	static RHITextureRef s_BlackTex2D;
	static RHITextureRef s_NormalTex2D;

	template<Concepts::Derived<RenderResource> T>
	static auto GetResourceIterator( ResourceVariants<T>& a_Variants, RenderResourceID a_VariantID )
	{
		// Find the resource with the same variant ID as 'a_VariantID'.
		return std::find_if( a_Variants.Begin(), a_Variants.End(),
			[a_VariantID]( const T& a_Resource ) -> bool
			{
				return a_Resource.VariantID == a_VariantID;
			} );
	}

	template<Concepts::Derived<RenderResource> T>
	static T GetResource( ResourceMap<T>& a_ResourceMap, AssetID a_AssetID, RenderResourceID a_VariantID )
	{
		auto variantsIt = a_ResourceMap.find( a_AssetID );
		if ( variantsIt == a_ResourceMap.end() )
			return {}; // Return an invalid resource


		auto resourceIt = GetResourceIterator( variantsIt->second, a_VariantID );
		if ( resourceIt == variantsIt->second.End() )
			return {}; // Unable to find the specified variant, return an invalid resource.

		return *resourceIt;
	}

	template<Concepts::Derived<RenderResource> T>
	static bool CanAddResource( ResourceMap<T>& a_ResourceMap, const T& a_Resource )
	{
		ResourceVariants<T>& variants = a_ResourceMap[a_Resource.AssetID];
		auto resourceIt = GetResourceIterator( variants, a_Resource.VariantID );
		return resourceIt == variants.End();
	}

	template<Concepts::Derived<RenderResource> T>
	static bool AddResource( ResourceMap<T>& a_ResourceMap, const T& a_Resource, bool a_ForceReplace )
	{
		ResourceVariants<T>& variants = a_ResourceMap[a_Resource.AssetID];
		auto resourceIt = GetResourceIterator( variants, a_Resource.VariantID );
		if ( resourceIt != variants.End() )
		{
			if ( !a_ForceReplace )
				return false; // Resource with the same variant ID already exists and replacement is not allowed.

			*resourceIt = a_Resource;
			return true; // Replaced the existing resource with the new one.
		}

		variants.PushBack( a_Resource );
		return true;
	}

	template<Concepts::Derived<RenderResource> T>
	static bool RemoveResource( ResourceMap<T>& a_ResourceMap, AssetID a_AssetID, RenderResourceID a_VariantID )
	{
		auto variantsIt = a_ResourceMap.find( a_AssetID );
		if ( variantsIt == a_ResourceMap.end() )
			return false; // No resources for the asset.

		if ( a_VariantID == InvalidRenderResourceID )
		{
			// Remove all variants
			a_ResourceMap.erase( variantsIt );
			return true;
		}

		// Find the resource with the same variant ID as 'a_VariantID'.
		auto resourceIt = GetResourceIterator( variantsIt->second, a_VariantID );
		if ( resourceIt == variantsIt->second.End() )
			return false; // Unable to find the specified variant.

		if ( variantsIt->second.Size() == 1 )
		{
			// If this is the only variant, remove the entire asset entry.
			a_ResourceMap.erase( variantsIt );
			return true;
		}

		variantsIt->second.Erase( resourceIt );
		return true;
	}

    bool RenderResourceManager::Init()
    {
		// Create default resources
		{
			auto texDesc = RHITextureDesc{}
				.SetDimension( ERHITextureDimension::Texture2D )
				.SetWidth( 1 )
				.SetHeight( 1 )
				.SetFormat( ERHIFormat::RGBA8_UNORM )
				.SetBindFlags( ERHIBindFlags::ShaderResource )
				.SetUsage( ERHIUsage::Static );

			constexpr uint8_t blackPixel[4] = { 0, 0, 0, 1 };
			constexpr uint8_t whitePixel[4] = { 255, 255, 255, 1 };
			constexpr uint8_t normalPixel[4] = { 128, 128, 255, 1 };

			s_WhiteTex2D = RHI::CreateTexture( texDesc.SetName( "Default White Texture2D" ), { RHITextureSubresourceData{}.SetData( whitePixel ).SetRowStride( 4 ) } );
			s_BlackTex2D = RHI::CreateTexture( texDesc.SetName( "Default Black Texture2D" ), { RHITextureSubresourceData{}.SetData( blackPixel ).SetRowStride( 4 ) } );
			s_NormalTex2D = RHI::CreateTexture( texDesc.SetName( "Default Normal Texture2D" ), { RHITextureSubresourceData{}.SetData( normalPixel ).SetRowStride( 4 ) } );
		}

        return true;
    }

    void RenderResourceManager::Shutdown()
    {
        s_StaticMeshes.clear();
        s_Materials.clear();
		s_Textures.clear();

		s_WhiteTex2D = nullptr;
		s_BlackTex2D = nullptr;
		s_NormalTex2D = nullptr;
    }

	RenderResourceStaticMesh RenderResourceManager::GetStaticMesh( AssetID a_AssetID, RenderResourceID a_VariantID )
	{
		return GetResource( s_StaticMeshes, a_AssetID, a_VariantID );
	}

	RenderResourceStaticMesh RenderResourceManager::GetOrCreateStaticMesh( const AssetRef<StaticMesh>& a_Asset, RenderResourceID a_VariantID )
	{
		ASSERT( a_Asset != nullptr, "Cannot get or create a static mesh render resource for a null asset." );

		if ( RenderResourceStaticMesh resource = GetStaticMesh( a_Asset->ID(), a_VariantID); resource.Valid() )
		{
			// Found a valid resource, return it.
			return resource;
		}

		// Create a new resource with the given AssetID and VariantID.
		RenderResourceStaticMesh newResource;
		newResource.AssetID = a_Asset->ID();
		newResource.VariantID = a_VariantID;

		if ( !CanAddResource( s_StaticMeshes, newResource ) )
		{
			// Failed to add the new resource, return an invalid resource.
			return {};
		}

		// Create index buffer
		{
			const auto bufferDesc = RHIBufferDesc{}
				.SetName( std::format( "StaticMesh_{}_IndexBuffer", a_Asset->ID() ) )
				.SetSize( a_Asset->LODs().Front().Indices.SizeBytes() )
				.SetUsage( ERHIUsage::Static )
				.SetBindFlags( ERHIBindFlags::IndexBuffer )
				.SetFormat( ERHIFormat::R32_UINT )
				.SetType( ERHIBufferType::Formatted )
				.SetCpuAccess( ERHICpuAccess::None );

			newResource.IndexBuffer = RHI::CreateBuffer( bufferDesc, AsBytes( Span<const uint32_t>{ a_Asset->LODs().Front().Indices } ) );
		}

		// Create vertex buffer
		{
			const auto bufferDesc = RHIBufferDesc{}
				.SetName( std::format( "StaticMesh_{}_VertexBuffer", a_Asset->ID() ) )
				.SetSize( a_Asset->LODs().Front().Vertices.SizeBytes() )
				.SetUsage( ERHIUsage::Static )
				.SetBindFlags( ERHIBindFlags::VertexBuffer )
				.SetCpuAccess( ERHICpuAccess::None );

			newResource.VertexBuffer = RHI::CreateBuffer( bufferDesc, AsBytes( Span<const Vertex>{ a_Asset->LODs().Front().Vertices } ) );
		}

		// Create vertex layout
		{
			TODO( "Support custom vertex layouts" );
			newResource.VertexLayout = RHIVertexLayout::From<Vertex>();
		}

		// Finally, add the new resource to the manager.
		AddStaticMesh( newResource );

		return newResource;
	}

	bool RenderResourceManager::AddStaticMesh( const RenderResourceStaticMesh& a_StaticMesh, bool a_ForceReplace )
	{
		if ( !ASSERT( a_StaticMesh.Valid(), "Cannot add an invalid static mesh render resource." ) )
			return false;

		return AddResource( s_StaticMeshes, a_StaticMesh, a_ForceReplace );
	}

	bool RenderResourceManager::RemoveStaticMesh( AssetID a_AssetID, RenderResourceID a_VariantID )
	{
		return RemoveResource( s_StaticMeshes, a_AssetID, a_VariantID );
	}

	//=============================================================================================

	RenderResourceMaterial RenderResourceManager::GetMaterial( AssetID a_AssetID, RenderResourceID a_VariantID )
	{
		return GetResource( s_Materials, a_AssetID, a_VariantID );
	}

	RenderResourceMaterial RenderResourceManager::GetOrCreateMaterial( const AssetRef<Material>& a_Asset, RenderResourceID a_VariantID )
	{
		ASSERT( a_Asset != nullptr, "Cannot get or create a static mesh render resource for a null asset." );

		if ( RenderResourceMaterial resource = GetMaterial( a_Asset->ID(), a_VariantID ); resource.Valid() )
		{
			// Found a valid resource, return it.
			return resource;
		}

		// Create a new resource with the given AssetID and VariantID.
		RenderResourceMaterial newResource;
		newResource.AssetID = a_Asset->ID();
		newResource.VariantID = a_VariantID;

		if ( !CanAddResource( s_Materials, newResource ) )
		{
			// Failed to add the new resource, return an invalid resource.
			return {};
		}

		const auto& shaderFamily = ShaderLibrary::GetOrCreateVariant( Hashing::HashString( a_Asset->ShaderFamily() ), {} );
		if ( shaderFamily == nullptr || !shaderFamily->Valid() )
		{
			ASSERT( false, "Failed to find shader family '{}' for material asset.", a_Asset->ShaderFamily() );
			return {};
		}

		// Create the graphics pipeline state
		{
			newResource.Pipeline.ShaderVariant = shaderFamily;

			newResource.Pipeline.Topology = ERHITopology::Triangle;

			newResource.Pipeline.RasterizerState = RHIRasterizerState{}
				.SetCullMode( a_Asset->Flags().HasFlag( EMaterialFlags::TwoSided ) ? ERHICullMode::None : ERHICullMode::Back )
				.SetFillMode( ERHIFillMode::Solid )
				.SetDepthClipEnabled( true );

			newResource.Pipeline.DepthState = RHIDepthState{}
				.SetDepthTestEnabled( true )
				.SetDepthWriteEnabled( true )
				.SetComparison( ERHIComparison::Less );

			if ( a_Asset->Flags().HasFlag( EMaterialFlags::Transparent ) )
			{
				newResource.Pipeline.BlendState
					.SetBlendEnabled( true )
					.SetSrcColor( ERHIBlendFactor::SrcAlpha )
					.SetDstColor( ERHIBlendFactor::OneMinusSrcAlpha )
					.SetBlendOpColor( ERHIBlendOp::Add )
					.SetSrcAlpha( ERHIBlendFactor::One )
					.SetDstAlpha( ERHIBlendFactor::Zero )
					.SetBlendOpAlpha( ERHIBlendOp::Add )
					.SetColorWriteMask( ERHIColorMask::RGBA );
			}
			else
			{
				newResource.Pipeline.BlendState
					.SetBlendEnabled( false )
					.SetColorWriteMask( ERHIColorMask::RGBA );
			}
		}

		// Create the binding set
		{
			// Set material properties
			struct MaterialProperties
			{
				Color AlbedoColor = Color::White();
				float MetallicIntensity = 1.0f;
				float RoughnessIntensity = 1.0f;
				float EmissiveIntensity = 1.0f;
				float Padding = 0.0f; // Padding to make the size a multiple of 16 bytes
			};

			MaterialProperties materialProperties
			{
				.AlbedoColor = a_Asset->AlbedoColor(),
				.MetallicIntensity = a_Asset->MetallicIntensity(),
				.RoughnessIntensity = a_Asset->RoughnessIntensity(),
				.EmissiveIntensity = a_Asset->EmissiveIntensity()
			};

			RHIBufferRef materialBuffer = RHI::CreateBuffer(
				RHIBufferDesc{}
					.SetName( std::format( "Material_{}_PropertiesBuffer", a_Asset->ID() ) )
					.SetSize( sizeof( MaterialProperties ) )
					.SetUsage( ERHIUsage::Dynamic )
					.SetBindFlags( ERHIBindFlags::ConstantBuffer ),
				AsBytes( Span<const MaterialProperties>{ &materialProperties, 1 } )
			);

			const auto bindingLayoutDesc = RHIBindingLayoutDesc{}
				.AddBinding( "Constants"_H, RHIShaderBinding{}.AsInlinedConstants( 128 ) )
				.AddBinding( "u_MaterialProps"_H, RHIShaderBinding{}.AsConstantBuffer( 0 ) )
				.AddBinding( "AlbedoMap"_H, RHIShaderBinding{}.AsTexture( 0 ) )
				.AddBinding( "NormalMap"_H, RHIShaderBinding{}.AsTexture( 1 ) )
				.AddBinding( "MetallicMap"_H, RHIShaderBinding{}.AsTexture( 2 ) )
				.AddBinding( "RoughnessMap"_H, RHIShaderBinding{}.AsTexture( 3 ) )
				.AddBinding( "EmissiveMap"_H, RHIShaderBinding{}.AsTexture( 4 ) )
				.AddBinding( "AmbientOcclusionMap"_H, RHIShaderBinding{}.AsTexture( 5 ) );

			const RHIBindingLayoutRef bindingLayout = RHI::CreateBindingLayout( bindingLayoutDesc );

			auto bindingSetDesc = RHIBindingSetDesc{}
				.SetName( std::format( "Material_{}_BindingSet", a_Asset->ID() ) )
				.SetLayout( bindingLayout )
				.AddConstantBuffer( "u_MaterialProps"_H, materialBuffer.get() );

			const auto AddTextureBinding = [&]( const HashedString a_Name, const AssetRef<Texture>& a_Texture, const RHITextureRef& a_DefaultTexture = {} )
			{
				if ( a_Texture )
				{
					RenderResourceTexture textureResource = GetOrCreateTexture( a_Texture );
					bindingSetDesc.AddTexture( a_Name, textureResource.Texture.get() );
				}
				else if ( a_DefaultTexture != nullptr )
				{
					bindingSetDesc.AddTexture( a_Name, a_DefaultTexture.get() );
				}
			};

			// Add texture bindings
			AddTextureBinding( "AlbedoMap"_H, a_Asset->AlbedoMap(), s_WhiteTex2D );
			AddTextureBinding( "NormalMap"_H, a_Asset->NormalMap(), s_NormalTex2D );
			AddTextureBinding( "MetallicMap"_H, a_Asset->MetallicMap(), s_BlackTex2D );
			AddTextureBinding( "RoughnessMap"_H, a_Asset->RoughnessMap(), s_WhiteTex2D );
			AddTextureBinding( "EmissiveMap"_H, a_Asset->EmissiveMap(), s_BlackTex2D );
			AddTextureBinding( "AmbientOcclusionMap"_H, a_Asset->AmbientOcclusionMap(), s_WhiteTex2D );
			//AddTextureBinding( "OpacityTexture"_H, a_Asset->OpacityMap() );

			newResource.BindingSet = RHI::CreateBindingSet( bindingSetDesc );

			ASSERT( newResource.BindingSet != nullptr, "Failed to create binding set for material render resource." );
		}

		// Finally, add the new resource to the manager.
		AddMaterial( newResource );

		return newResource;
	}

	bool RenderResourceManager::AddMaterial( const RenderResourceMaterial& a_Material, bool a_ForceReplace )
	{
		if ( !ASSERT( a_Material.Valid(), "Cannot add an invalid material render resource." ) )
			return false;
		return AddResource( s_Materials, a_Material, a_ForceReplace );
	}

	bool RenderResourceManager::RemoveMaterial( AssetID a_AssetID, RenderResourceID a_VariantID )
	{
		return RemoveResource( s_Materials, a_AssetID, a_VariantID );
	}

	//=============================================================================================

	RenderResourceTexture RenderResourceManager::GetTexture( AssetID a_AssetID, RenderResourceID a_VariantID )
	{
		return GetResource( s_Textures, a_AssetID, a_VariantID );
	}

	RenderResourceTexture RenderResourceManager::GetOrCreateTexture( const AssetRef<class Texture>& a_Asset, RenderResourceID a_VariantID )
	{
		ASSERT( a_Asset != nullptr, "Cannot get or create a texture render resource for a null asset." );

		if ( RenderResourceTexture resource = GetTexture( a_Asset->ID(), a_VariantID ); resource.Valid() )
		{
			// Found a valid resource, return it.
			return resource;
		}

		// Create a new resource with the given AssetID and VariantID.
		RenderResourceTexture newResource;
		newResource.AssetID = a_Asset->ID();
		newResource.VariantID = a_VariantID;

		if ( !CanAddResource( s_Textures, newResource ) )
		{
			// Failed to add the new resource, return an invalid resource.
			return {};
		}

		// Create the RHI texture
		{
			const auto textureDesc = RHITextureDesc{}
				.SetName( std::format( "Texture_{}", a_Asset->ID() ) )
				.SetDimension( a_Asset->Dimension() )
				.SetWidth( a_Asset->Width() )
				.SetHeight( a_Asset->Height() )
				.SetDepth( a_Asset->DepthOrArraySize() )
				.SetMips( 0 ) TODO( "Support mipmaps" )
				.SetFormat( a_Asset->Format() )
				.SetUsage( ERHIUsage::Default )
				.SetBindFlags( ERHIBindFlags::ShaderResource )
				.SetCpuAccess( ERHICpuAccess::None );

			const auto initialData = RHITextureSubresourceData{}
				.SetData( a_Asset->PixelData().Data() )
				.SetRowStride( a_Asset->Width() * GetRHIFormatInfo( a_Asset->Format() ).Bytes() )
				.SetDepthStride( a_Asset->Width() * a_Asset->Height() * GetRHIFormatInfo( a_Asset->Format() ).Bytes() );

			newResource.Texture = RHI::CreateTexture( textureDesc, initialData );
			ASSERT( newResource.Texture != nullptr, "Failed to create RHI texture for texture render resource." );
		}

		// Finally, add the new resource to the manager.
		AddTexture( newResource );

		return newResource;
	}

	bool RenderResourceManager::AddTexture( const RenderResourceTexture& a_Texture, bool a_ForceReplace )
	{
		if ( !ASSERT( a_Texture.Valid(), "Cannot add an invalid texture render resource." ) )
			return false;
		return AddResource( s_Textures, a_Texture, a_ForceReplace );
	}

	bool RenderResourceManager::RemoveTexture( AssetID a_AssetID, RenderResourceID a_VariantID )
	{
		return RemoveResource( s_Textures, a_AssetID, a_VariantID );
	}

	const RHITextureRef& RenderResourceManager::GetWhiteTexture2D()
	{
		return s_WhiteTex2D;
	}

	const RHITextureRef& RenderResourceManager::GetBlackTexture2D()
	{
		return s_BlackTex2D;
	}

	const RHITextureRef& RenderResourceManager::GetNormalTexture2D()
	{
		return s_NormalTex2D;
	}
	

} // namespace Tridium
