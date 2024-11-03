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
			{ EFramebufferTextureFormat::RGBA16F },		   // Position
			{ EFramebufferTextureFormat::RGB16F },		   // Normal
			{ EFramebufferTextureFormat::RGB8 },		   // Albedo
			{ EFramebufferTextureFormat::RGB8 },		   // Ambient Occlusion + Metalness + Roughness (AOMR)
			{ EFramebufferTextureFormat::RGB8 },		   // Emission
			{ EFramebufferTextureFormat::DEPTH24STENCIL8 } // Depth
		};

		m_Framebuffer = Framebuffer::Create( spec );
	}

} // namespace Tridium