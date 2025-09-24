#pragma once
#include <Tridium/Asset/AssetDefinitions.h>
#include <Tridium/Graphics/Renderer/ShaderFamily.h>
#include <Tridium/Graphics/RHI/RHIResource.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>
#include <Tridium/Graphics/RHI/RHIBuffer.h>

namespace Tridium {

	using RenderResourceID = uint32_t;
	constexpr RenderResourceID InvalidRenderResourceID = ~0u;

	//=================================================================================================
	// Render Resource: Represents the link between an Asset and the RHI resources that are used to render it.
	// An Asset can have multiple RenderResources, each being differentiated by a 'VariantID'.
	//=================================================================================================
	struct RenderResource
	{
		// Since an asset can have multiple RenderResources, this unique ID corresponds to a variant of the Asset.
		// If 'VariantID' = 0, then it is not a unique variant and is the default version for the owning Asset.
		RenderResourceID VariantID = 0;
		// The ID of the Asset this RenderResource is associated with.
		// NOTE: The Asset owns the RenderResource.
		AssetID AssetID = AssetID::InvalidID;

		virtual bool Valid() const { return VariantID != InvalidRenderResourceID && AssetID != AssetID::InvalidID; }
	};

	struct RenderResourceStaticMesh : RenderResource
	{
		RHIBufferRef VertexBuffer{};
		RHIBufferRef IndexBuffer{};
		RHIVertexLayout VertexLayout{};

		bool Valid() const override { return RenderResource::Valid() && VertexBuffer != nullptr; }
	};

	struct RenderResourceMaterial : RenderResource
	{
		RHIBindingSetRef BindingSet{};
		struct PipelineSettings
		{
			SharedPtr<ShaderFamilyVariant> ShaderVariant{};
			ERHITopology Topology = ERHITopology::Triangle;
			RHIRasterizerState RasterizerState{};
			RHIDepthState DepthState{};
			RHIBlendState::RenderTarget BlendState{};
		} Pipeline{};

		bool Valid() const override { return RenderResource::Valid() && Pipeline.ShaderVariant != nullptr && BindingSet != nullptr; }
	};

	struct RenderResourceTexture : RenderResource
	{
		RHITextureRef Texture{};

		bool Valid() const override { return RenderResource::Valid() && Texture != nullptr; }
	};
}