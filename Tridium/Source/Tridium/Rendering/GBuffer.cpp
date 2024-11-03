#include "tripch.h"
#include "GBuffer.h"

namespace Tridium {

	GBuffer::GBuffer( uint32_t a_Width, uint32_t a_Height )
	{
		FramebufferSpecification spec;
		spec.Width = a_Width;
		spec.Height = a_Height;
		spec.Attachments =
		{
			{ EFramebufferTextureFormat::RGBA16F }, // Position
			{ EFramebufferTextureFormat::RGBA16F }, // Normal
			{ EFramebufferTextureFormat::RGBA8 }, // Albedo
			{ EFramebufferTextureFormat::RGBA8 }, // Ambient Occlusion + Metalness + Roughness (AOMR)
			{ EFramebufferTextureFormat::RGBA8 }, // Emission
			{ EFramebufferTextureFormat::DEPTH24STENCIL8 } // Depth
		};

		m_Framebuffer = Framebuffer::Create( spec );
	}

}