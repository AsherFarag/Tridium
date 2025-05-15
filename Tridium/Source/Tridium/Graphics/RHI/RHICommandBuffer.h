#pragma once
#include "RHICommon.h"
#include "RHIResource.h"
#include "RHICommandAllocator.h"
#include "RHISampler.h"
#include "RHITexture.h"
#include "RHIVertexLayout.h"
#include "RHIPipelineState.h"
#include "RHIShader.h"
#include "RHIBuffer.h"

namespace Tridium {

	// Forward declarations
	class RHICommandList;
	using RHICommandListRef = SharedPtr<RHICommandList>;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Graphics Command Buffer
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	//=====================================================
	// RHI Graphics Command Type
	//=====================================================
	enum class ERHICommandType : uint8_t
	{
		SetBindingLayout,
		SetShaderBindings,
		SetInlinedConstants,
		ResourceBarrier,
		UpdateBuffer,
		CopyBuffer,
		UpdateTexture,
		CopyTexture,

		// Graphics Commands
		SetGraphicsPipelineState,
		SetRenderTargets,
		ClearRenderTargets,
		SetScissors,
		SetViewports,
		SetIndexBuffer,
		SetVertexBuffer,
		SetPrimitiveTopology,
		Draw,
		DrawIndexed,

		// Compute Commands
		SetComputePipelineState,
		DispatchCompute,
		DispatchComputeIndirect,

		COUNT,

		_GraphicsCommandStart = SetGraphicsPipelineState,
		_GraphicsCommandEnd = DrawIndexed,
		_ComputeCommandStart = SetComputePipelineState,
		_ComputeCommandEnd = DispatchComputeIndirect,
	};

	namespace RHI {
		inline constexpr bool IsGraphicsCommand( ERHICommandType a_Type ) { return a_Type >= ERHICommandType::_GraphicsCommandStart && a_Type <= ERHICommandType::_GraphicsCommandEnd; }
		inline constexpr bool IsComputeCommand( ERHICommandType a_Type ) { return a_Type >= ERHICommandType::_ComputeCommandStart && a_Type <= ERHICommandType::_ComputeCommandEnd; }
		inline constexpr const char* GetCommandName( ERHICommandType a_Type )
		{
			switch ( a_Type )
			{
			case ERHICommandType::SetBindingLayout:         return "SetBindingLayout";
			case ERHICommandType::SetShaderBindings:        return "SetShaderBindings";
			case ERHICommandType::ResourceBarrier:          return "ResourceBarrier";
			case ERHICommandType::UpdateBuffer:             return "UpdateBuffer";
			case ERHICommandType::CopyBuffer:               return "CopyBuffer";
			case ERHICommandType::UpdateTexture:            return "UpdateTexture";
			case ERHICommandType::CopyTexture:              return "CopyTexture";
			// Graphics Commands
			case ERHICommandType::SetGraphicsPipelineState: return "SetGraphicsPipelineState";
			case ERHICommandType::SetRenderTargets:         return "SetRenderTargets";
			case ERHICommandType::ClearRenderTargets:       return "ClearRenderTargets";
			case ERHICommandType::SetScissors:              return "SetScissors";
			case ERHICommandType::SetViewports:             return "SetViewports";
			case ERHICommandType::SetIndexBuffer:           return "SetIndexBuffer";
			case ERHICommandType::SetVertexBuffer:          return "SetVertexBuffer";
			case ERHICommandType::SetPrimitiveTopology:     return "SetPrimitiveTopology";
			case ERHICommandType::Draw:                     return "Draw";
			case ERHICommandType::DrawIndexed:              return "DrawIndexed";
			// Compute Commands
			case ERHICommandType::SetComputePipelineState:  return "SetComputePipelineState";
			case ERHICommandType::DispatchCompute:          return "DispatchCompute";
			case ERHICommandType::DispatchComputeIndirect:  return "DispatchComputeIndirect";
			}
			return "<INVALID>";
		}

