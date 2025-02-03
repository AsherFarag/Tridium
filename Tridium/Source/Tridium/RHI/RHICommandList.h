#pragma once
#include "RHICommon.h"
#include "RHIResource.h"
#include "RHICommandAllocator.h"
#include "RHITexture.h"
#include "RHIMesh.h"
#include "RHIPipelineState.h"

namespace Tridium {

	// Forward declarations
	class RHICommandList;
	using RHICommandListRef = SharedPtr<RHICommandList>;
    
	//======================================================================================================
    // RHI Shader Input Payload
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
	// RHI Command Type
	//=====================================================
	enum class ERHICommandType : uint8_t
	{
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
		Execute,
		COUNT,
	};

    //=====================================================
	// RHI Command
	//  A single command that can be executed on the GPU.
	//=====================================================
    struct RHICommand
    {
        #pragma region Commands

        struct SetPipelineState 
        {
            RHIPipelineStateRef PSO;
        };

        struct SetRenderTargets 
        {
            FixedArray<RHITextureRef, RHIQuery::MaxColourTargets> RTV;
            RHITextureRef DSV;
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
            RHIIndexBufferRef IBO;
        };

        struct SetVertexBuffer 
        {
            RHIVertexBufferRef VBO;
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
            RHICommandListRef CommandList;
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

		template <ERHICommandType T>
		auto& Get()
		{
			return std::get<std::underlying_type_t<ERHICommandType>(T)>(Data);
		}

		template <ERHICommandType T>
		const auto& Get() const
		{
			return std::get<std::underlying_type_t<ERHICommandType>( T )>( Data );
		}

		template <typename T>
		bool Is() const
		{
			return std::holds_alternative<T>( Data );
		}

		ERHICommandType Type() const
		{
			return static_cast<ERHICommandType>( Data.index() );
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

#if RHI_DEBUG_ENABLED

	// Helper function that enqueues a command with debug information.
#define RHICommandEnqueue( _CommandBuffer, _Command, ... ) \
        (_CommandBuffer)._Command( __VA_ARGS__ ); \
	    (_CommandBuffer).Commands.Back().Debug = { __FUNCTION__, __FILE__, __LINE__ }
#else
	// Helper function that enqueues a command.
    #define RHICommandEnqueue( _CommandBuffer, _Command, ... ) \
        (_CommandBuffer)._Command( __VA_ARGS__ )
#endif // RHI_DEBUG_ENABLED


	//=====================================================
	// RHICommandBuffer
	//  A buffer that holds an array of commands to be executed on the GPU.
	//=====================================================
	struct RHICommandBuffer
	{
		Array<RHICommand> Commands;

		//=====================================================

		void SetPipelineState( RHIPipelineStateRef a_PSO )
		{
            Commands.EmplaceBack( RHICommand::SetPipelineState{ std::move( a_PSO ) } );
		}

		void SetRenderTargets( const Span<RHITextureRef>& a_RTV, RHITextureRef a_DSV )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetRenderTargets() );
			RHICommand::SetRenderTargets& data = cmd.Get<RHICommand::SetRenderTargets>();

			data.DSV = std::move( a_DSV );
			for ( size_t i = 0; i < a_RTV.size() && i < data.RTV.MaxSize(); ++i )
			{
				data.RTV[i] = a_RTV[i];
			}
		}

		void SetClearValues( const float a_Colour[4], float a_Depth, uint8_t a_Stencil )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetClearValues() );
			RHICommand::SetClearValues& data = cmd.Get<RHICommand::SetClearValues>();
			memcpy( data.Colour, a_Colour, sizeof( data.Colour ) );
			data.Depth = a_Depth;
			data.Stencil = a_Stencil;
		}

		void ClearRenderTargets( uint32_t a_ColourTargets, bool a_DepthBit, bool a_StencilBit )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::ClearRenderTargets() );
			RHICommand::ClearRenderTargets& data = cmd.Get<RHICommand::ClearRenderTargets>();
			data.ColourTargets = a_ColourTargets;
			data.DepthBit = a_DepthBit;
			data.StencilBit = a_StencilBit;
		}

		void SetScissors( const Span<uint16_t>& a_Rects )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetScissors() );
			RHICommand::SetScissors& data = cmd.Get<RHICommand::SetScissors>();
			memcpy( data.Rects.Data(), a_Rects.data(), a_Rects.size() );
		}

		void SetViewports( const Span<float>& a_Viewports )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetViewports() );
			RHICommand::SetViewports& data = cmd.Get<RHICommand::SetViewports>();
			memcpy( data.Viewports.Data(), a_Viewports.data(), a_Viewports.size() );
		}

		template<typename T>
		void SetShaderInput( uint32_t a_Index, T a_Value )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetShaderInput() );
			RHICommand::SetShaderInput& data = cmd.Get<RHICommand::SetShaderInput>();
			data.Index = a_Index;
			data.Payload.Count = 1;

			TODO( "Implement SetShaderInput" );
		}

		void SetIndexBuffer( RHIIndexBufferRef a_IBO )
		{
			Commands.EmplaceBack( RHICommand::SetIndexBuffer{ std::move( a_IBO ) } );
		}

		void SetVertexBuffer( RHIVertexBufferRef a_VBO )
		{
			Commands.EmplaceBack( RHICommand::SetVertexBuffer{ std::move( a_VBO ) } );
		}

		void SetPrimitiveTopology( ERHITopology a_Topology )
		{
			Commands.EmplaceBack( RHICommand::SetPrimitiveTopology{ a_Topology } );
		}

		void DrawIndexed( uint32_t a_IndexStart, uint32_t a_IndexCount )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::DrawIndexed() );
			RHICommand::DrawIndexed& data = cmd.Get<RHICommand::DrawIndexed>();
			data.IndexStart = a_IndexStart;
			data.IndexCount = a_IndexCount;
		}

		void DispatchCompute( uint16_t a_GroupSizeX, uint16_t a_GroupSizeY, uint16_t a_GroupSizeZ )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::DispatchCompute() );
			RHICommand::DispatchCompute& data = cmd.Get<RHICommand::DispatchCompute>();
			data.GroupSize[0] = a_GroupSizeX;
			data.GroupSize[1] = a_GroupSizeY;
			data.GroupSize[2] = a_GroupSizeZ;
		}

		void FenceSignal( RHIFence a_Fence )
		{
			Commands.EmplaceBack( RHICommand::FenceSignal{ a_Fence } );
		}
	
		void FenceWait( RHIFence a_Fence )
		{
			Commands.EmplaceBack( RHICommand::FenceWait{ a_Fence } );
		}
	};

	RHI_RESOURCE_BASE_TYPE( CommandList,
		virtual bool SetCommands( const RHICommandBuffer& a_CmdBuffer ) { return false; }
	)
	{
		RHICommandBuffer Commands;
		RHICommandAllocatorRef Allocator;
	};

}