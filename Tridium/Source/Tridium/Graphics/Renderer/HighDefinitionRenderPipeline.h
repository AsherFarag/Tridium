#pragma once
#include <Tridium/Graphics/Renderer/RenderPipeline.h>

namespace Tridium {

	//=============================================================================================
	// High-Definition Render Pipeline (HDRP):
	// An advanced rendering pipeline that supports high-fidelity graphics and effects.
	// Uses physically based rendering (PBR) techniques, advanced lighting models,
	// and post-processing effects to achieve realistic visuals.
	//=============================================================================================
	class HighDefinitionRenderPipeline : public IRenderPipeline
	{
	public:

		struct Passes : IRenderPipeline::Passes
		{
			static constexpr HashedString GBuffer = "GBuffer"_H;
			static constexpr HashedString Lighting = "Lighting"_H;
			static constexpr HashedString Transparent = "Transparent"_H;
			static constexpr HashedString Skybox = "Skybox"_H;
			static constexpr HashedString PostProcess = "PostProcess"_H;
		};

		//=========================================================================================
		HighDefinitionRenderPipeline() = default;
		~HighDefinitionRenderPipeline() override = default;

		//=========================================================================================
		bool Setup() override;

	protected:

		//=========================================================================================
		UniquePtr<class IRenderPipeline> Create() const override 
		{ 
			return MakeUnique<HighDefinitionRenderPipeline>();
		}

		//=========================================================================================
		RHIFenceValue Render( const RenderContext& a_Context, RenderViewList a_Views ) override;

	protected:

		//=========================================================================================
		RHICommandListRef m_GfxCmdList;

	};

	//=================================================================================================
	// High-Definition Lighting Pass: Performs PBR lighting calculations using G-Buffer data.
	//=================================================================================================
	class HDLightingPipelinePass : public IRenderPipelinePass
	{
	public:

		//=============================================================================================
		void Setup( RenderGraph& a_RenderGraph ) override;

		//=============================================================================================
		// The ID of the output texture which contains the final lit scene.
		RenderPassTextureID GetOutputID() const { return m_Output; }

	protected:

		//=============================================================================================
		//RHIGraphicsPipelineStateRef m_PipelineState;
		RHIBufferRef m_QuadVertexBuffer;
		RHIBufferRef m_QuadIndexBuffer;
		RenderPassTextureID m_Output;

	};

}