		inline constexpr const char* GetCommandNameWithSpaces( ERHICommandType a_Type )
		{
			switch ( a_Type )
			{
			case ERHICommandType::SetBindingLayout:         return "Set Binding Layout";
			case ERHICommandType::SetShaderBindings:        return "Set Shader Bindings";
			case ERHICommandType::ResourceBarrier:          return "Resource Barrier";
			case ERHICommandType::UpdateBuffer:             return "Update Buffer";
			case ERHICommandType::CopyBuffer:               return "Copy Buffer";
			case ERHICommandType::UpdateTexture:            return "Update Texture";
			case ERHICommandType::CopyTexture:              return "Copy Texture";
			// Graphics Commands
			case ERHICommandType::SetGraphicsPipelineState: return "Set Graphics Pipeline State";
			case ERHICommandType::SetRenderTargets:         return "Set Render Targets";
			case ERHICommandType::ClearRenderTargets:       return "Clear Render Targets";
			case ERHICommandType::SetScissors:              return "Set Scissors";
			case ERHICommandType::SetViewports:             return "Set Viewports";
			case ERHICommandType::SetIndexBuffer:           return "Set Index Buffer";
			case ERHICommandType::SetVertexBuffer:          return "Set Vertex Buffer";
			case ERHICommandType::SetPrimitiveTopology:     return "Set Primitive Topology";
			case ERHICommandType::Draw:                     return "Draw";
			case ERHICommandType::DrawIndexed:              return "Draw Indexed";
			// Compute Commands
			case ERHICommandType::SetComputePipelineState:  return "Set Compute Pipeline State";
			case ERHICommandType::DispatchCompute:          return "Dispatch Compute";
			case ERHICommandType::DispatchComputeIndirect:  return "Dispatch Compute Indirect";
			}
			return "<INVALID>";
		}
	}



    //=====================================================
	// RHI Command
	//  A single command that can be executed on the GPU.
	//  This includes both graphics and compute commands.
	//=====================================================
    struct RHICommand
    {
        #pragma region Commands

		#pragma region Base
		struct SetBindingLayout
		{
			RHIBindingLayout* SBL;
		};

		struct SetShaderBindings
		{
			RHIBindingSet* BindingSet;
		};

		struct SetInlinedConstants
		{
			FixedArray<uint8_t, RHIConstants::MaxInlinedConstantsSize> Data;
			RHIBufferRange Range;
		};

		struct ResourceBarrier
		{
			RHIResourceBarrier Barrier;
		};

		struct UpdateBuffer
		{
			RHIBuffer* Buffer;
			Span<const uint8_t> Data;
			size_t Offset;
			ERHIStateTransition StateTransitionMode;
		};

		struct CopyBuffer
		{
			size_t Size;
			RHIBuffer* Source;
			size_t SourceOffset;
			ERHIStateTransition SrcStateTransitionMode;
			RHIBuffer* Destination;
			size_t DestinationOffset;
			ERHIStateTransition DstStateTransitionMode;
		};

		struct UpdateTexture
		{
			RHITexture* Texture;
			uint32_t MipLevel;
			uint32_t ArraySlice;
			Box Region;
			RHITextureSubresourceData Data;
			ERHIStateTransition StateTransitionMode;
		};

		struct CopyTexture
		{
			RHITexture* SrcTexture;
			uint32_t SrcMipLevel;
			uint32_t SrcArraySlice;
			Box SrcRegion;
			ERHIStateTransition SrcStateTransitionMode;
			RHITexture* DstTexture;
			uint32_t DstMipLevel;
			uint32_t DstArraySlice;
			Box DstRegion;
			ERHIStateTransition DstStateTransitionMode;
		};

		#pragma endregion

		#pragma region Graphics

        struct SetGraphicsPipelineState 
        {
            RHIGraphicsPipelineState* PSO;
        };

        struct SetRenderTargets 
        {
            InlineArray<RHITexture*, RHIConstants::MaxColorTargets> RTV;
            RHITexture* DSV;
			ERHIStateTransition StateTransitionMode;
        };

        struct ClearRenderTargets 
        {
			Color ClearColorValues[RHIConstants::MaxColorTargets];
			float DepthValue;
			uint8_t StencilValue;
			EnumFlags<ERHIClearFlags> ClearFlags;
			ERHIStateTransition StateTransitionMode;
        };

        struct SetScissors 
        {
			InlineArray<RHIScissorRect, RHIConstants::MaxColorTargets> Rects;
        };

        struct SetViewports 
        {
			InlineArray<RHIViewport, RHIConstants::MaxColorTargets> Viewports;
        };

        struct SetIndexBuffer 
        {
			RHIBuffer* IBO;
			ERHIStateTransition StateTransitionMode;
        };

        struct SetVertexBuffer 
        {
            RHIBuffer* VBO;
			ERHIStateTransition StateTransitionMode;
        };

        struct SetPrimitiveTopology 
        {
            ERHITopology Topology;
        };

		struct Draw
		{
			uint32_t VertexStart;
			uint32_t VertexCount;
		};

        struct DrawIndexed 
        {
            uint32_t IndexStart;
            uint32_t IndexCount;
        };

		#pragma endregion

		#pragma region Compute

		struct SetComputePipelineState
		{
			NOT_IMPLEMENTED_STATIC;
		};

        struct DispatchCompute 
        {
            uint16_t GroupSize[3];
        };

		struct DispatchComputeIndirect
		{
			NOT_IMPLEMENTED_STATIC;
		};

		#pragma endregion

        #pragma endregion

