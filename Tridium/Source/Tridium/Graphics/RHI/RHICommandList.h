#pragma once
#include "RHICommon.h"
#include "RHIResource.h"
#include "RHICommandAllocator.h"
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
			void* References[RHIQuery::MaxTextureBindings];
			uint8_t InlineData[sizeof( References )];
		};
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
            RHIPipelineState* PSO;
        };

        struct SetRenderTargets 
        {
            FixedArray<RHITexture*, RHIQuery::MaxColourTargets> RTV;
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
	    (_CommandBuffer).Commands.Back().Debug = { _FUNCTION_, __FILE__, _USABLE_LINE_ }
#else
	// Helper function that enqueues a command.
    #define RHICommandEnqueue( _CommandBuffer, _Command, ... ) \
        (_CommandBuffer)._Command( __VA_ARGS__ )
#endif // !RHI_DEBUG_ENABLED



	//=====================================================
	// RHICommandBuffer
	//  A buffer that holds an array of commands to be executed on the GPU.
	//=====================================================
	class RHICommandBuffer
	{
	public:
		void SetPipelineState( RHIPipelineStateRef a_PSO )
		{
			Commands.EmplaceBack( RHICommand::SetPipelineState{ a_PSO.get() } );
			m_PipelineStates.insert( std::move( a_PSO ) );
		}

		void SetRenderTargets( const Span<RHITextureRef>& a_RTV, RHITextureRef a_DSV )
		{
			RHICommand& cmd = Commands.EmplaceBack( RHICommand::SetRenderTargets() );
			RHICommand::SetRenderTargets& data = cmd.Get<RHICommand::SetRenderTargets>();

			data.DSV = a_DSV.get();
			m_Textures.insert( a_DSV );

			for ( size_t i = 0; i < a_RTV.size() && i < data.RTV.MaxSize(); ++i )
			{
				data.RTV[i] = a_RTV[i].get();
				m_Textures.insert( a_RTV[i] );
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
			data.Payload = RHIShaderInputPayload();
			BuildShaderInput( a_Value, data.Payload, m_Textures, m_Buffers );
		}

		void SetIndexBuffer( RHIIndexBufferRef a_IBO )
		{
			Commands.EmplaceBack( RHICommand::SetIndexBuffer{ a_IBO.get() } );
			m_IndexBuffers.insert( std::move( a_IBO ) );
		}

		void SetVertexBuffer( RHIVertexBufferRef a_VBO )
		{
			Commands.EmplaceBack( RHICommand::SetVertexBuffer{ a_VBO.get() } );
			m_VertexBuffers.insert( std::move( a_VBO ) );
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

	public:
		Array<RHICommand> Commands;

	private:
		UnorderedSet<RHIPipelineStateRef> m_PipelineStates;
		UnorderedSet<RHIIndexBufferRef>   m_IndexBuffers;
		UnorderedSet<RHIVertexBufferRef>  m_VertexBuffers;
		UnorderedSet<RHICommandListRef>   m_CommandLists;
		UnorderedSet<RHITextureRef>       m_Textures;
		UnorderedSet<RHIBufferRef>        m_Buffers;

	private:
			template<typename T>
			static void BuildShaderInput(
				T a_Value, RHIShaderInputPayload& o_Payload,
				UnorderedSet<RHITextureRef>& o_TextureRefs,
				UnorderedSet<RHIBufferRef>& o_Buffers );
	};

	RHI_RESOURCE_BASE_TYPE( CommandList,
		virtual bool SetCommands( const RHICommandBuffer& a_CmdBuffer ) { return false; }
	)
	{
		RHICommandBuffer Commands;
		RHICommandAllocatorRef Allocator;
	};

	template<typename T>
	inline void RHICommandBuffer::BuildShaderInput( T a_Value, RHIShaderInputPayload& o_Payload, UnorderedSet<RHITextureRef>& o_TextureRefs, UnorderedSet<RHIBufferRef>& o_Buffers )
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

}