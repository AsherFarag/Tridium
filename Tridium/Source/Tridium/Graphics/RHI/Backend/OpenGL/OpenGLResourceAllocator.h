#pragma once
#include "OpenGLCommon.h"

namespace Tridium {

	// OpenGL does not have a concept of resource allocators.
	// This is a dummy class to satisfy the RHIResourceAllocator interface.
	class OpenGLResourceAllocator : public RHIResourceAllocator
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );
		bool Commit( const void* a_Params ) override
		{
			ParamsToDescriptor<RHIResourceAllocatorDescriptor>( a_Params );
			return true;
		}

		bool Release() override { return true; }
		bool IsValid() const override { return true; }
		const void* NativePtr() const override { return nullptr; }
	};

} // namespace Tridium