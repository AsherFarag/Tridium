#pragma once
#include "RHICommon.h"
#include "RHIResource.h"
#include "RHITexture.h"
#include "RHIMesh.h"
#include "RHIPipelineState.h"

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
        #pragma region Commands

        struct SetPipelineState 
        {
            RHIPipelineState* PSO;
        };

        struct SetRenderTargets 
        {
            RHITexture* RTV[RHIQuery::MaxColourTargets];
            RHITexture* DSV;
        };

        struct SetClearValues 
        {
            float   Colour[4];
            float   Depth;
            uint8_t Stencil;
        };

        struct ClearRenderTargets 
        {
            uint32_t ColourTargets : RHIQuery::MaxColourTargets;
            bool     DepthBit : 1;
            bool     StencilBit : 1;
        };

        struct SetScissors 
        {
			Array<uint16_t> Rects{ RHIQuery::MaxColourTargets * ( sizeof( uint16_t ) * 4 ) };
        };

        struct SetViewports 
        {
			Array<float> Viewports{ RHIQuery::MaxColourTargets * 6 * sizeof( float ) };
        };

        struct SetShaderInput 
        {
            uint8_t Index;
            RHIShaderInputPayload Payload;
        };

        struct SetIndexBuffer 
        {
            RHIIndexBuffer* IBO;
        };

        struct SetVertexBuffer 
        {
            RHIVertexBuffer* VBO;
        };

        struct SetPrimitiveTopology 
        {
            ERHITopology Topology;
        };

        struct DrawIndexed 
        {
            uint32_t IndexStart;
            uint32_t IndexCount;
        };

        struct DispatchCompute 
        {
            uint16_t GroupSize[3];
        };

        struct FenceSignal 
        {
            RHIFence Fence;
        };

        struct FenceWait 
        {
            RHIFence Fence;
        };

        struct Execute 
        {
            RHICommandList* CommandList;
        };

        #pragma endregion

		Variant<
            SetPipelineState,
            SetRenderTargets,
            SetClearValues,
            ClearRenderTargets,
            SetScissors,
            SetViewports,
            SetShaderInput,
            SetIndexBuffer,
            SetVertexBuffer,
            SetPrimitiveTopology,
            DrawIndexed,
            DispatchCompute,
            FenceSignal,
            FenceWait,
            Execute> Data;

        template <typename T>
		T& Get()
		{
			return std::get<T>( Data );
		}

		template <typename T>
		const T& Get() const
		{
			return std::get<T>( Data );
		}

		template <typename T>
		bool Is() const
		{
			return std::holds_alternative<T>( Data );
		}

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
	};

	RHI_RESOURCE_BASE_TYPE( CommandList,
		virtual bool SetCommands();
	)
	{

	};

}