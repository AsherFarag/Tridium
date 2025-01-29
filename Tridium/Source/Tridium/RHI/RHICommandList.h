#pragma once
#include "RHICommon.h"
#include "RHIResource.h"
#include "RHITexture.h"
#include "RHIMesh.h"

namespace Tridium {

	// A fence that can be used to synchronize the CPU and GPU.
    enum class RHIFence : int32_t {};

	// The state of a fence.
	enum class ERHIFenceState : uint8_t
	{
		Pending,  // The fence has not been signaled yet.
		Complete, // The fence has been signaled.
		Unknown   // The state of the fence is unknown.
	};

	enum class ERHICommandType : uint8_t
	{
        SetPipelineState,
        SetRenderTargets,
        SetClearValues,
        ClearRenderTargets,
        SetViewports,
        SetScissors,
        SetShaderInput,
        SetIndexBuffer,
        SetVertexBuffer,
        SetPrimitiveTopology,
        DrawIndexed,
        DispatchCompute,
        FenceSignal,
        FenceWait,
        Execute,
		COUNT,
		Unknown,
	};
    
	//======================================================================================================
    // RHIShaderInputPayload
    //  A structure that holds shader input data,
    //  including a count of inputs and either an array of references to textures or inline data.
	//======================================================================================================
	struct RHIShaderInputPayload
	{
		uint8_t Count;
		union Payload
		{
			void* Data;
            uint32_t InlineData[sizeof( Data ) / sizeof( uint32_t )];
		} Payload;
	};

    //=====================================================
	// RHICommand
	//  A single command that can be executed on the GPU.
	//=====================================================
    struct RHICommand
    {
		ERHICommandType Type = ERHICommandType::Unknown;
        union Payload
        {
            struct SetPipelineState {
                RHIPipelineState* PSO;
            } SetPipelineState;

            struct SetRenderTargets {
                RHITexture* RTV[RHIQuery::MaxColourTargets];
                RHITexture* DSV;
            } SetRenderTargets;

            struct SetClearValues {
                float   Colour[4];
                float   Depth;
                uint8_t Stencil;
            } SetClearValues;

            struct ClearRenderTargets {
                uint32_t ColourTargets : RHIQuery::MaxColourTargets;
                bool     DepthBit : 1;
                bool     StencilBit : 1;
            } ClearRenderTargets;

            struct SetScissors {
                uint16_t Rects[RHIQuery::MaxColourTargets * ( sizeof( uint16_t ) * 4 )];
            } SetScissors;

            struct SetViewports {
                float Viewports[RHIQuery::MaxColourTargets * 6 * sizeof( float )];
            } SetViewports;

            struct SetShaderInput {
                uint8_t Index;
                RHIShaderInputPayload Payload;
            } SetShaderInput;

            struct SetIndexBuffer {
                RHIIndexBuffer* Ibo;
            } SetIndexBuffer;

            struct SetVertexBuffer {
                RHIVertexBuffer* Vbo;
            } SetVertexBuffer;

            struct SetPrimitiveTopology {
                ERHITopology Topology;
            } SetPrimitiveTopology;

            struct DrawIndexed {
                uint32_t IndexStart;
                uint32_t IndexCount;
            } DrawIndexed;

            struct DispatchCompute {
                uint16_t GroupSize[3];
            } DispatchCompute;

            struct FenceSignal {
                RHIFence Fence;
            } FenceSignal;

            struct FenceWait {
                RHIFence Fence;
            } FenceWait;

            struct Execute {
				RHICommandList* CommandList;
            } Execute;
		} Payload;

#if RHI_DEBUG_ENABLED
		struct DebugInfo
		{
			const char* Function = nullptr;
			const char* File = nullptr;
			uint32_t Line = 0;
		} Debug;
#endif // RHI_DEBUG_ENABLED
    };

	struct RHICommandBuffer
	{

		Array<RHICommand> Commands;
	private:
		UnorderedSet<
	};

	RHI_RESOURCE_BASE_TYPE( CommandList,
		virtual bool SetCommands();
	)
	{

	};

}