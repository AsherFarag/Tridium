#pragma once
#include "../OpenGL4.h"
#include <Tridium/Graphics/RHI/RHICommon.h>
#include <Tridium/Graphics/RHI/RHISampler.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIVertexLayout.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>
#include <Tridium/Graphics/RHI/RHIShader.h>
#include <Tridium/Graphics/RHI/RHIShaderBindingLayout.h>
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHISwapChain.h>
#include <Tridium/Graphics/RHI/RHIDescriptorAllocator.h>
#include <Tridium/Graphics/RHI/RHIFence.h>


namespace Tridium::OpenGL {

	template<typename _GLFactory>
	class GLObjWrapper
	{
	public:
		// Wrapper type that handles the creation and destruction of a type of OpenGL object.
		using GLFactory = _GLFactory;

		explicit GLObjWrapper( bool a_Create = false )
		{
			if ( a_Create )
			{
				Create();
			}
		}
		explicit GLObjWrapper( GLuint a_Handle ) 
			: m_Handle( a_Handle ) 
		{}

		GLObjWrapper( GLObjWrapper&& a_Other ) noexcept : m_Handle( a_Other.m_Handle )
		{
			a_Other.m_Handle = 0;
		}

		GLObjWrapper& operator=( GLObjWrapper&& a_Other ) noexcept
		{
			if ( this != &a_Other )
			{
				Release();
				m_Handle = a_Other.m_Handle;
				a_Other.m_Handle = 0;
			}
			return *this;
		}

		~GLObjWrapper()
		{
			Release();
		}

		GLObjWrapper( const GLObjWrapper& ) = delete;
		GLObjWrapper& operator=( const GLObjWrapper& ) = delete;

		bool Valid() const
		{
			return m_Handle != 0;
		}

		static GLObjWrapper Null()
		{
			return GLObjWrapper( false );
		}

		bool Create()
		{
			Release();
			m_Handle = GLFactory::Create();
			return Valid();
		}

		void Release()
		{
			if ( Valid() )
			{
				GLFactory::Delete( m_Handle );
				m_Handle = 0;
			}
		}

		void SetName( StringView a_Name )
		{
		#if RHI_USE_DEBUG_NAMES
			if ( glObjectLabel && Valid() && !a_Name.empty() )
			{
				OpenGL4::ObjectLabel( GLFactory::Type(), m_Handle, a_Name.size(), static_cast<const GLchar*>( a_Name.data() ) );
			}
		#endif	
		}

		operator GLuint() const
		{
			return m_Handle;
		}

		const void* NativePtr() const
		{
			return &m_Handle;
		}

	private:
		GLuint m_Handle = 0;
	};

	class GLBufferFactory
	{
	public:
		static GLuint Create()
		{
			GLuint handle = 0;
			OpenGL1::GenBuffers( 1, &handle );
			return handle;
		}

		static void Delete( GLuint a_Handle )
		{
			OpenGL1::DeleteBuffers( 1, &a_Handle );
		}

		static constexpr GLenum Type()
		{
			return GL_BUFFER;
		}
	};
	using GLBufferWrapper = GLObjWrapper<GLBufferFactory>;

	class GLTextureFactory
	{
	public:
		static GLuint Create()
		{
			GLuint handle = 0;
			OpenGL1::GenTextures( 1, &handle );
			return handle;
		}

		static void Delete( GLuint a_Handle )
		{
			OpenGL1::DeleteTextures( 1, &a_Handle );
		}

		static constexpr GLenum Type()
		{
			return GL_TEXTURE;
		}
	};
	using GLTextureWrapper = GLObjWrapper<GLTextureFactory>;

}