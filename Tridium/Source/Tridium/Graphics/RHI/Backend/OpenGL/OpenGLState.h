#pragma once
#include "OpenGL4.h"
#include <Tridium/Graphics/RHI/RHICommon.h>
#include <Tridium/Graphics/RHI/RHISampler.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIMesh.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>
#include <Tridium/Graphics/RHI/RHIShader.h>
#include <Tridium/Graphics/RHI/RHIShaderBindingLayout.h>
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHISwapChain.h>
#include <Tridium/Graphics/RHI/RHIResourceAllocator.h>

namespace Tridium {

	namespace OpenGLState {

		extern RHIShaderBindingLayoutWeakRef s_BoundSBL;
		extern GLuint s_BoundIBO;
		extern GLuint s_BoundVBO;
		extern GLuint s_BoundVAO;
		extern GLenum s_BoundPrimitiveTopology;

	}
}