		Variant<
			SetBindingLayout,
			SetShaderBindings,
			SetInlinedConstants,
			ResourceBarrier,
			UpdateBuffer,
			CopyBuffer,
			UpdateTexture,
			CopyTexture,
			// Graphics
            SetGraphicsPipelineState,
            SetRenderTargets,
            ClearRenderTargets,
            SetScissors,
            SetViewports,
            SetIndexBuffer,
            SetVertexBuffer,
            SetPrimitiveTopology,
			Draw,
            DrawIndexed,
			// Compute
			SetComputePipelineState,
            DispatchCompute,
			DispatchComputeIndirect> Data;

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
			return Cast<ERHICommandType>( Data.index() );
		}

	#if RHI_DEBUG_ENABLED
		struct Debug
		{
			SourceLocation Source;
		} Debug{};
	#endif // RHI_DEBUG_ENABLED
    };


	//=====================================================
	// Helper macros for adding debug information to commands
	// These are undefined at the end of this file.
	#if RHI_DEBUG_ENABLED
		#define DEBUG_INFO , const SourceLocation& a_SourceLocation
		#define DEBUG_INFO_PARAM DEBUG_INFO = SourceLocation::Current()
		#define ADD_DEBUG_INFO() do { Commands.Back().Debug.Source = a_SourceLocation; } while ( false )
		#define PASS_DEBUG_INFO , a_SourceLocation
	#else 
		#define DEBUG_INFO_PARAM
		#define DEBUG_INFO_PARAM_PARAM
		#define ADD_DEBUG_INFO() do {} while ( false )
		#define PASS_DEBUG_INFO
	#endif // RHI_DEBUG_ENABLED


	//=====================================================
	// RHI Base Command Buffer
	//  A buffer that holds a list of commands to be executed on the GPU.
	//  Adding a command does not execute it immediately.
	//  The command buffer must be submitted to a command list via RHICommandList::SetCommands.
	//  To execute the commands, the command list must be submitted to the RHI via RHI::ExecuteCommandList.
	template <typename _CommandBufferType>
	struct RHIBaseCommandBuffer
	{
		using CommandBufferType = _CommandBufferType;

		//=====================================================
		// Set Shader Binding Layout
		// - Sets the shader binding layout for the command buffer.
		CommandBufferType& SetBindingLayout( 
			RHIBindingLayoutRef a_SBL DEBUG_INFO_PARAM );

		//=====================================================
		// Set Shader Bindings
		// - Sets the shader bindings for the command buffer.
		CommandBufferType& SetShaderBindings( 
			RHIBindingSetRef a_BindingSet DEBUG_INFO_PARAM );

		//=====================================================
		// Set Inlined Constants
		// - An offset, in 32-bit values, can be defined to specify where the constants should be written. 
		// - NOTE: The max size of a_Data is defined by RHIConstants::MaxInlinedConstantsSize - a_OffsetIn32Bits.
		CommandBufferType& SetInlinedConstants(
			Span<const uint8_t> a_Data, uint32_t a_OffsetIn32Bits = 0 DEBUG_INFO_PARAM );

		//=====================================================
		// Set Inlined Constants
		// - An offset, in 32-bit values, can be defined to specify where the constants should be written. 
		// - NOTE: The max size of a_Data is defined by RHIConstants::MaxInlinedConstantsSize - a_OffsetIn32Bits.
		template<typename T> requires std::is_pod_v<T>
		CommandBufferType& SetInlinedConstants(
			const T& a_Data, uint32_t a_OffsetIn32Bits = 0 DEBUG_INFO_PARAM );

		//=====================================================
		// Resource Barrier
		// - Adds a resource barrier to the command buffer.
		// - Note: Only useful for manual resource state transitions.
		CommandBufferType& ResourceBarrier( 
			RHIResourceBarrier a_Barrier DEBUG_INFO_PARAM );

		//=====================================================
		// Resource Barrier
		//  Adds a resource barrier to the command buffer.
		//  Note: Only useful for manual resource state transitions.
		CommandBufferType& ResourceBarrier( 
			RHIResource* a_Resource,
			ERHIResourceStates a_Before, ERHIResourceStates a_After DEBUG_INFO_PARAM );

		//=====================================================
		// Update Buffer
		// - Updates the contents of a buffer with the given data.
		// - Note: This can perform a resource state transition if needed.
		CommandBufferType& UpdateBuffer(
			RHIBufferRef a_Buffer,
			Span<const uint8_t> a_Data, size_t a_Offset = 0,
			ERHIStateTransition a_StateTransitionMode = ERHIStateTransition::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Copy Buffer
		// - Copies data from one buffer to another.
		// - Note: This can perform a resource state transition if needed.
		CommandBufferType& CopyBuffer( 
			RHIBufferRef a_SrcBuffer,
			RHIBufferRef a_DstBuffer,
			size_t a_Size, size_t a_SrcOffset = 0, size_t a_DstOffset = 0,
			ERHIStateTransition a_StateTransitionMode = ERHIStateTransition::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Update Texture
		// - Updates the contents of a texture with the given data.
		// - Note: This can perform a resource state transition if needed.
		CommandBufferType& UpdateTexture( 
			RHITextureRef a_Texture,
			uint32_t a_MipLevel, uint32_t a_ArraySlice, Box a_Region,
			const RHITextureSubresourceData& a_Data,
			ERHIStateTransition a_StateTransitionMode = ERHIStateTransition::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Copy Texture
		// - Copies data from one texture to another.
		// - Note: This can perform a resource state transition if needed.
		CommandBufferType& CopyTextureRegion(
			RHITextureRef a_SrcTexture, RHITextureRef a_DstTexture,
			uint32_t a_SrcMipLevel, uint32_t a_SrcArraySlice, Box a_SrcRegion,
			uint32_t a_DstMipLevel, uint32_t a_DstArraySlice, Box a_DstRegion,
			ERHIStateTransition a_SrcStateTransitionMode = ERHIStateTransition::Transition,
			ERHIStateTransition a_DstStateTransitionMode = ERHIStateTransition::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Copy Texture
		// - Copies data from one texture to another.
		// - Note: This can perform a resource state transition if needed.
		CommandBufferType& CopyTexture(
			RHITextureRef a_SrcTexture, RHITextureRef a_DstTexture,
			uint32_t a_SrcMipLevel, uint32_t a_SrcArraySlice,
			uint32_t a_DstMipLevel, uint32_t a_DstArraySlice,
			ERHIStateTransition a_SrcStateTransitionMode = ERHIStateTransition::Transition,
			ERHIStateTransition a_DstStateTransitionMode = ERHIStateTransition::Transition DEBUG_INFO_PARAM );

	public:
		Array<RHICommand> Commands;

	protected:
		UnorderedSet<RHIBindingLayoutRef> m_ShaderBindingLayouts;
		UnorderedSet<RHICommandListRef>   m_CommandLists;
		UnorderedSet<RHITextureRef>       m_Textures;
		UnorderedSet<RHIBufferRef>        m_Buffers;
		UnorderedSet<RHIBindingSetRef>    m_BindingSets;

	protected:
		CommandBufferType& Self() { return Cast<CommandBufferType&>( *this ); }
	};



	//=====================================================
	// RHI Graphics Command Buffer
	//  A buffer that holds a list of graphics commands to be executed on the GPU.
	struct RHIGraphicsCommandBuffer : RHIBaseCommandBuffer<RHIGraphicsCommandBuffer>
	{
		//=====================================================
		// Set Graphics Pipeline State
		// - Sets the graphics pipeline state for the command buffer.
		RHIGraphicsCommandBuffer& SetGraphicsPipelineState( RHIGraphicsPipelineStateRef a_PSO DEBUG_INFO_PARAM );

		//=====================================================
		// Set Index Buffer
		// - Sets the index buffer for the command buffer.
		// - Note: This can perform a resource state transition if needed.
		RHIGraphicsCommandBuffer& SetIndexBuffer( RHIBufferRef a_IBO, ERHIStateTransition a_StateTransitionMode = ERHIStateTransition::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Set Vertex Buffer
		// - Sets the vertex buffer for the command buffer.
		// - Note: This can perform a resource state transition if needed.
		RHIGraphicsCommandBuffer& SetVertexBuffer( RHIBufferRef a_VBO, ERHIStateTransition a_StateTransitionMode = ERHIStateTransition::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Set Render Targets
		// - Sets the render targets for the command buffer.
		// - Note: This can perform a resource state transition if needed.
		RHIGraphicsCommandBuffer& SetRenderTargets( Span<RHITexture*> a_RTV, RHITexture* a_DSV, ERHIStateTransition a_StateTransitionMode = ERHIStateTransition::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Set Render Targets
		// - Sets the render targets for the command buffer.
		// - Note: This can perform a resource state transition if needed.
		RHIGraphicsCommandBuffer& SetRenderTargets( Span<RHITextureRef> a_RTV, RHITexture* a_DSV, ERHIStateTransition a_StateTransitionMode = ERHIStateTransition::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Clear Render Targets
		// - Clears the render targets for the command buffer.
		// - Note: This can perform a resource state transition if needed.
		RHIGraphicsCommandBuffer& ClearRenderTargets( ERHIClearFlags a_Flags, Span<Color> a_ClearColors, float a_DepthValue = 1.0f, uint8_t a_StencilValue = 0u, ERHIStateTransition a_StateTransitionMode = ERHIStateTransition::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Clear Render Targets
		// - Clears the render targets for the command buffer.
		// - Note: This can perform a resource state transition if needed.
		RHIGraphicsCommandBuffer& ClearRenderTargets( ERHIClearFlags a_Flags, Color a_ClearColor, float a_DepthValue = 1.0f, uint8_t a_StencilValue = 0u, ERHIStateTransition a_StateTransitionMode = ERHIStateTransition::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Set Scissors
		// - Sets the scissors, which are used to restrict the rendering area.
		RHIGraphicsCommandBuffer& SetScissors( Span<RHIScissorRect> a_Rects DEBUG_INFO_PARAM );

		//=====================================================
		// Set Viewports
		// - Sets the viewports, which are used to define the rendering area.
		RHIGraphicsCommandBuffer& SetViewports( Span<RHIViewport> a_Viewports DEBUG_INFO_PARAM );

		//=====================================================
		// Set Primitive Topology
		// - Sets the primitive topology, which defines the type of geometry to be rendered.
		RHIGraphicsCommandBuffer& SetPrimitiveTopology( ERHITopology a_Topology DEBUG_INFO_PARAM );

		//=====================================================
		// Draw
		// - Issues a draw call to the GPU.
		RHIGraphicsCommandBuffer& Draw( uint32_t a_VertexStart, uint32_t a_VertexCount DEBUG_INFO_PARAM );

		//=====================================================
		// Draw Indexed
		// - Issues a draw call to the GPU using an index buffer.
		RHIGraphicsCommandBuffer& DrawIndexed( uint32_t a_IndexStart, uint32_t a_IndexCount DEBUG_INFO_PARAM );

	private:
		UnorderedSet<RHIGraphicsPipelineStateRef> m_PipelineStates;
	};



	//=====================================================
	// RHI Compute Command Buffer
	//  A buffer that holds a list of compute commands to be executed on the GPU.
	struct RHIComputeCommandBuffer : RHIBaseCommandBuffer<RHIComputeCommandBuffer>
	{
		//=====================================================
		// Set Compute Pipeline State
		// - Sets the compute pipeline state for the command buffer.
		TODO( "Implement SetComputePipelineState" );

		//=====================================================
		// Dispatch Compute
		// - Issues a dispatch call to the GPU.
		RHIComputeCommandBuffer& DispatchCompute( uint16_t a_GroupSizeX, uint16_t a_GroupSizeY, uint16_t a_GroupSizeZ DEBUG_INFO_PARAM );

		//=====================================================
		// Dispatch Compute Indirect
		// - Issues a dispatch call to the GPU using an indirect buffer.
		RHIComputeCommandBuffer& DispatchComputeIndirect( RHIBufferRef a_Buffer DEBUG_INFO_PARAM );

	private:
		//UnorderedSet<RHIGraphicsPipelineStateRef> m_PipelineStates;
	};



	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// IMPLEMENTATION																					  //
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	//===================================
	// RHI BASE COMMAND BUFFER
	//===================================

	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::SetBindingLayout( 
		RHIBindingLayoutRef a_SBL DEBUG_INFO )
	{
		Commands.EmplaceBack( RHICommand::SetBindingLayout{ a_SBL.get() } );
		m_ShaderBindingLayouts.insert( std::move( a_SBL ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::SetShaderBindings( 
		RHIBindingSetRef a_BindingSet DEBUG_INFO )
	{
		Commands.EmplaceBack( RHICommand::SetShaderBindings( a_BindingSet.get() ) );
		m_BindingSets.insert( std::move( a_BindingSet ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::SetInlinedConstants(
		Span<const uint8_t> a_Data, uint32_t a_OffsetIn32Bits DEBUG_INFO )
	{
		const uint32_t offsetInBytes = a_OffsetIn32Bits * sizeof( uint32_t );
		RHI_DEV_CHECK( a_Data.size_bytes() <= RHIConstants::MaxInlinedConstantsSize - offsetInBytes, "Inlined constants size exceeds the maximum size!" );
		Commands.EmplaceBack( RHICommand::SetInlinedConstants( a_Data, RHIBufferRange( offsetInBytes, a_Data.size_bytes() ) ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	template<typename _CommandBufferType>
	template<typename T> requires std::is_pod_v<T>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::SetInlinedConstants(
		const T& a_Data, uint32_t a_OffsetIn32Bits DEBUG_INFO )
	{
		const uint32_t offsetInBytes = a_OffsetIn32Bits * sizeof( uint32_t );
		RHI_DEV_CHECK( sizeof( T ) <= RHIConstants::MaxInlinedConstantsSize - offsetInBytes, "Inlined constants size exceeds the maximum size!" );
		Commands.EmplaceBack( RHICommand::SetInlinedConstants( Span<const uint8_t>( ReinterpretCast<const uint8_t*>(&a_Data), sizeof( T ) ), RHIBufferRange( offsetInBytes, sizeof( T ) ) ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::ResourceBarrier( 
		RHIResourceBarrier a_Barrier DEBUG_INFO )
	{
		RHI_DEV_CHECK( a_Barrier.Resource || a_Barrier.Resource->IsValid(), "Failed to create a resource barrier as the resource is invalid!" );

		switch ( a_Barrier.Resource->GetType() )
		{
		case ERHIResourceType::Texture:
			m_Textures.insert( a_Barrier.Resource->As<RHITexture>()->SharedFromThis() );
			break;
		case ERHIResourceType::Buffer:
			m_Buffers.insert( a_Barrier.Resource->As<RHIBuffer>()->SharedFromThis() );
			break;
		default:
			ASSERT( false, "Can only set resource barriers for textures and buffers!" );
			return Self();
		}

		Commands.EmplaceBack( RHICommand::ResourceBarrier( a_Barrier ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::ResourceBarrier( 
		RHIResource* a_Resource, ERHIResourceStates a_Before, ERHIResourceStates a_After DEBUG_INFO )
	{
		return ResourceBarrier( RHIResourceBarrier( a_Resource, a_Before, a_After ) PASS_DEBUG_INFO );
	}

	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::UpdateBuffer( 
		RHIBufferRef a_Buffer, Span<const uint8_t> a_Data, size_t a_Offset, ERHIStateTransition a_StateTransitionMode DEBUG_INFO )
	{
		Commands.EmplaceBack( RHICommand::UpdateBuffer( a_Buffer.get(), a_Data, a_Offset, a_StateTransitionMode ) );
		m_Buffers.insert( std::move( a_Buffer ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::CopyBuffer(
		RHIBufferRef a_SrcBuffer, RHIBufferRef a_DstBuffer, size_t a_Size, size_t a_SrcOffset, size_t a_DstOffset, ERHIStateTransition a_StateTransitionMode DEBUG_INFO )
	{
		Commands.EmplaceBack( RHICommand::CopyBuffer( a_SrcBuffer.get(), a_DstBuffer.get(), a_Size, a_SrcOffset, a_StateTransitionMode ) );
		m_Buffers.insert( std::move( a_SrcBuffer ) );
		m_Buffers.insert( std::move( a_DstBuffer ) );
		ADD_DEBUG_INFO();
		return Self();
	}


	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::UpdateTexture(
		RHITextureRef a_Texture, uint32_t a_MipLevel, uint32_t a_ArraySlice, Box a_Region, const RHITextureSubresourceData& a_Data, ERHIStateTransition a_StateTransitionMode DEBUG_INFO )
	{
		Commands.EmplaceBack( RHICommand::UpdateTexture( a_Texture.get(), a_MipLevel, a_ArraySlice, a_Region, a_Data, a_StateTransitionMode ) );
		m_Textures.insert( std::move( a_Texture ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::CopyTextureRegion(
		RHITextureRef a_SrcTexture, RHITextureRef a_DstTexture,
		uint32_t a_SrcMipLevel, uint32_t a_SrcArraySlice, Box a_SrcRegion,
		uint32_t a_DstMipLevel, uint32_t a_DstArraySlice, Box a_DstRegion,
		ERHIStateTransition a_SrcStateTransitionMode, ERHIStateTransition a_DstStateTransitionMode
		DEBUG_INFO )
	{
		RHICommand::CopyTexture& cmd = Commands.EmplaceBack( RHICommand::CopyTexture() ).Get<RHICommand::CopyTexture>();
		cmd.SrcTexture = a_SrcTexture.get();
		cmd.SrcMipLevel = a_SrcMipLevel;
		cmd.SrcArraySlice = a_SrcArraySlice;
		cmd.SrcRegion = a_SrcRegion;
		cmd.SrcStateTransitionMode = a_SrcStateTransitionMode;
		cmd.DstTexture = a_DstTexture.get();
		cmd.DstMipLevel = a_DstMipLevel;
		cmd.DstArraySlice = a_DstArraySlice;
		cmd.DstRegion = a_DstRegion;
		cmd.DstStateTransitionMode = a_DstStateTransitionMode;

		m_Textures.insert( std::move( a_SrcTexture ) );
		m_Textures.insert( std::move( a_DstTexture ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::CopyTexture( 
		RHITextureRef a_SrcTexture, RHITextureRef a_DstTexture,
		uint32_t a_SrcMipLevel, uint32_t a_SrcArraySlice,
		uint32_t a_DstMipLevel, uint32_t a_DstArraySlice,
		ERHIStateTransition a_SrcStateTransitionMode, ERHIStateTransition a_DstStateTransitionMode
		DEBUG_INFO )
	{
		RHICommand::CopyTexture& cmd = Commands.EmplaceBack( RHICommand::CopyTexture() ).Get<RHICommand::CopyTexture>();
		cmd.SrcTexture = a_SrcTexture.get();
		cmd.SrcMipLevel = a_SrcMipLevel;
		cmd.SrcArraySlice = a_SrcArraySlice;
		cmd.SrcStateTransitionMode = a_SrcStateTransitionMode;
		cmd.DstTexture = a_DstTexture.get();
		cmd.DstMipLevel = a_DstMipLevel;
		cmd.DstArraySlice = a_DstArraySlice;
		cmd.DstStateTransitionMode = a_DstStateTransitionMode;

		// Set the regions to the entire texture
		cmd.SrcRegion = Box(
			0, a_SrcTexture->Descriptor().Width,
			0, a_SrcTexture->Descriptor().Height,
			0, a_SrcTexture->Descriptor().Depth
		);
		cmd.DstRegion = Box(
			0, a_DstTexture->Descriptor().Width,
			0, a_DstTexture->Descriptor().Height,
			0, a_DstTexture->Descriptor().Depth
		);

		m_Textures.insert( std::move( a_SrcTexture ) );
		m_Textures.insert( std::move( a_DstTexture ) );
		ADD_DEBUG_INFO();
		return Self();
	}



	//===================================
	// RHI GRAPHICS COMMAND BUFFER
	//===================================

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::SetGraphicsPipelineState( 
		RHIGraphicsPipelineStateRef a_PSO DEBUG_INFO )
	{
		Commands.EmplaceBack( RHICommand::SetGraphicsPipelineState{ a_PSO.get() } );
		m_PipelineStates.insert( std::move( a_PSO ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::SetIndexBuffer(
		RHIBufferRef a_IBO, ERHIStateTransition a_StateTransitionMode DEBUG_INFO )
	{
		RHI_DEV_CHECK( EnumFlags( a_IBO->Descriptor().BindFlags ).HasFlag( ERHIBindFlags::IndexBuffer ), "Buffer is not an index buffer!" );
		RHI_DEV_CHECK( a_IBO->Descriptor().Size > 0, "Buffer size is zero!" );

		Commands.EmplaceBack( RHICommand::SetIndexBuffer{ a_IBO.get(), a_StateTransitionMode } );
		m_Buffers.insert( std::move( a_IBO ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::SetVertexBuffer( 
		RHIBufferRef a_VBO, ERHIStateTransition a_StateTransitionMode DEBUG_INFO )
	{
		RHI_DEV_CHECK( EnumFlags( a_VBO->Descriptor().BindFlags ).HasFlag( ERHIBindFlags::VertexBuffer ), "Buffer is not a vertex buffer!" );
		RHI_DEV_CHECK( a_VBO->Descriptor().Size > 0, "Buffer size is zero!" );

		Commands.EmplaceBack( RHICommand::SetVertexBuffer{ a_VBO.get(), a_StateTransitionMode } );
		m_Buffers.insert( std::move( a_VBO ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::SetRenderTargets( 
		Span<RHITexture*> a_RTV, RHITexture* a_DSV, ERHIStateTransition a_StateTransitionMode DEBUG_INFO )
	{
		RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetRenderTargets() );
		RHICommand::SetRenderTargets& data = cmd.Get<RHICommand::SetRenderTargets>();
		data.StateTransitionMode = a_StateTransitionMode;

		if ( a_DSV )
		{
			data.DSV = a_DSV;
			m_Textures.insert( a_DSV->SharedFromThis() );
		}

		for ( size_t i = 0; i < a_RTV.size() && i < data.RTV.MaxSize(); ++i )
		{
			if ( !a_RTV[i] || !a_RTV[i]->IsValid() )
			{
				ASSERT( false, "Render target is null!" );
				continue;
			}

			if ( !EnumFlags( a_RTV[i]->Descriptor().BindFlags ).HasFlag( ERHIBindFlags::RenderTarget ) )
			{
				ASSERT( false, "Texture is not a render target!" );
				continue;
			}

			data.RTV.PushBack( a_RTV[i] );
			m_Textures.insert( a_RTV[i]->SharedFromThis() );
		}

		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::SetRenderTargets( 
		Span<RHITextureRef> a_RTV, RHITexture* a_DSV, ERHIStateTransition a_StateTransitionMode DEBUG_INFO )
	{
		RHITexture* rtvs[RHIConstants::MaxColorTargets];
		for ( size_t i = 0; i < a_RTV.size() && i < RHIConstants::MaxColorTargets; ++i )
		{
			rtvs[i] = a_RTV[i].get();
		}
		return SetRenderTargets( Span<RHITexture*>{ rtvs, a_RTV.size() }, a_DSV, a_StateTransitionMode PASS_DEBUG_INFO );
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::ClearRenderTargets( 
		ERHIClearFlags a_Flags, Span<Color> a_ClearColors, float a_DepthValue, uint8_t a_StencilValue, ERHIStateTransition a_StateTransitionMode DEBUG_INFO )
	{
		RHICommand& cmd = Commands.EmplaceBack( RHICommand::ClearRenderTargets() );
		RHICommand::ClearRenderTargets& data = cmd.Get<RHICommand::ClearRenderTargets>();
		data.ClearFlags = a_Flags;
		data.DepthValue = a_DepthValue;
		data.StencilValue = a_StencilValue;
		data.StateTransitionMode = a_StateTransitionMode;
		for ( size_t i = 0; i < a_ClearColors.size() && i < RHIConstants::MaxColorTargets; ++i )
		{
			data.ClearColorValues[i] = a_ClearColors[i];
		}
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::ClearRenderTargets( 
		ERHIClearFlags a_Flags, Color a_ClearColor, float a_DepthValue, uint8_t a_StencilValue, ERHIStateTransition a_StateTransitionMode DEBUG_INFO )
	{
		RHICommand& cmd = Commands.EmplaceBack( RHICommand::ClearRenderTargets() );
		RHICommand::ClearRenderTargets& data = cmd.Get<RHICommand::ClearRenderTargets>();
		data.ClearFlags = a_Flags;
		data.DepthValue = a_DepthValue;
		data.StencilValue = a_StencilValue;
		data.StateTransitionMode = a_StateTransitionMode;
		for ( size_t i = 0; i < RHIConstants::MaxColorTargets; ++i )
		{
			data.ClearColorValues[i] = a_ClearColor;
		}
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::SetScissors( Span<RHIScissorRect>a_Rects DEBUG_INFO )
	{
		RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetScissors() );
		RHICommand::SetScissors& data = cmd.Get<RHICommand::SetScissors>();
		data.Rects.Resize( a_Rects.size() );
		std::memcpy( data.Rects.Data(), a_Rects.data(), a_Rects.size() * sizeof( RHIScissorRect ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::SetViewports( Span<RHIViewport>a_Viewports DEBUG_INFO )
	{
		RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetViewports() );
		RHICommand::SetViewports& data = cmd.Get<RHICommand::SetViewports>();
		data.Viewports.Resize( a_Viewports.size() );
		std::memcpy( data.Viewports.Data(), a_Viewports.data(), a_Viewports.size() * sizeof( RHIViewport ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::SetPrimitiveTopology( ERHITopology a_Topology DEBUG_INFO )
	{
		Commands.EmplaceBack( RHICommand::SetPrimitiveTopology{ a_Topology } );
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::Draw( uint32_t a_VertexStart, uint32_t a_VertexCount DEBUG_INFO )
	{
		RHICommand& cmd = Commands.EmplaceBack( RHICommand::Draw() );
		RHICommand::Draw& data = cmd.Get<RHICommand::Draw>();
		data.VertexStart = a_VertexStart;
		data.VertexCount = a_VertexCount;
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::DrawIndexed( uint32_t a_IndexStart, uint32_t a_IndexCount DEBUG_INFO )
	{
		RHICommand& cmd = Commands.EmplaceBack( RHICommand::DrawIndexed() );
		RHICommand::DrawIndexed& data = cmd.Get<RHICommand::DrawIndexed>();
		data.IndexStart = a_IndexStart;
		data.IndexCount = a_IndexCount;
		ADD_DEBUG_INFO();
		return Self();
	}



	//===================================
	// RHI COMPUTE COMMAND BUFFER
	//===================================

	inline RHIComputeCommandBuffer& RHIComputeCommandBuffer::DispatchCompute( uint16_t a_GroupSizeX, uint16_t a_GroupSizeY, uint16_t a_GroupSizeZ DEBUG_INFO )
	{
		RHICommand& cmd = Commands.EmplaceBack( RHICommand::DispatchCompute() );
		RHICommand::DispatchCompute& data = cmd.Get<RHICommand::DispatchCompute>();
		data.GroupSize[0] = a_GroupSizeX;
		data.GroupSize[1] = a_GroupSizeY;
		data.GroupSize[2] = a_GroupSizeZ;
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIComputeCommandBuffer& RHIComputeCommandBuffer::DispatchComputeIndirect( RHIBufferRef a_Buffer DEBUG_INFO )
	{
		Commands.EmplaceBack( RHICommand::DispatchComputeIndirect{} );
		ADD_DEBUG_INFO();
		return Self();
	}

#undef DEBUG_INFO_PARAM
#undef ADD_DEBUG_INFO
#undef PASS_DEBUG_INFO

} // namespace Tridium