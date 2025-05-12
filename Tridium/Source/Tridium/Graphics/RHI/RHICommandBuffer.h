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
    
	//======================================================================================================
    // RHI Shader Input Payload
    //  A structure that holds shader input data,
    //  including a count of inputs and either an array of references to textures or inline data.
	//======================================================================================================
	struct RHIShaderInputPayload
	{
		uint8_t Count;
		bool IsReference;
		union
		{
			void* References[RHIConstants::MaxTextureBindings];
			uint8_t InlineData[sizeof( References )];
		};
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Graphics Command Buffer
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	//=====================================================
	// RHI Graphics Command Type
	//=====================================================
	enum class ERHICommandType : uint8_t
	{
		SetShaderBindingLayout,
		SetShaderInput,
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
			case ERHICommandType::SetShaderBindingLayout:   return "SetShaderBindingLayout";
			case ERHICommandType::SetShaderInput:           return "SetShaderInput";
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
			return "Unknown";
		}

		inline constexpr const char* GetCommandNameWithSpaces( ERHICommandType a_Type )
		{
			switch ( a_Type )
			{
			case ERHICommandType::SetShaderBindingLayout:   return "Set Shader Binding Layout";
			case ERHICommandType::SetShaderInput:           return "Set Shader Input";
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
			return "Unknown";
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
		struct SetShaderBindingLayout
		{
			RHIBindingLayout* SBL;
		};

		struct SetShaderInput
		{
			hash_t NameHash;
			RHIShaderInputPayload Payload;
			ERHIResourceStateTransitionMode StateTransitionMode;
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
			ERHIResourceStateTransitionMode StateTransitionMode;
		};

		struct CopyBuffer
		{
			size_t Size;
			RHIBuffer* Source;
			size_t SourceOffset;
			ERHIResourceStateTransitionMode SrcStateTransitionMode;
			RHIBuffer* Destination;
			size_t DestinationOffset;
			ERHIResourceStateTransitionMode DstStateTransitionMode;
		};

		struct UpdateTexture
		{
			RHITexture* Texture;
			uint32_t MipLevel;
			uint32_t ArraySlice;
			Box Region;
			RHITextureSubresourceData Data;
			ERHIResourceStateTransitionMode StateTransitionMode;
		};

		struct CopyTexture
		{
			RHITexture* SrcTexture;
			uint32_t SrcMipLevel;
			uint32_t SrcArraySlice;
			Box SrcRegion;
			ERHIResourceStateTransitionMode SrcStateTransitionMode;
			RHITexture* DstTexture;
			uint32_t DstMipLevel;
			uint32_t DstArraySlice;
			Box DstRegion;
			ERHIResourceStateTransitionMode DstStateTransitionMode;
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
			ERHIResourceStateTransitionMode StateTransitionMode;
        };

        struct ClearRenderTargets 
        {
			Color ClearColorValues[RHIConstants::MaxColorTargets];
			float DepthValue;
			uint8_t StencilValue;
			EnumFlags<ERHIClearFlags> ClearFlags;
			ERHIResourceStateTransitionMode StateTransitionMode;
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
			ERHIResourceStateTransitionMode StateTransitionMode;
        };

        struct SetVertexBuffer 
        {
            RHIBuffer* VBO;
			ERHIResourceStateTransitionMode StateTransitionMode;
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
			SetShaderBindingLayout,
			SetShaderInput,
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
			return static_cast<ERHICommandType>( Data.index() );
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
		CommandBufferType& SetShaderBindingLayout( RHIBindingLayoutRef a_SBL DEBUG_INFO_PARAM );

		//=====================================================
		// Set Shader Input
		// - Will attempt to set the shader variable, with the given name found in the shader binding layout, to the given value.
		template<typename T>
		CommandBufferType& SetShaderInput( 
			HashedString a_Name, T a_Value,
			ERHIResourceStateTransitionMode a_StateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

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
			ERHIResourceStateTransitionMode a_StateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Copy Buffer
		// - Copies data from one buffer to another.
		// - Note: This can perform a resource state transition if needed.
		CommandBufferType& CopyBuffer( 
			RHIBufferRef a_SrcBuffer,
			RHIBufferRef a_DstBuffer,
			size_t a_Size, size_t a_SrcOffset = 0, size_t a_DstOffset = 0,
			ERHIResourceStateTransitionMode a_StateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Update Texture
		// - Updates the contents of a texture with the given data.
		// - Note: This can perform a resource state transition if needed.
		CommandBufferType& UpdateTexture( 
			RHITextureRef a_Texture,
			uint32_t a_MipLevel, uint32_t a_ArraySlice, Box a_Region,
			const RHITextureSubresourceData& a_Data,
			ERHIResourceStateTransitionMode a_StateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Copy Texture
		// - Copies data from one texture to another.
		// - Note: This can perform a resource state transition if needed.
		CommandBufferType& CopyTextureRegion(
			RHITextureRef a_SrcTexture, RHITextureRef a_DstTexture,
			uint32_t a_SrcMipLevel, uint32_t a_SrcArraySlice, Box a_SrcRegion,
			uint32_t a_DstMipLevel, uint32_t a_DstArraySlice, Box a_DstRegion,
			ERHIResourceStateTransitionMode a_SrcStateTransitionMode = ERHIResourceStateTransitionMode::Transition,
			ERHIResourceStateTransitionMode a_DstStateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Copy Texture
		// - Copies data from one texture to another.
		// - Note: This can perform a resource state transition if needed.
		CommandBufferType& CopyTexture(
			RHITextureRef a_SrcTexture, RHITextureRef a_DstTexture,
			uint32_t a_SrcMipLevel, uint32_t a_SrcArraySlice,
			uint32_t a_DstMipLevel, uint32_t a_DstArraySlice,
			ERHIResourceStateTransitionMode a_SrcStateTransitionMode = ERHIResourceStateTransitionMode::Transition,
			ERHIResourceStateTransitionMode a_DstStateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

	public:
		Array<RHICommand> Commands;

	protected:
		UnorderedSet<RHIBindingLayoutRef> m_ShaderBindingLayouts;
		UnorderedSet<RHICommandListRef>         m_CommandLists;
		UnorderedSet<RHISamplerRef>             m_Samplers;
		UnorderedSet<RHITextureRef>             m_Textures;
		UnorderedSet<RHIBufferRef>              m_Buffers;

	protected:
		CommandBufferType& Self() { return static_cast<CommandBufferType&>( *this ); }

		template<typename T>
		static void BuildShaderInput(
			T a_Value, RHIShaderInputPayload& o_Payload,
			UnorderedSet<RHISamplerRef>& o_Samplers,
			UnorderedSet<RHITextureRef>& o_TextureRefs,
			UnorderedSet<RHIBufferRef>& o_Buffers );
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
		RHIGraphicsCommandBuffer& SetIndexBuffer( RHIBufferRef a_IBO, ERHIResourceStateTransitionMode a_StateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Set Vertex Buffer
		// - Sets the vertex buffer for the command buffer.
		// - Note: This can perform a resource state transition if needed.
		RHIGraphicsCommandBuffer& SetVertexBuffer( RHIBufferRef a_VBO, ERHIResourceStateTransitionMode a_StateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Set Render Targets
		// - Sets the render targets for the command buffer.
		// - Note: This can perform a resource state transition if needed.
		RHIGraphicsCommandBuffer& SetRenderTargets( Span<RHITexture*> a_RTV, RHITexture* a_DSV, ERHIResourceStateTransitionMode a_StateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Set Render Targets
		// - Sets the render targets for the command buffer.
		// - Note: This can perform a resource state transition if needed.
		RHIGraphicsCommandBuffer& SetRenderTargets( Span<RHITextureRef> a_RTV, RHITexture* a_DSV, ERHIResourceStateTransitionMode a_StateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Clear Render Targets
		// - Clears the render targets for the command buffer.
		// - Note: This can perform a resource state transition if needed.
		RHIGraphicsCommandBuffer& ClearRenderTargets( ERHIClearFlags a_Flags, Span<Color> a_ClearColors, float a_DepthValue = 1.0f, uint8_t a_StencilValue = 0u, ERHIResourceStateTransitionMode a_StateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

		//=====================================================
		// Clear Render Targets
		// - Clears the render targets for the command buffer.
		// - Note: This can perform a resource state transition if needed.
		RHIGraphicsCommandBuffer& ClearRenderTargets( ERHIClearFlags a_Flags, Color a_ClearColor, float a_DepthValue = 1.0f, uint8_t a_StencilValue = 0u, ERHIResourceStateTransitionMode a_StateTransitionMode = ERHIResourceStateTransitionMode::Transition DEBUG_INFO_PARAM );

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
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::SetShaderBindingLayout( 
		RHIBindingLayoutRef a_SBL DEBUG_INFO )
	{
		Commands.EmplaceBack( RHICommand::SetShaderBindingLayout{ a_SBL.get() } );
		m_ShaderBindingLayouts.insert( std::move( a_SBL ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	template<typename _CommandBufferType>
	template<typename T>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::SetShaderInput( 
		HashedString a_Name, T a_Value, ERHIResourceStateTransitionMode a_StateTransitionMode DEBUG_INFO )
	{
		RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetShaderInput() );
		RHICommand::SetShaderInput& data = cmd.Get<RHICommand::SetShaderInput>();
		data.NameHash = a_Name;
		data.Payload = RHIShaderInputPayload();
		data.StateTransitionMode = a_StateTransitionMode;
		BuildShaderInput( a_Value, data.Payload, m_Samplers, m_Textures, m_Buffers );
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
			m_Textures.insert( a_Barrier.Resource->As<RHITexture>()->shared_from_this() );
			break;
		case ERHIResourceType::Buffer:
			m_Buffers.insert( a_Barrier.Resource->As<RHIBuffer>()->shared_from_this() );
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
		RHIBufferRef a_Buffer, Span<const uint8_t> a_Data, size_t a_Offset, ERHIResourceStateTransitionMode a_StateTransitionMode DEBUG_INFO )
	{
		Commands.EmplaceBack( RHICommand::UpdateBuffer( a_Buffer.get(), a_Data, a_Offset, a_StateTransitionMode ) );
		m_Buffers.insert( std::move( a_Buffer ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::CopyBuffer(
		RHIBufferRef a_SrcBuffer, RHIBufferRef a_DstBuffer, size_t a_Size, size_t a_SrcOffset, size_t a_DstOffset, ERHIResourceStateTransitionMode a_StateTransitionMode DEBUG_INFO )
	{
		Commands.EmplaceBack( RHICommand::CopyBuffer( a_SrcBuffer.get(), a_DstBuffer.get(), a_Size, a_SrcOffset, a_StateTransitionMode ) );
		m_Buffers.insert( std::move( a_SrcBuffer ) );
		m_Buffers.insert( std::move( a_DstBuffer ) );
		ADD_DEBUG_INFO();
		return Self();
	}


	template<typename _CommandBufferType>
	inline _CommandBufferType& RHIBaseCommandBuffer<_CommandBufferType>::UpdateTexture(
		RHITextureRef a_Texture, uint32_t a_MipLevel, uint32_t a_ArraySlice, Box a_Region, const RHITextureSubresourceData& a_Data, ERHIResourceStateTransitionMode a_StateTransitionMode DEBUG_INFO )
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
		ERHIResourceStateTransitionMode a_SrcStateTransitionMode, ERHIResourceStateTransitionMode a_DstStateTransitionMode
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
		ERHIResourceStateTransitionMode a_SrcStateTransitionMode, ERHIResourceStateTransitionMode a_DstStateTransitionMode
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

	template<typename _CommandBufferType>
	template<typename T>
	inline void RHIBaseCommandBuffer<_CommandBufferType>::BuildShaderInput( 
		T a_Value,
		RHIShaderInputPayload& o_Payload,
		UnorderedSet<RHISamplerRef>& o_Samplers,
		UnorderedSet<RHITextureRef>& o_TextureRefs, 
		UnorderedSet<RHIBufferRef>& o_Buffers )
	{
		using UnderlyingType = std::remove_const_t<std::remove_reference_t<T>>;

		// Copy Texture Span
		if constexpr ( Concepts::IsSame<UnderlyingType, Span<RHITextureRef>> )
		{
			o_Payload.Count = static_cast<uint8_t>( a_Value.size() );
			o_Payload.IsReference = true;
			auto it = o_Payload.References;

			for ( auto& texRef : a_Value )
			{
				*( it++ ) = static_cast<void*>( texRef.get() );
				o_TextureRefs.insert( texRef );
			}
		}
		// Copy Texture
		else if constexpr ( Concepts::IsConvertible<T, RHITextureRef> )
		{
			o_Payload.Count = 1;
			o_Payload.IsReference = true;
			o_Payload.References[0] = static_cast<void*>( a_Value.get() );
			o_TextureRefs.insert( a_Value );
		}
		// Copy Sampler Span
		else if constexpr ( Concepts::IsSame<UnderlyingType, Span<RHISamplerRef>> )
		{
			o_Payload.Count = static_cast<uint8_t>( a_Value.size() );
			o_Payload.IsReference = true;
			auto it = o_Payload.References;
			for ( auto& samplerRef : a_Value )
			{
				*( it++ ) = static_cast<void*>( samplerRef.get() );
				o_Samplers.insert( samplerRef );
			}
		}
		// Copy Sampler
		else if constexpr ( Concepts::IsConvertible<T, RHISamplerRef> )
		{
			o_Payload.Count = 1;
			o_Payload.IsReference = true;
			o_Payload.References[0] = static_cast<void*>( a_Value.get() );
			o_Samplers.insert( a_Value );
		}
		// Copy Buffer Span
		else if constexpr ( Concepts::IsSame<UnderlyingType, Span<RHIBufferRef>> )
		{
			o_Payload.Count = static_cast<uint8_t>( a_Value.size() );
			o_Payload.IsReference = true;
			auto it = o_Payload.References;

			for ( auto& bufRef : a_Value )
			{
				*( it++ ) = static_cast<void*>( bufRef.get() );
				o_Buffers.insert( bufRef );
			}
		}
		// Copy Buffer
		else if constexpr ( Concepts::IsConvertible<T, RHIBufferRef> )
		{
			o_Payload.Count = 1;
			o_Payload.IsReference = true;
			o_Payload.References[0] = static_cast<void*>( a_Value.get() );
			o_Buffers.insert( a_Value );
		}
		// Copy POD Type
		else
		{
			constexpr size_t InlineDataSize = sizeof( o_Payload.InlineData );

			static_assert( Concepts::IsTriviallyCopyable<T>, "Type must be trivially copyable!" );
			static_assert( sizeof( T ) <= InlineDataSize, "Value is too large to be stored inline!" );

			o_Payload.Count = sizeof( T );
			o_Payload.IsReference = false;
			void* data = &o_Payload.InlineData;
			std::memcpy( data, &a_Value, sizeof( T ) );
		}
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
		RHIBufferRef a_IBO, ERHIResourceStateTransitionMode a_StateTransitionMode DEBUG_INFO )
	{
		RHI_DEV_CHECK( EnumFlags( a_IBO->Descriptor().BindFlags ).HasFlag( ERHIBindFlags::IndexBuffer ), "Buffer is not an index buffer!" );
		RHI_DEV_CHECK( a_IBO->Descriptor().Size > 0, "Buffer size is zero!" );

		Commands.EmplaceBack( RHICommand::SetIndexBuffer{ a_IBO.get(), a_StateTransitionMode } );
		m_Buffers.insert( std::move( a_IBO ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::SetVertexBuffer( 
		RHIBufferRef a_VBO, ERHIResourceStateTransitionMode a_StateTransitionMode DEBUG_INFO )
	{
		RHI_DEV_CHECK( EnumFlags( a_VBO->Descriptor().BindFlags ).HasFlag( ERHIBindFlags::VertexBuffer ), "Buffer is not a vertex buffer!" );
		RHI_DEV_CHECK( a_VBO->Descriptor().Size > 0, "Buffer size is zero!" );

		Commands.EmplaceBack( RHICommand::SetVertexBuffer{ a_VBO.get(), a_StateTransitionMode } );
		m_Buffers.insert( std::move( a_VBO ) );
		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::SetRenderTargets( 
		Span<RHITexture*> a_RTV, RHITexture* a_DSV, ERHIResourceStateTransitionMode a_StateTransitionMode DEBUG_INFO )
	{
		RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetRenderTargets() );
		RHICommand::SetRenderTargets& data = cmd.Get<RHICommand::SetRenderTargets>();
		data.StateTransitionMode = a_StateTransitionMode;

		if ( a_DSV )
		{
			data.DSV = a_DSV;
			m_Textures.insert( a_DSV->shared_from_this() );
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
			m_Textures.insert( a_RTV[i]->shared_from_this() );
		}

		ADD_DEBUG_INFO();
		return Self();
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::SetRenderTargets( 
		Span<RHITextureRef> a_RTV, RHITexture* a_DSV, ERHIResourceStateTransitionMode a_StateTransitionMode DEBUG_INFO )
	{
		RHITexture* rtvs[RHIConstants::MaxColorTargets];
		for ( size_t i = 0; i < a_RTV.size() && i < RHIConstants::MaxColorTargets; ++i )
		{
			rtvs[i] = a_RTV[i].get();
		}
		return SetRenderTargets( Span<RHITexture*>{ rtvs, a_RTV.size() }, a_DSV, a_StateTransitionMode PASS_DEBUG_INFO );
	}

	inline RHIGraphicsCommandBuffer& RHIGraphicsCommandBuffer::ClearRenderTargets( 
		ERHIClearFlags a_Flags, Span<Color> a_ClearColors, float a_DepthValue, uint8_t a_StencilValue, ERHIResourceStateTransitionMode a_StateTransitionMode DEBUG_INFO )
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
		ERHIClearFlags a_Flags, Color a_ClearColor, float a_DepthValue, uint8_t a_StencilValue, ERHIResourceStateTransitionMode a_StateTransitionMode DEBUG_INFO )
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