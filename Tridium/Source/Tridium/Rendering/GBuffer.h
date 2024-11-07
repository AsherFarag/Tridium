#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Rendering/FrameBuffer.h>

namespace Tridium {

	// The GBuffer is a wrapper for a FrameBuffer with multiple color attachments used for deferred rendering
	class GBuffer
	{
	public:
		GBuffer( uint32_t a_Width, uint32_t a_Height );
		~GBuffer() = default;

		void Bind() { m_Framebuffer->Bind(); }
		void Unbind() { m_Framebuffer->Unbind(); }
		void Resize( uint32_t a_Width, uint32_t a_Height ) { m_Framebuffer->Resize( a_Width, a_Height ); }

		SharedPtr<Framebuffer> GetFramebuffer() const { return m_Framebuffer; }
		const FramebufferSpecification& GetSpecification() const { return m_Framebuffer->GetSpecification(); }

		uint32_t GetWidth() const { return GetSpecification().Width; }
		uint32_t GetHeight() const { return GetSpecification().Height; }

		uint32_t GetPositionAttachment() const { return m_PositionAttachment; }
		uint32_t GetNormalAttachment() const { return m_NormalAttachment; }
		uint32_t GetAlbedoAttachment() const { return m_AlbedoAttachment; }
		uint32_t GetOMRAttachment() const { return m_OMRAttachment; }
		uint32_t GetEmissionAttachment() const { return m_EmissionAttachment; }
		uint32_t GetDepthAttachment() const { return m_DepthAttachment; }

	protected:
		uint32_t m_PositionAttachment = 0;
		uint32_t m_NormalAttachment = 1;
		uint32_t m_AlbedoAttachment = 2;
		uint32_t m_OMRAttachment = 3; // Ambient Occlusion + Metalness + Roughness (OMR)
		uint32_t m_EmissionAttachment = 4;
		uint32_t m_DepthAttachment = 5;

		SharedPtr<Framebuffer> m_Framebuffer;
	};

}