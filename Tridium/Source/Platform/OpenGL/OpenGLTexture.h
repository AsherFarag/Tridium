#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include "Tridium/Rendering/Texture.h"

#include <glad/glad.h>

namespace Tridium {

	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture( const TextureSpecification& specification );
		virtual ~OpenGLTexture();

		virtual bool operator==( const Texture& other ) const override { return m_RendererID == other.GetRendererID(); }

		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetDepth() const override { return m_Depth; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void SetData( void* data, uint32_t size ) override;

		virtual void Bind( uint32_t slot = 0 ) const override;
		virtual void Unbind( uint32_t slot = 0 ) const override;

		virtual bool IsLoaded() const override { return m_IsLoaded; }

	protected:
		virtual void SetIsLoaded( bool loaded ) override { m_IsLoaded = loaded; }

	private:
		TextureSpecification m_Specification;

		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height, m_Depth;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};

}
