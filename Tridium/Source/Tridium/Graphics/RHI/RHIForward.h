#pragma once
#include <Tridium/Core/Memory.h>

namespace Tridium {

	class IDynamicRHI;

	class IRHIObject;
	class IRHIResource;
	class IRHITexture;
	class IRHIShaderModule;
	class IRHIBuffer;
	class IRHIBindingLayout;
	class IRHIBindingSet;
	class IRHIGraphicsPipelineState;
	class IRHIComputePipelineState;
	class IRHICommandList;
	class IRHISwapChain;

	struct RHIObjectDesc;
	struct RHIResourceDesc;
	struct RHITextureDesc;
	struct RHIShaderModuleDesc;
	struct RHIBufferDesc;
	struct RHIBindingLayoutDesc;
	struct RHIBindingSetDesc;
	struct RHIGraphicsPipelineStateDesc;
	struct RHIComputePipelineStateDesc;
	struct RHICommandListDesc;
	struct RHISwapChainDesc;

	using RHIObjectRef                    = SharedPtr<IRHIObject>;
	using RHIResourceRef                  = SharedPtr<IRHIResource>;
	using RHITextureRef                   = SharedPtr<IRHITexture>;
	using RHIShaderModuleRef              = SharedPtr<IRHIShaderModule>;
	using RHIBufferRef		              = SharedPtr<IRHIBuffer>;
	using RHIBindingLayoutRef             = SharedPtr<IRHIBindingLayout>;
	using RHIBindingSetRef                = SharedPtr<IRHIBindingSet>;
	using RHIGraphicsPipelineStateRef     = SharedPtr<IRHIGraphicsPipelineState>;
	using RHIComputePipelineStateRef      = SharedPtr<IRHIComputePipelineState>;
	using RHICommandListRef               = SharedPtr<IRHICommandList>;
	using RHISwapChainRef                 = SharedPtr<IRHISwapChain>;

	using RHIObjectWeakRef                = WeakPtr<IRHIObject>;
	using RHIResourceWeakRef              = WeakPtr<IRHIResource>;                 
	using RHITextureWeakRef               = WeakPtr<IRHITexture>;
	using RHIShaderModuleWeakRef          = WeakPtr<IRHIShaderModule>;
	using RHIBufferWeakRef	              = WeakPtr<IRHIBuffer>;
	using RHIBindingLayoutWeakRef         = WeakPtr<IRHIBindingLayout>;
	using RHIBindingSetWeakRef            = WeakPtr<IRHIBindingSet>;
	using RHIGraphicsPipelineStateWeakRef = WeakPtr<IRHIGraphicsPipelineState>;
	using RHIComputePipelineStateWeakRef  = WeakPtr<IRHIComputePipelineState>;
	using RHICommandListWeakRef           = WeakPtr<IRHICommandList>;
	using RHISwapChainWeakRef             = WeakPtr<IRHISwapChain>;

}