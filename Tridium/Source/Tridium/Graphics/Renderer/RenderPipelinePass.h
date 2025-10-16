#pragma once
#include <Tridium/Graphics/Renderer/RenderGraph.h>

namespace Tridium {

	//=============================================================================================
	// IRenderPipelinePass Interface:
	// Represents a single rendering operation or stage within the render pipeline.
	//=============================================================================================
	class IRenderPipelinePass
	{
	public:

		//=========================================================================================
		virtual ~IRenderPipelinePass() = default;

		//=========================================================================================
		// Returns the render pipeline that owns this render pass.
		class IRenderPipeline& GetRenderPipeline() const { ASSERT( m_RenderPipeline ); return *m_RenderPipeline; }

		//=========================================================================================
		// Called before the render graph is compiled.
		// Use this to set up render passes, declare resources, and configure the render graph.
		virtual void Setup( RenderGraph& a_RenderGraph ) = 0;

		//=========================================================================================
		// Called if the viewport size has changed.
		// Use this to resize any resources that depend on the viewport size.
		virtual void OnViewportResize( RenderGraph& a_RenderGraph, uint32_t a_Width, uint32_t a_Height ) {};

	private:

		//=========================================================================================
		friend class IRenderPipeline;
		IRenderPipeline* m_RenderPipeline = nullptr;

	};

	//=================================================================================================
	// Root Pipeline Pass: The root pass that imports the output texture and prepares for rendering.
	//=================================================================================================
	class RootPipelinePass : public IRenderPipelinePass
	{
	public:

		//=============================================================================================
		void Setup( RenderGraph& a_RenderGraph ) override;

		//=============================================================================================
		void OnViewportResize( RenderGraph& a_RenderGraph, uint32_t a_Width, uint32_t a_Height ) override;

		//=============================================================================================
		// The ID of the output texture which contains the final rendered scene.
		RenderPassTextureID GetOutputID() const { return m_Output.first; }

		//=============================================================================================
		// The output texture which contains the final rendered scene.
		const RHITextureRef& GetOutputTexture() const { return m_Output.second; }

		//=============================================================================================
		// The ID of the depth texture which contains the final rendered scene.
		RenderPassTextureID GetDepthID() const { return m_Depth.first; }

		//=============================================================================================
		// The depth texture which contains the final rendered scene.
		const RHITextureRef& GetDepthTexture() const { return m_Depth.second; }

	protected:

		//=============================================================================================
		Pair<RenderPassTextureID, RHITextureRef> m_Output;
		Pair<RenderPassTextureID, RHITextureRef> m_Depth;

	};

	//=================================================================================================
	// GBuffer Pipeline Pass: Generates the G-Buffer by rendering scene geometry.
	//=================================================================================================
	class GBufferPipelinePass : public IRenderPipelinePass
	{
	public:

		//=============================================================================================
		void Setup( RenderGraph& a_RenderGraph ) override;

		//=============================================================================================
		void OnViewportResize( RenderGraph& a_RenderGraph, uint32_t a_Width, uint32_t a_Height ) override;

		//=============================================================================================
		// The ID of the position texture which contains world space positions of pixels.
		RenderPassTextureID GetPositionID() const { return m_Position.first; }
		RHITextureRef GetPositionTexture() const { return m_Position.second; }

		//=============================================================================================
		// The ID of the albedo texture which contains base color information of pixels.
		RenderPassTextureID GetAlbedoID() const { return m_Albedo.first; }
		RHITextureRef GetAlbedoTexture() const { return m_Albedo.second; }

		//=============================================================================================
		// The ID of the normal texture which contains world space normals of pixels.
		RenderPassTextureID GetNormalID() const { return m_Normal.first; }
		RHITextureRef GetNormalTexture() const { return m_Normal.second; }

		//=============================================================================================
		// The ID of the metallic-roughness-ambient occlusion texture. The channels are packed as follows:
		// R = Metallic, G = Roughness, B = Ambient Occlusion
		RenderPassTextureID GetMRAOID() const { return m_MetallicRoughnessAO.first; }
		RHITextureRef GetMRAOTexture() const { return m_MetallicRoughnessAO.second; }

		//=============================================================================================
		// The ID of the emission texture which contains emissive color information of pixels.
		RenderPassTextureID GetEmissionID() const { return m_Emission.first; }
		RHITextureRef GetEmissionTexture() const { return m_Emission.second; }

		//=============================================================================================
		// The ID of the depth texture which contains depth information of pixels.
		RenderPassTextureID GetDepthID() const { return m_Depth.first; }
		RHITextureRef GetDepthTexture() const { return m_Depth.second; }

	protected:

		//=============================================================================================
		Pair<RenderPassTextureID, RHITextureRef> m_Position;
		Pair<RenderPassTextureID, RHITextureRef> m_Albedo;
		Pair<RenderPassTextureID, RHITextureRef> m_Normal;
		Pair<RenderPassTextureID, RHITextureRef> m_MetallicRoughnessAO;
		Pair<RenderPassTextureID, RHITextureRef> m_Emission;
		Pair<RenderPassTextureID, RHITextureRef> m_Depth;

	};

	//=================================================================================================
	// Skybox Pipeline Pass: Renders the skybox in the scene.
	//=================================================================================================
	class SkyboxPipelinePass : public IRenderPipelinePass
	{
	public:

		//=============================================================================================
		void Setup( RenderGraph& a_RenderGraph ) override;

		//=============================================================================================
		// The ID of the output texture which contains the scene with the skybox rendered.
		RenderPassTextureID GetOutputID() const { return m_Output; }

	protected:

		//=============================================================================================
		RHIGraphicsPipelineStateRef m_PipelineState;
		RHIBufferRef m_CubeVertexBuffer;
		RHIBufferRef m_CubeIndexBuffer;
		RenderPassTextureID m_Output;
		RenderPassTextureID m_Depth;

	};

}