#pragma once
#include "RHICommandBuffer.h"

namespace Tridium {

	//======================================================================================================
	// RHI Command List
	//  Command lists are used to submit work to the GPU.
	//======================================================================================================

	struct RHICommandListDescriptor
	{
		using ResourceType = class RHICommandList;
		ERHICommandQueueType QueueType = ERHICommandQueueType::Graphics;
		String Name{};

		constexpr auto& SetQueueType( ERHICommandQueueType a_QueueType ) { QueueType = a_QueueType; return *this; }
		constexpr auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHICommandList )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHICommandList, ERHIResourceType::CommandList );

		RHICommandList( const RHICommandListDescriptor& a_Desc ) : m_Desc( a_Desc ) {}
		virtual bool SetGraphicsCommands( const RHIGraphicsCommandBuffer& a_CmdBuffer ) = 0;
		virtual bool SetComputeCommands( const RHIComputeCommandBuffer& a_CmdBuffer ) = 0;
		virtual bool IsCompleted() const = 0;
		virtual void WaitUntilCompleted() = 0;

		// Has the command list been submitted for execution?
		bool IsPendingExecution() const { return m_PendingExecution; }
		// Get the pending fence value for the command list.
		uint64_t FenceValue() const { return m_FenceValue; }

		// Set if the command list is pending execution.
		// WARNING: Avoid using this unless you know what you're doing, as this is meant for internal use only.
		void SetPendingExecution( bool a_PendingExecution ) { m_PendingExecution = a_PendingExecution; }
		// Set the fence value for the command list.
		// WARNING: Avoid using this unless you know what you're doing, as this is meant for internal use only.
		void SetFenceValue( uint64_t a_FenceValue ) { m_FenceValue = a_FenceValue; }

	private:
		uint64_t m_FenceValue = 0;
		bool m_PendingExecution = false;
	};

}