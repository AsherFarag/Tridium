#pragma once
#include "RHICommon.h"
#include "RHIResource.h"
#include "RHICommandAllocator.h"
#include "RHISampler.h"
#include "RHITexture.h"
#include "RHIMesh.h"
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

		TODO( "These should be moved to the command list" )
		FenceSignal,
		FenceWait,
		Execute,

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
			case ERHICommandType::SetComputePipelineState:  return "SetComputePipelineState";
			case ERHICommandType::DispatchCompute:          return "DispatchCompute";
			case ERHICommandType::DispatchComputeIndirect:  return "DispatchComputeIndirect";
			case ERHICommandType::FenceSignal:              return "FenceSignal";
			case ERHICommandType::FenceWait:                return "FenceWait";
			case ERHICommandType::Execute:                  return "Execute";
			default:                                        return "Unknown";
			}
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

		struct SetShaderBindingLayout
		{
			RHIShaderBindingLayout* SBL;
		};

		struct SetShaderInput
		{
			hash_t NameHash;
			RHIShaderInputPayload Payload;
		};

		struct ResourceBarrier
		{
			RHIResource* Resource;
			ERHIResourceState Before;
			ERHIResourceState After;
		};

		#pragma region Graphics

        struct SetGraphicsPipelineState 
        {
            RHIGraphicsPipelineState* PSO;
        };

        struct SetRenderTargets 
        {
            InlineArray<RHITexture*, RHIConstants::MaxColorTargets> RTV;
            RHITexture* DSV;
        };

        struct ClearRenderTargets 
        {
			InlineArray<RHITexture*, RHIConstants::MaxColorTargets> RTV;
			Color ClearColor;
			uint8_t StencilValue;
			float DepthValue;
			bool DepthBit;
            bool StencilBit;
        };

        struct SetScissors 
        {
			InlineArray<ScissorRect, RHIConstants::MaxColorTargets> Rects;
        };

        struct SetViewports 
        {
			InlineArray<Viewport, RHIConstants::MaxColorTargets> Viewports;
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
			SetShaderBindingLayout,
			SetShaderInput,
			ResourceBarrier,
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
			DispatchComputeIndirect,

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
		struct Debug
		{
			SourceLocation Source;
		} Debug{};
	#endif // RHI_DEBUG_ENABLED
    };



	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// RHI Base Command Buffer
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	//=====================================================
	// RHI Base Command Buffer
	//  A buffer that holds a list of commands to be executed on the GPU.
	//  Adding a command does not execute it immediately.
	//  The command buffer must be submitted to a command list via RHICommandList::SetCommands.
	//  To execute the commands, the command list must be submitted to the RHI via RHI::ExecuteCommandList.
	template <typename _CommandBufferType>
	struct RHIBaseCommandBuffer
	{
		#if RHI_DEBUG_ENABLED
			#define DEBUG_INFO , const SourceLocation& a_SourceLocation = SourceLocation::Current()
			#define ADD_DEBUG_INFO() do { Commands.Back().Debug.Source = a_SourceLocation; } while ( false )
		#else 
			#define DEBUG_INFO
			#define ADD_DEBUG_INFO() do {} while ( false )
		#endif // RHI_DEBUG_ENABLED

		_CommandBufferType& SetShaderBindingLayout( RHIShaderBindingLayoutRef a_SBL DEBUG_INFO )
		{
			Commands.EmplaceBack( RHICommand::SetShaderBindingLayout{ a_SBL.get() } );
			m_ShaderBindingLayouts.insert( std::move( a_SBL ) );
			ADD_DEBUG_INFO();
			return Self();
		}

		template<typename T>
		_CommandBufferType& SetShaderInput( HashedString a_Name, T a_Value DEBUG_INFO )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetShaderInput() );
			RHICommand::SetShaderInput& data = cmd.Get<RHICommand::SetShaderInput>();
			data.NameHash = a_Name;
			data.Payload = RHIShaderInputPayload();
			BuildShaderInput( a_Value, data.Payload, m_Samplers, m_Textures, m_Buffers );
			ADD_DEBUG_INFO();
			return Self();
		}

		_CommandBufferType& ResourceBarrier( RHIResourceRef a_Resource, ERHIResourceState a_Before, ERHIResourceState a_After DEBUG_INFO )
		{
			if ( !a_Resource || !a_Resource->IsValid() ) [[unlikely]]
			{
				ASSERT_LOG( false, "Failed to create a resource barrier as the resource is invalid!" );
				return Self();
			}

			switch ( a_Resource->GetType() )
			{
			case ERHIResourceType::Texture:
				m_Textures.insert( a_Resource->As<RHITexture>()->shared_from_this() );
				break;
			case ERHIResourceType::Buffer:
				m_Buffers.insert( a_Resource->As<RHIBuffer>()->shared_from_this() );
				break;
			default:
				ASSERT_LOG( false, "Can only set resource barriers for textures and buffers!" );
				return Self();
			}

			RHICommand& cmd = Commands.EmplaceBack( RHICommand::ResourceBarrier() );
			RHICommand::ResourceBarrier& data = cmd.Get<RHICommand::ResourceBarrier>();
			data.Resource = a_Resource.get();
			data.Before = a_Before;
			data.After = a_After;
			ADD_DEBUG_INFO();
			return Self();
		}

		_CommandBufferType& FenceSignal( RHIFence a_Fence DEBUG_INFO )
		{
			Commands.EmplaceBack( RHICommand::FenceSignal{ a_Fence } );
			ADD_DEBUG_INFO();
			return Self();
		}
	
		_CommandBufferType& FenceWait( RHIFence a_Fence DEBUG_INFO )
		{
			Commands.EmplaceBack( RHICommand::FenceWait{ a_Fence } );
			ADD_DEBUG_INFO();
			return Self();
		}

	public:
		Array<RHICommand> Commands;

	protected:
		UnorderedSet<RHIShaderBindingLayoutRef> m_ShaderBindingLayouts;
		UnorderedSet<RHICommandListRef>         m_CommandLists;
		UnorderedSet<RHISamplerRef>             m_Samplers;
		UnorderedSet<RHITextureRef>             m_Textures;
		UnorderedSet<RHIBufferRef>              m_Buffers;

	protected:

		_CommandBufferType& Self() { return static_cast<_CommandBufferType&>( *this ); }

		template<typename T>
		static void BuildShaderInput(
			T a_Value, RHIShaderInputPayload& o_Payload,
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
	};



	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Graphics Command Buffer
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	//=====================================================
	// RHI Graphics Command Buffer
	//  A buffer that holds a list of graphics commands to be executed on the GPU.
	struct RHIGraphicsCommandBuffer : RHIBaseCommandBuffer<RHIGraphicsCommandBuffer>
	{
		RHIGraphicsCommandBuffer& SetGraphicsPipelineState( RHIGraphicsPipelineStateRef a_PSO DEBUG_INFO )
		{
			Commands.EmplaceBack( RHICommand::SetGraphicsPipelineState{ a_PSO.get() } );
			m_PipelineStates.insert( std::move( a_PSO ) );
			ADD_DEBUG_INFO();
			return *this;
		}

		RHIGraphicsCommandBuffer& SetIndexBuffer( RHIIndexBufferRef a_IBO DEBUG_INFO )
		{
			Commands.EmplaceBack( RHICommand::SetIndexBuffer{ a_IBO.get() } );
			m_IndexBuffers.insert( std::move( a_IBO ) );
			ADD_DEBUG_INFO();
			return *this;
		}

		RHIGraphicsCommandBuffer& SetVertexBuffer( RHIVertexBufferRef a_VBO DEBUG_INFO )
		{
			Commands.EmplaceBack( RHICommand::SetVertexBuffer{ a_VBO.get() } );
			m_VertexBuffers.insert( std::move( a_VBO ) );
			ADD_DEBUG_INFO();
			return *this;
		}

		RHIGraphicsCommandBuffer& SetRenderTargets( Span<RHITextureRef> a_RTV, RHITextureRef a_DSV DEBUG_INFO )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetRenderTargets() );
			RHICommand::SetRenderTargets& data = cmd.Get<RHICommand::SetRenderTargets>();

			if ( a_DSV )
			{
				data.DSV = a_DSV.get();
				m_Textures.insert( a_DSV );
			}

			for ( size_t i = 0; i < a_RTV.size() && i < data.RTV.MaxSize(); ++i )
			{
				if ( !a_RTV[i] || !a_RTV[i]->IsValid() )
				{
					ASSERT_LOG( false, "Render target is null!" );
					continue;
				}

				if ( !a_RTV[i]->GetDescriptor()->IsRenderTarget )
				{
					ASSERT_LOG( false, "Texture is not a render target!" );
					continue;
				}

				data.RTV.PushBack( a_RTV[i].get() );
				m_Textures.insert( a_RTV[i] );
			}

			ADD_DEBUG_INFO();
			return *this;
		}

		RHIGraphicsCommandBuffer& ClearRenderTargets( Span<RHITextureRef> a_RenderTarget, Color a_ClearColor, bool a_ClearDepth, float a_DepthValue = 1.0f, bool a_ClearStencil = false, uint8_t a_StencilValue = 0 DEBUG_INFO )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::ClearRenderTargets() );
			RHICommand::ClearRenderTargets& data = cmd.Get<RHICommand::ClearRenderTargets>();
			for ( size_t i = 0; i < a_RenderTarget.size() && i < data.RTV.MaxSize(); ++i )
			{
				data.RTV.PushBack( a_RenderTarget[i].get() );
				m_Textures.insert( a_RenderTarget[i] );
			}
			data.ClearColor = a_ClearColor;
			data.DepthValue = a_DepthValue;
			data.StencilValue = a_StencilValue;
			data.DepthBit = a_ClearDepth;
			data.StencilBit = a_ClearStencil;

			ADD_DEBUG_INFO();
			return *this;
		}

		RHIGraphicsCommandBuffer& SetScissors( Span<ScissorRect> a_Rects DEBUG_INFO )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetScissors() );
			RHICommand::SetScissors& data = cmd.Get<RHICommand::SetScissors>();
			data.Rects.Resize( a_Rects.size() );
			std::memcpy( data.Rects.Data(), a_Rects.data(), a_Rects.size() * sizeof( ScissorRect ) );
			ADD_DEBUG_INFO();
			return *this;
		}

		RHIGraphicsCommandBuffer& SetViewports( Span<Viewport> a_Viewports DEBUG_INFO )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetViewports() );
			RHICommand::SetViewports& data = cmd.Get<RHICommand::SetViewports>();
			data.Viewports.Resize( a_Viewports.size() );
			std::memcpy( data.Viewports.Data(), a_Viewports.data(), a_Viewports.size() * sizeof( Viewport ) );
			ADD_DEBUG_INFO();
			return *this;
		}

		RHIGraphicsCommandBuffer& SetPrimitiveTopology( ERHITopology a_Topology DEBUG_INFO )
		{
			Commands.EmplaceBack( RHICommand::SetPrimitiveTopology{ a_Topology } );
			ADD_DEBUG_INFO();
			return *this;
		}

		RHIGraphicsCommandBuffer& Draw( uint32_t a_VertexStart, uint32_t a_VertexCount DEBUG_INFO )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::Draw() );
			RHICommand::Draw& data = cmd.Get<RHICommand::Draw>();
			data.VertexStart = a_VertexStart;
			data.VertexCount = a_VertexCount;
			ADD_DEBUG_INFO();
			return *this;
		}

		RHIGraphicsCommandBuffer& DrawIndexed( uint32_t a_IndexStart, uint32_t a_IndexCount DEBUG_INFO )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::DrawIndexed() );
			RHICommand::DrawIndexed& data = cmd.Get<RHICommand::DrawIndexed>();
			data.IndexStart = a_IndexStart;
			data.IndexCount = a_IndexCount;
			ADD_DEBUG_INFO();
			return *this;
		}

	private:
		UnorderedSet<RHIGraphicsPipelineStateRef> m_PipelineStates;
		UnorderedSet<RHIIndexBufferRef>   m_IndexBuffers;
		UnorderedSet<RHIVertexBufferRef>  m_VertexBuffers;
	};



	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Compute Command Buffer
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	//=====================================================
	// RHI Compute Command Buffer
	//  A buffer that holds a list of compute commands to be executed on the GPU.
	struct RHIComputeCommandBuffer : RHIBaseCommandBuffer<RHIComputeCommandBuffer>
	{
		//RHIComputeCommandBuffer& SetComputePipelineState( RHIGraphicsPipelineStateRef a_PSO DEBUG_INFO )
		//{
		//	Commands.EmplaceBack( RHICommand::SetComputePipelineState{ a_PSO.get() } );
		//	m_PipelineStates.insert( std::move( a_PSO ) );
		//	ADD_DEBUG_INFO();
		//	return *this;
		//}

		RHIComputeCommandBuffer& DispatchCompute( uint16_t a_GroupSizeX, uint16_t a_GroupSizeY, uint16_t a_GroupSizeZ DEBUG_INFO )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::DispatchCompute() );
			RHICommand::DispatchCompute& data = cmd.Get<RHICommand::DispatchCompute>();
			data.GroupSize[0] = a_GroupSizeX;
			data.GroupSize[1] = a_GroupSizeY;
			data.GroupSize[2] = a_GroupSizeZ;
			ADD_DEBUG_INFO();
			return *this;
		}

		RHIComputeCommandBuffer& DispatchComputeIndirect( RHIBufferRef a_Buffer DEBUG_INFO )
		{
			Commands.EmplaceBack( RHICommand::DispatchComputeIndirect{} );
			ADD_DEBUG_INFO();
			return *this;
		}

	private:
		//UnorderedSet<RHIGraphicsPipelineStateRef> m_PipelineStates;
	};

#undef ADD_DEBUG_INFO

}