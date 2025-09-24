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

	static const StringView VS = R"(
#include "Globals.hlsli"

    struct InlinedConstants
    {
        float4x4 PVM;
    };

    INLINED_CONSTANTS( inlinedConstants, InlinedConstants );

    struct VS_INPUT
    {
		float3 Position : POSITION;
		float3 Normal : NORMAL;
		float3 Tangent : TANGENT;
		float2 TexCoord : TEXCOORD0;
    };

    struct PS_INPUT
    {
      float4 pos : SV_POSITION;
      float4 col : COLOR0;
      float2 uv  : TEXCOORD0;
    };

    PS_INPUT VSMain(VS_INPUT input)
    {
      PS_INPUT output;
	  output.pos = mul(inlinedConstants.PVM, float4(input.Position, 1.0f));
	  output.uv  = input.TexCoord;
	  output.col = float4(1.0f, 1.0f, 1.0f, 1.0f);
      return output;
    }

)";

	static const StringView PS = R"(
	#include "Globals.hlsli"
	#include "Material.hlsli"

	struct PS_INPUT
	{
	  float4 pos : SV_POSITION;
	  float4 col : COLOR0;
	  float2 uv  : TEXCOORD0;
	};

	COMBINED_SAMPLER( AlbedoMap, Texture2D, 0 );
	COMBINED_SAMPLER( NormalMap, Texture2D, 1 );

	float4 PSMain(PS_INPUT input) : SV_TARGET0
	{
		return SampleTexture( AlbedoMap, input.uv ) * 0.8 + SampleTexture( NormalMap, input.uv ) * u_MaterialProps.AlbedoColor.r;
	}
)";


    bool RenderResourceManager::Init()
    {
		ShaderFamily defaultLitFamily;
		defaultLitFamily.Name = DefaultShaderFamilies::Lit;
		defaultLitFamily.ShaderSources[(size_t)ERHIShaderType::Vertex] = VS;
		defaultLitFamily.ShaderSources[(size_t)ERHIShaderType::Pixel] = PS;
		ShaderLibrary::RegisterFamily( std::move( defaultLitFamily ) );

        return true;
    }

    void RenderResourceManager::Shutdown()
    {
        s_StaticMeshes.clear();
        s_Materials.clear();
		s_Textures.clear();
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

			auto bindingSetDesc = RHIBindingSetDesc{}
				.SetName( std::format( "Material_{}_BindingSet", a_Asset->ID() ) )
				.SetLayout( shaderFamily->BindingLayout )
				.AddConstantBuffer( "u_MaterialProps"_H, materialBuffer.get() );

			const auto AddTextureBinding = [&]( const HashedString a_Name, const AssetRef<Texture>& a_Texture )
			{
				if ( a_Texture )
				{
					RenderResourceTexture textureResource = GetOrCreateTexture( a_Texture );
					bindingSetDesc.AddTexture( a_Name, textureResource.Texture.get() );
				}
			};

			// Add texture bindings
			AddTextureBinding( "AlbedoMap"_H, a_Asset->AlbedoMap() );
			//AddTextureBinding( "u_NormalTexture"_H, a_Asset->NormalMap() );
			//AddTextureBinding( "u_MetallicTexture"_H, a_Asset->MetallicMap() );
			//AddTextureBinding( "u_RoughnessTexture"_H, a_Asset->RoughnessMap() );
			//AddTextureBinding( "u_EmissiveTexture"_H, a_Asset->EmissiveMap() );
			//AddTextureBinding( "u_AmbientOcclusionTexture"_H, a_Asset->AmbientOcclusionMap() );
			//AddTextureBinding( "u_OpacityTexture"_H, a_Asset->OpacityMap() );

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
	

} // namespace Tridium
