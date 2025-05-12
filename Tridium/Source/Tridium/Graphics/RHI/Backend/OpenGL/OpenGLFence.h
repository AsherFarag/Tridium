#pragma once
#include "OpenGLCommon.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIFence_OpenGLImpl, RHIFence )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIFence_OpenGLImpl, ERHInterfaceType::OpenGL );
		RHIFence_OpenGLImpl( const DescriptorType& a_Desc );
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		uint64_t GetCompletedValue() override;
		void Signal( uint64_t a_Value ) override;
		void Wait( uint64_t a_Value ) override;

		void AddPendingFence( uint64_t a_Value, GLsync a_Fence )
		{
			m_PendingFences.emplace_back( a_Value, a_Fence );
		}

	private:
		std::deque<Pair<uint64_t, GLsync>> m_PendingFences;
	};

} // namespace Tridium