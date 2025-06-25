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

    #ifndef RHI_DEBUG_ENABLE_CMD_RECORDING
        #define RHI_DEBUG_ENABLE_CMD_RECORDING RHI_DEBUG_ENABLED
    #endif // RHI_DEBUG_ENABLE_CMD_RECORDING

    //=====================================================
    // Helper macros for adding debug information to commands
    #define RHI_DEBUG_SRC_LOC a_DebugSourceLocation
    #define RHI_DEBUG_SRC_LOC_PARAM const SourceLocation& RHI_DEBUG_SRC_LOC = SourceLocation::current()
    #if RHI_DEBUG_ENABLE_CMD_RECORDING
        #define RHI_DEBUG_RES_INFO( a_Resource ) (a_Resource).Desc().Name, (a_Resource).StaticType()
        #define RHI_ADD_DEBUG_CMD_INFO(...) do { AddCmdDebugInfo( RHI_DEBUG_SRC_LOC , __VA_ARGS__ ); } while ( false )
    #else
        #define RHI_DEBUG_RES_INFO( a_Resource )
        #define RHI_ADD_DEBUG_CMD_INFO(...) do {} while ( false )
    #endif // RHI_DEBUG_ENABLE_CMD_RECORDING

    struct RHIGraphicsState
    {
        IRHIGraphicsPipelineState* PipelineState = nullptr;
		RHIFramebuffer Framebuffer{};
        IRHIBuffer* VertexBuffer = nullptr;
        IRHIBuffer* IndexBuffer = nullptr;
        InlineArray<IRHIBindingSet*, RHIConstants::MaxBindingLayouts> BindingSets{};

        auto& SetPipelineState( IRHIGraphicsPipelineState* a_PipelineState ) { PipelineState = a_PipelineState; return *this; }
		auto& SetFramebuffer( const RHIFramebuffer& a_Framebuffer ) { Framebuffer = a_Framebuffer; return *this; }
        auto& SetVertexBuffer( IRHIBuffer* a_VertexBuffer ) { VertexBuffer = a_VertexBuffer; return *this; }
        auto& SetIndexBuffer( IRHIBuffer* a_IndexBuffer ) { IndexBuffer = a_IndexBuffer; return *this; }
        auto& AddBindingSet( IRHIBindingSet* a_BindingSet ) { BindingSets.PushBack( a_BindingSet ); return *this; }
    };

    struct RHIComputeState
    {
	};

    struct RHIDrawArgs
    {
        uint32_t VertexCount = 0;          // Number of vertices to draw
		uint32_t IndexCount = 0;           // Number of indices to draw. 0 means non-indexed draw.
        uint32_t InstanceCount = 1;        // Number of instances to draw
        uint32_t BaseVertex = 0;           // Starting vertex index
        uint32_t BaseIndex = 0;            // Starting index for indexed drawing
        uint32_t BaseInstance = 0;         // Starting instance index

		constexpr bool IsIndexed() const { return IndexCount > 0; }

		constexpr auto& SetVertexCount( uint32_t a_Count ) { VertexCount = a_Count; return *this; }
		constexpr auto& SetIndexCount( uint32_t a_Count ) { IndexCount = a_Count; return *this; }
        constexpr auto& SetInstanceCount( uint32_t a_Count ) { InstanceCount = a_Count; return *this; }
        constexpr auto& SetBaseVertex( uint32_t a_BaseVertex ) { BaseVertex = a_BaseVertex; return *this; }
        constexpr auto& SetBaseIndex( uint32_t a_BaseIndex ) { BaseIndex = a_BaseIndex; return *this; }
        constexpr auto& SetBaseInstance( uint32_t a_BaseInstance ) { BaseInstance = a_BaseInstance; return *this; }
    };

    //======================================================================================================
    // RHI Command List
    //  Command lists are used to submit work to the GPU.
    //======================================================================================================

    struct RHICommandListDesc
    {
        using ResourceType = class IRHICommandList;
        ERHICommandQueueType QueueType = ERHICommandQueueType::Graphics;
        // If true, the command list will execute commands immediately, matching OpenGL behavior. 
		// NOTE: Only one immediate command list can be active at a time.
		bool EnableImmediateExecution = false;
        String Name{};

        constexpr auto& SetQueueType( ERHICommandQueueType a_QueueType ) { QueueType = a_QueueType; return *this; }
		constexpr auto& SetEnableImmediateExecution( bool a_Enable ) { EnableImmediateExecution = a_Enable; return *this; }
                  auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }
    };

    class IRHICommandList : public IRHIObject
    {
        RHI_OBJECT_INTERFACE_BODY( CommandList );

        IRHICommandList( IDynamicRHI* a_Device, const RHICommandListDesc& a_Desc ) 
            : IRHIObject( a_Device ), m_Desc( a_Desc ) {}

        // If true, the command list will automatically validate and transition resource states when necessary.
        // If false, the resources are expected to be in the correct state before the command list is executed.
        void SetAutomaticResourceStateTransitionEnabled( bool a_Enabled ) { m_AutomaticResourceStateTransitionEnabled = a_Enabled; }
        bool IsAutomaticResourceStateTransitionEnabled() const { return m_AutomaticResourceStateTransitionEnabled; }

		// Returns if this command list is an immediate mode.
		// Immediate mode command lists execute commands the moment they are recorded,
		// matching the behavior of OpenGL.
		// NOTE: Only one immediate command list can be active at a time.
		// NOTE: Immediate command lists are not supported in all RHI backends.
		//       Supported backends include OpenGL and DirectX11.
		virtual bool IsImmediate() const = 0;

        // Opens the command list, preparing it for recording commands.
		// Returns false if failed to open the command list.
        virtual bool Open() = 0;
        // Prepares the command list for execution. To execute the command list, call RHI::ExecuteCommandLists(...).
		// Returns false if failed to close the command list.
        virtual bool Close() = 0;
		// Resets the command list to its initial state and clears all owning references to resources.
		virtual void ClearState() = 0;

		// Adds a list of resource barriers to the command list, which are used to synchronize resource states.
		// - OpenGL: No-op, OpenGL does not require explicit resource barriers.
		// - DX12: Maps to ID3D12GraphicsCommandList::ResourceBarrier.
        virtual void ResourceBarriers( Span<const RHIResourceBarrier> a_Barriers, RHI_DEBUG_SRC_LOC_PARAM )
        {
            RHI_ADD_DEBUG_CMD_INFO( "ResourceBarrier" );
		}

		// Adds a single resource barrier to the command list.
		// - See ResourceBarriers( Span<const RHIResourceBarrier>, RHI_DEBUG_SRC_LOC_PARAM ) for details.
        void ResourceBarrier( const RHIResourceBarrier& a_Barrier, RHI_DEBUG_SRC_LOC_PARAM )
        {
            ResourceBarriers( Span{ &a_Barrier, 1 }, RHI_DEBUG_SRC_LOC );
        }

		// Adds a resource barrier to transition the state of 'a_Resource' from its current state to 'a_NewState'.
		// - See ResourceBarriers( Span<const RHIResourceBarrier>, RHI_DEBUG_SRC_LOC_PARAM ) for details.
        void ResourceBarrier( IRHIResource& a_Resource, ERHIResourceStates a_NewState, RHI_DEBUG_SRC_LOC_PARAM )
        {
            ResourceBarrier( { &a_Resource, a_Resource.State(), a_NewState }, RHI_DEBUG_SRC_LOC );
		}

        // Writes 'a_Data' from CPU memory into the GPU buffer 'a_Buffer' at the specified 'a_OffsetBytes' offset.
        virtual void UpdateBuffer( IRHIBuffer& a_Buffer, const void* a_Data, size_t a_DataSizeBytes, size_t a_DstOffsetBytes = 0, RHI_DEBUG_SRC_LOC_PARAM )
        { 
            RHI_ADD_DEBUG_CMD_INFO( "UpdateBuffer", {}, RHI_DEBUG_RES_INFO( a_Buffer ) );
            RHI_DEV_CHECK( a_Data != nullptr && a_DataSizeBytes > 0,
                "Attempting to update a buffer with no data!" );
            RHI_DEV_CHECK( a_DstOffsetBytes + a_DataSizeBytes <= a_Buffer.Desc().Size,
                "Attempting to update a buffer beyond its size! Buffer size: {}, Update size: {}, Offset: {}", a_Buffer.Desc().Size, a_DataSizeBytes, a_DstOffsetBytes );
        }

        // Copies 'a_SizeBytes' bytes from 'a_SrcBuffer' at 'a_SrcOffsetBytes' to 'a_DstBuffer' at 'a_DstOffsetBytes'.
        virtual void CopyBuffer( IRHIBuffer& a_DstBuffer, size_t a_DstOffsetBytes, IRHIBuffer& a_SrcBuffer, RHIBufferRange a_SrcRange, RHI_DEBUG_SRC_LOC_PARAM ) 
        { 
            RHI_ADD_DEBUG_CMD_INFO( "CopyBuffer", {}, RHI_DEBUG_RES_INFO( a_DstBuffer ), RHI_DEBUG_RES_INFO( a_SrcBuffer ) );
            RHI_DEV_CHECK( a_SrcRange.Size > 0 || a_SrcBuffer.Desc().Size == 0,
                "Source buffer is empty or invalid!" );
            RHI_DEV_CHECK( a_DstBuffer.Desc().Size > 0,
                "Destination buffer is invalid!" );
            RHI_DEV_CHECK( a_SrcRange.Offset + a_SrcRange.Size <= a_SrcBuffer.Desc().Size,
                "Source buffer range is out of bounds! Buffer size: {}, Range: [{}, {}]", a_SrcBuffer.Desc().Size, a_SrcRange.Offset, a_SrcRange.Offset + a_SrcRange.Size );
        }

        // Writes 'a_Data' from CPU memory into the GPU texture 'a_Texture' at the specified mip level and array slice.
        virtual void UpdateTexture( IRHITexture& a_Texture, const RHITextureSlice& a_DstSlice, RHITextureSubresourceData a_Data, RHI_DEBUG_SRC_LOC_PARAM )
        {
            RHI_ADD_DEBUG_CMD_INFO( "UpdateTexture", {}, RHI_DEBUG_RES_INFO( a_Texture ) );
        }

		// Copies a region from 'a_SrcTexture' to 'a_DstTexture' using the specified source and destination slices.
        virtual void CopyTexture( IRHITexture& a_DstTexture, const RHITextureSlice& a_DstSlice,
                                  IRHITexture& a_SrcTexture, const RHITextureSlice& a_SrcSlice, RHI_DEBUG_SRC_LOC_PARAM )
        {
            RHI_ADD_DEBUG_CMD_INFO( "CopyTexture", {}, RHI_DEBUG_RES_INFO( a_DstTexture ), RHI_DEBUG_RES_INFO( a_SrcTexture ) );
        }

		// Writes 'a_Data' into the inlined constants block at 'a_DstOffsetBytes' offset.
		// NOTE: The state (RHIGraphicsState, RHIComputeState, etc.) must be set before calling this function.
		// - OpenGL: This maps to using a small GL_UNIFORM_BUFFER.
		// - DX12: This maps to setting the root constants in the root signature via SetGraphicsRoot32BitConstants or SetComputeRoot32BitConstants.
		// - Vulkan: This maps to setting the push constants via vkCmdPushConstants.
        virtual void SetInlinedConstants( const void* a_Data, uint32_t a_SizeBytes, uint32_t a_DstOffsetBytes = 0, RHI_DEBUG_SRC_LOC_PARAM )
        {
            RHI_ADD_DEBUG_CMD_INFO( "SetInlinedConstants" );
            RHI_DEV_CHECK( a_SizeBytes + a_DstOffsetBytes <= RHIConstants::MaxInlinedConstantsSize, 
				"Inlined constants size exceeds the maximum allowed size of {} bytes.", RHIConstants::MaxInlinedConstantsSize );
        }

		// Template overload for SetInlinedConstants that automatically converts a POD type to a byte array.
		// See SetInlinedConstants( const void*, uint32_t, uint32_t, RHI_DEBUG_SRC_LOC_PARAM ) for details.
        template<Concepts::Pod T>
        void SetInlinedConstants( const T& a_Data, uint32_t a_DstOffsetBytes = 0, RHI_DEBUG_SRC_LOC_PARAM )
        {
            SetInlinedConstants( Cast<const void*>( &a_Data ), sizeof(T), a_DstOffsetBytes, RHI_DEBUG_SRC_LOC);
		}

		// Sets the graphics pipeline state for subsequent draw commands.
		// NOTE: This must be called before any draw commands.
        virtual void SetGraphicsState( const RHIGraphicsState& a_GraphicsState, RHI_DEBUG_SRC_LOC_PARAM ) 
        {
            RHI_ADD_DEBUG_CMD_INFO( "SetGraphicsState", {}, RHI_DEBUG_RES_INFO( (*a_GraphicsState.PipelineState) ) );
			RHI_DEV_CHECK( Desc().QueueType == ERHICommandQueueType::Graphics, "SetGraphicsState can only be called on graphics command lists." );
			RHI_DEV_CHECK( a_GraphicsState.PipelineState, "Graphics pipeline state must be valid." );
        }

		// Clears the render targets bound in the current graphics state.
		// a_ColorAttachmentIndex specifies which color attachment to clear, or -1 to clear all color attachments.
		virtual void ClearRenderTargets( ERHIClearFlags a_Flags, Color a_ClearColor, float a_DepthValue = 1.0f, uint8_t a_StencilValue = 0u, int32_t a_ColorAttachmentIndex = -1, RHI_DEBUG_SRC_LOC_PARAM )
        {
            RHI_ADD_DEBUG_CMD_INFO( "ClearRenderTargets" );
			RHI_DEV_CHECK( Desc().QueueType == ERHICommandQueueType::Graphics, "ClearRenderTargets can only be called on graphics command lists." );
        }

		// Binds an array of viewports and scissor rectangles to the rendering pipeline.
		// NOTE: The Graphics State (via SetGraphicsState(...)) must be set before calling this function.
		// - OpenGL: Maps to glViewportIndexed or glViewport & glScissorIndexed.
		// - DX12: Maps to ID3D12GraphicsCommandList::RSSetViewports & ID3D12GraphicsCommandList::RSSetScissorRects.
        virtual void SetViewportState( const RHIViewportState& a_Viewports, RHI_DEBUG_SRC_LOC_PARAM )
        {
            RHI_ADD_DEBUG_CMD_INFO( "SetViewportState" );
		}

		// Draws primitives using the currently set graphics pipeline state.
		// NOTE: The Graphics State (via SetGraphicsState(...)) must be set before calling this function.
        virtual void Draw( const RHIDrawArgs& a_DrawArgs, RHI_DEBUG_SRC_LOC_PARAM )
        {
            RHI_ADD_DEBUG_CMD_INFO( "Draw" );
			RHI_DEV_CHECK( Desc().QueueType == ERHICommandQueueType::Graphics, "Draw can only be called on graphics command lists." );
			RHI_DEV_CHECK( a_DrawArgs.VertexCount > 0, "Vertex count must be greater than zero." );
			RHI_DEV_CHECK( a_DrawArgs.InstanceCount > 0, "Instance count must be greater than zero." );
		}

		// Places a debug marker declaring a group of commands for easier debugging.
		// Use PopDebugGroup() to end the group.
		// NOTE: Debug Groups can be nested.
		// - OpenGL: Maps to glPushDebugGroup.
        // - DX12: Maps to PIXBeginEvent.
        virtual void PushDebugGroup( StringView a_Name ) = 0;

		// Places a debug marker declaring the end of a group of commands.
		// - OpenGL: Maps to glPopDebugGroup.
		// - DX12: Maps to PIXEndEvent.
		virtual void PopDebugGroup() = 0;

        // This inserts a debug message into the command stream.
        // - OpenGL: Maps to glDebugMessageInsert.
        // - DX12: Maps to PIXSetMarker.
		virtual void InsertDebugMarker( StringView a_Name ) = 0;

    #if RHI_DEBUG_ENABLE_CMD_RECORDING
        const auto& DebugCommands() const { return m_DebugCommands; }
    #endif

    protected:
        bool m_AutomaticResourceStateTransitionEnabled = true; // Automatically transition resource states when necessary

    #if RHI_DEBUG_ENABLE_CMD_RECORDING
        struct CmdDebugInfo
        {
            struct ResourceInfo
            {
                StringView Name = "<UNNAMED>";
                ERHIObjectType Type = ERHIObjectType::Unknown;
            };

            StringView CmdName; // Name of the command
            std::source_location CallLocation; // Location where the command was called from
			String Message; // Custom message for the command, if any
            InlineArray<ResourceInfo, 2> Resources; // Resources used by the command
        };
        Array<CmdDebugInfo> m_DebugCommands; // Debug information for commands

		void AddCmdDebugInfo( const SourceLocation& a_SourceLocation, StringView a_CmdName, String a_Message = String{},
            StringView a_ResourceName1 = "<UNNAMED>", ERHIObjectType a_ResourceType1 = ERHIObjectType::Unknown,
            StringView a_ResourceName2 = "<UNNAMED>", ERHIObjectType a_ResourceType2 = ERHIObjectType::Unknown )
        {
            CmdDebugInfo& debugInfo = m_DebugCommands.EmplaceBack( CmdDebugInfo{ a_CmdName, a_SourceLocation, std::move( a_Message ) } );
            debugInfo.Resources.EmplaceBack( CmdDebugInfo::ResourceInfo{ a_ResourceName1, a_ResourceType1 } );
            debugInfo.Resources.EmplaceBack( CmdDebugInfo::ResourceInfo{ a_ResourceName2, a_ResourceType2 } );
        }
    #endif
    };
}