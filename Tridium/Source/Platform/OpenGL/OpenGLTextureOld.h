#pragma once

#include "Tridium/Graphics/Rendering/Texture.h"

#include <glad/glad.h>

namespace Tridium {

	class OpenGLTextureOld : public Texture
	{
	public:
		OpenGLTextureOld( const TextureSpecification& a_Specification, void* a_TextureData = nullptr );
		virtual ~OpenGLTextureOld();

		virtual bool operator==( const Texture& other ) const override { return m_RendererID == other.GetRendererID(); }

		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual void SetMinFilter( ETextureFilter a_Filter ) override;
		virtual void SetMagFilter( ETextureFilter a_Filter ) override;
		virtual void SetWrapS( ETextureWrap a_Wrap ) override;
		virtual void SetWrapT( ETextureWrap a_Wrap ) override;
		virtual void SetWrapR( ETextureWrap a_Wrap ) override;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetDepth() const override { return m_Depth; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void* GetData() const override { return m_LocalData; }
		virtual void SetData( void* data, uint32_t size ) override;

		virtual void Bind( uint32_t slot = 0 ) const override;
		virtual void Unbind( uint32_t slot = 0 ) const override;

	private:
		TextureSpecification m_Specification;

		void* m_LocalData;
		uint32_t m_Width, m_Height, m_Depth;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};

	class OpenGLCubeMap : public CubeMap
	{
	public:
		OpenGLCubeMap( const TextureSpecification& a_Specification, SharedPtr<Texture> a_Texture = nullptr );
		OpenGLCubeMap( const TextureSpecification& a_Specification, const std::array<float*, 6>& a_CubeMapData );
		virtual ~OpenGLCubeMap();

		virtual bool operator==( const CubeMap& other ) const override { return m_RendererID == other.GetRendererID(); }

		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual void SetMinFilter( ETextureFilter a_Filter ) override;
		virtual void SetMagFilter( ETextureFilter a_Filter ) override;
		virtual void SetWrapS( ETextureWrap a_Wrap ) override;
		virtual void SetWrapT( ETextureWrap a_Wrap ) override;
		virtual void SetWrapR( ETextureWrap a_Wrap ) override;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetDepth() const override { return m_Depth; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void SetData( SharedPtr<Texture> a_Texture, uint32_t a_Size ) override;
		virtual void SetData( const std::array<float*, 6>& a_CubeMapData, uint32_t a_Size ) override;
		virtual void GenerateMipMaps() override;
		virtual void SetMipLevel( uint32_t a_Level ) override;

		virtual void Bind( uint32_t slot = 0 ) const override;
		virtual void Unbind( uint32_t slot = 0 ) const override;

	private:
		TextureSpecification m_Specification;

		uint32_t m_Width, m_Height, m_Depth;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}
