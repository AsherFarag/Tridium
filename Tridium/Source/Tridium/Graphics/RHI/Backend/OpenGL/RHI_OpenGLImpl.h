#pragma once
#include <Tridium/Utils/Log.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/RHI/RHICommon.h>
#include <Tridium/Graphics/RHI/DynamicRHI.h>
#include <Tridium/Graphics/RHI/RHIVertexLayout.h>
#include <Tridium/Graphics/RHI/RHISampler.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHIShader.h>
#include <Tridium/Graphics/RHI/RHIShaderBindings.h>
#include <Tridium/Graphics/RHI/RHISwapChain.h>
#include <Tridium/Graphics/RHI/RHIDescriptorAllocator.h>
#include <Tridium/Graphics/RHI/RHIFence.h>
#include <Tridium/Graphics/RHI/RHIStateTracker.h>

#include "OpenGL4.h"
#include "OpenGLToRHI.h"
#include "OpenGLState.h"
#include "OpenGLWrappers.h"


DECLARE_LOG_CATEGORY( OpenGL );

#if RHI_DEBUG_ENABLED
	#define OPENGL_SET_DEBUG_NAME( _ObjectType, _Handle, _Name ) \
		do { \
			if ( ::Tridium::RHIQuery::IsDebug() && !_Name.empty() ) \
			{ \
				OpenGL4::ObjectLabel( GL_TEXTURE, _Handle, _Name.size(), Cast<const GLchar*>( _Name.data() ) ); \
			} \
		} while ( false )
#else
	#define OPENGL_SET_DEBUG_NAME( _ObjectType, _Handle, _Name )
#endif

namespace Tridium::OpenGL {

	//=====================================================================
	// OpenGL Texture Format
	//  Wrapper that holds OpenGL texture format information for a given RHI format
	struct GLTextureFormat
	{
		GLenum InternalFormat = GL_NONE;
		GLenum Format = GL_NONE;
		GLenum Type = GL_NONE;

		bool IsValid() const
		{
			return InternalFormat != GL_NONE
				&& Format != GL_NONE
				&& Type != GL_NONE;
		}

		static constexpr GLTextureFormat From( ERHIFormat a_Format )
		{
			switch ( a_Format )
			{
			case ERHIFormat::Unknown:       return { GL_NONE, GL_NONE, GL_NONE };

			// 8-bit Normalized
			case ERHIFormat::R8_UNORM:      return { GL_R8, GL_RED, GL_UNSIGNED_BYTE };
			case ERHIFormat::RG8_UNORM:     return { GL_RG8, GL_RG, GL_UNSIGNED_BYTE };
			case ERHIFormat::RGBA8_UNORM:   return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };

			// 8-bit Integer
			case ERHIFormat::R8_SINT:       return { GL_R8I, GL_RED_INTEGER, GL_BYTE };
			case ERHIFormat::RG8_SINT:      return { GL_RG8I, GL_RG_INTEGER, GL_BYTE };
			case ERHIFormat::RGBA8_SINT:    return { GL_RGBA8I, GL_RGBA_INTEGER, GL_BYTE };

			case ERHIFormat::R8_UINT:       return { GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE };
			case ERHIFormat::RG8_UINT:      return { GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE };
			case ERHIFormat::RGBA8_UINT:    return { GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE };

			// 16-bit Normalized
			case ERHIFormat::R16_UNORM:     return { GL_R16, GL_RED, GL_UNSIGNED_SHORT };
			case ERHIFormat::RG16_UNORM:    return { GL_RG16, GL_RG, GL_UNSIGNED_SHORT };
			case ERHIFormat::RGBA16_UNORM:  return { GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT };

			// 16-bit Float
			case ERHIFormat::R16_FLOAT:     return { GL_R16F, GL_RED, GL_HALF_FLOAT };
			case ERHIFormat::RG16_FLOAT:    return { GL_RG16F, GL_RG, GL_HALF_FLOAT };
			case ERHIFormat::RGBA16_FLOAT:  return { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT };

			// 16-bit Integer
			case ERHIFormat::R16_SINT:      return { GL_R16I, GL_RED_INTEGER, GL_SHORT };
			case ERHIFormat::RG16_SINT:     return { GL_RG16I, GL_RG_INTEGER, GL_SHORT };
			case ERHIFormat::RGBA16_SINT:   return { GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT };

			case ERHIFormat::R16_UINT:      return { GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT };
			case ERHIFormat::RG16_UINT:     return { GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT };
			case ERHIFormat::RGBA16_UINT:   return { GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT };

			// 32-bit Float
			case ERHIFormat::R32_FLOAT:     return { GL_R32F, GL_RED, GL_FLOAT };
			case ERHIFormat::RG32_FLOAT:    return { GL_RG32F, GL_RG, GL_FLOAT };
			case ERHIFormat::RGB32_FLOAT:   return { GL_RGB32F, GL_RGB, GL_FLOAT };
			case ERHIFormat::RGBA32_FLOAT:  return { GL_RGBA32F, GL_RGBA, GL_FLOAT };

			// 32-bit Integer
			case ERHIFormat::R32_SINT:      return { GL_R32I, GL_RED_INTEGER, GL_INT };
			case ERHIFormat::RG32_SINT:     return { GL_RG32I, GL_RG_INTEGER, GL_INT };
			case ERHIFormat::RGB32_SINT:    return { GL_RGB32I, GL_RGB_INTEGER, GL_INT };
			case ERHIFormat::RGBA32_SINT:   return { GL_RGBA32I, GL_RGBA_INTEGER, GL_INT };

			case ERHIFormat::R32_UINT:      return { GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT };
			case ERHIFormat::RG32_UINT:     return { GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT };
			case ERHIFormat::RGB32_UINT:    return { GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT };
			case ERHIFormat::RGBA32_UINT:   return { GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT };

			// sRGB
			case ERHIFormat::SRGBA8_UNORM:  return { GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE };

			// Depth & Stencil
			case ERHIFormat::D16_UNORM:     return { GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT };
			case ERHIFormat::D32_FLOAT:     return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT };
			case ERHIFormat::D24_UNORM_S8_UINT: return { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 };

			// Compressed Formats
			TODO( "Implement compressed formats" );
			//case ERHIFormat::BC1_UNORM:     return { GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_RGBA, GL_UNSIGNED_BYTE };
			//case ERHIFormat::BC3_UNORM:     return { GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_RGBA, GL_UNSIGNED_BYTE };
			case ERHIFormat::BC4_UNORM:       return { GL_COMPRESSED_RED_RGTC1, GL_RED, GL_UNSIGNED_BYTE };
			case ERHIFormat::BC5_UNORM:       return { GL_COMPRESSED_RG_RGTC2, GL_RG, GL_UNSIGNED_BYTE };
			//case ERHIFormat::BC7_UNORM:     return { GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, GL_RGBA, GL_UNSIGNED_BYTE };

			default:                          return { GL_NONE, GL_NONE, GL_NONE };
			}
		}
	};

	//=====================================================================
	// OpenGL Framebuffer
	//  Wrapper that holds OpenGL framebuffer information for a given swapchain
	struct Framebuffer
	{
		RHITextureRef BackBufferTexture = nullptr;
		GLuint ShaderID = 0;
		struct Quad
		{

			static constexpr float Vertices[] =
			{
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 0.0f, 0.0f, 1.0f
			};

			static constexpr uint32_t Indices[] = { 0, 1, 2, 2, 3, 0 };

			GLuint VAO = 0;
			GLuint VBO = 0;
			GLuint IBO = 0;

			~Quad()
			{
				if ( VAO )
				{
					OpenGL3::DeleteVertexArrays( 1, &VAO );
				}
				if ( VBO )
				{
					OpenGL3::DeleteBuffers( 1, &VBO );
				}
				if ( IBO )
				{
					OpenGL3::DeleteBuffers( 1, &IBO );
				}
			}

		} ScreenQuad;

		bool Init( const RHISwapChainDescriptor& a_Desc );
		void Resize( uint32_t a_Width, uint32_t a_Height );

		~Framebuffer()
		{
			if ( ShaderID )
			{
				OpenGL3::DeleteProgram( ShaderID );
			}
		}
	};

#pragma region OPENGL RHI IMPLEMENTATIONS

	//======================================================================
	// TEXTURE IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHITexture_OpenGLImpl, RHITexture )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHITexture_OpenGLImpl, ERHInterfaceType::OpenGL );
		RHITexture_OpenGLImpl( const DescriptorType & a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData );
		virtual bool Release() override;
		virtual size_t GetSizeInBytes() const override;
		virtual const void* NativePtr() const { return TextureObj.NativePtr(); }
		virtual bool IsValid() const override { return TextureObj.Valid(); }

		OpenGL::GLTextureWrapper TextureObj{};
		GLTextureFormat GLFormat{};
	};

	//======================================================================
	// BUFFER IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIBuffer_OpenGLImpl, RHIBuffer )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIBuffer_OpenGLImpl, ERHInterfaceType::OpenGL );

		RHIBuffer_OpenGLImpl( const RHIBufferDescriptor & a_Desc, Span<const uint8_t> a_Data = {} );

		virtual ~RHIBuffer_OpenGLImpl() override = default;
		virtual bool Release() override { BufferObj.Release(); return true; }
		virtual bool IsValid() const override { return BufferObj.Valid(); }
		virtual size_t GetSizeInBytes() const override;
		virtual const void* NativePtr() const override { return BufferObj.NativePtr(); }

		OpenGL::GLBufferWrapper BufferObj{};
	};

	//======================================================================
	// FENCE IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIFence_OpenGLImpl, RHIFence )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIFence_OpenGLImpl, ERHInterfaceType::OpenGL );
		RHIFence_OpenGLImpl( const DescriptorType & a_Desc );
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

	//======================================================================
	// SAMPLER IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHISampler_OpenGLImpl, RHISampler )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHISampler_OpenGLImpl, ERHInterfaceType::OpenGL );

		RHISampler_OpenGLImpl( const DescriptorType & a_Desc )
			: RHISampler( a_Desc )
		{
			OpenGL3::GenSamplers( 1, &m_SamplerID );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_MIN_FILTER, Translate( a_Desc.Filter ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_MAG_FILTER, Translate( a_Desc.Filter ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_S, Translate( a_Desc.AddressU ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_T, Translate( a_Desc.AddressV ) );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_R, Translate( a_Desc.AddressW ) );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_LOD_BIAS, a_Desc.MipLODBias );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_MAX_ANISOTROPY, a_Desc.MaxAnisotropy );
			OpenGL3::SamplerParameteri( m_SamplerID, GL_TEXTURE_COMPARE_FUNC, Translate( a_Desc.ComparisonFunc ) );
			OpenGL3::SamplerParameterfv( m_SamplerID, GL_TEXTURE_BORDER_COLOR, &a_Desc.BorderColor.r );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_MIN_LOD, a_Desc.MinLOD );
			OpenGL3::SamplerParameterf( m_SamplerID, GL_TEXTURE_MAX_LOD, a_Desc.MaxLOD );
		}

		bool Release() override
		{
			glDeleteSamplers( 1, &m_SamplerID );
			return true;
		}

		bool IsValid() const override { return m_SamplerID != 0; }
		const void* NativePtr() const override { return &m_SamplerID; }

		GLuint GetGLHandle() const { return m_SamplerID; }

	private:
		GLuint m_SamplerID = 0;
	};

	//======================================================================
	// BINDING LAYOUT IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIBindingLayout_OpenGLImpl, RHIBindingLayout )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIBindingLayout_OpenGLImpl, ERHInterfaceType::OpenGL );
		RHIBindingLayout_OpenGLImpl( const DescriptorType & a_Desc );
		bool Release() override { return true; }
		bool IsValid() const override { return true; }
		const void* NativePtr() const override { return nullptr; }
	};

	//======================================================================
	// BINDING SET IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIBindingSet_OpenGLImpl, RHIBindingSet )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIBindingSet_OpenGLImpl, ERHInterfaceType::OpenGL );
		RHIBindingSet_OpenGLImpl( const DescriptorType & a_Desc );
		bool Release() override { return true; }
		bool IsValid() const override { return true; }
		const void* NativePtr() const override { return nullptr; }
	};

	//======================================================================
	// SHADER IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIShaderModule_OpenGLImpl, RHIShaderModule )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIShaderModule_OpenGLImpl, ERHInterfaceType::OpenGL );

		RHIShaderModule_OpenGLImpl( const DescriptorType & a_Desc );
		bool Release() override;
		bool IsValid() const override { return m_ShaderID != 0; }
		const void* NativePtr() const override { return &m_ShaderID; }

		GLuint GetGLHandle() const { return m_ShaderID; }

	private:
		GLuint m_ShaderID = 0;
	};

	//======================================================================
	// GRAPHICS PIPELINE STATE IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIGraphicsPipelineState_OpenGLImpl, RHIGraphicsPipelineState )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIGraphicsPipelineState_OpenGLImpl, ERHInterfaceType::OpenGL );
		RHIGraphicsPipelineState_OpenGLImpl( const DescriptorType & a_Desc );
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override { return nullptr; }

		GLuint GetShaderProgramID() const { return m_ShaderProgramID; }
		GLuint GetVAO() const { return m_VAO; }
		bool ApplyVertexLayoutToVAO( GLuint a_VAO );
		GLint TryGetUniformLocation( hash_t a_NameHash ) const;

	private:
		GLuint m_ShaderProgramID = 0;
		GLuint m_VAO = 0;
		UnorderedMap<hash_t, GLint> m_UnifromLocations;
		UnorderedMap<StringView, Array<StringView>> m_CombinedSamplers; // Map of texture name to an array of sampler name
	};

	//======================================================================
	// SWAPCHAIN IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHISwapChain_OpenGLImpl, RHISwapChain )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHISwapChain_OpenGLImpl, ERHInterfaceType::OpenGL );
		RHISwapChain_OpenGLImpl( const DescriptorType & a_Desc );
		bool Release() override;
		bool IsValid() const override { return Window != nullptr; }
		const void* NativePtr() const override { return Window; }
		bool Present() override;
		RHITextureRef GetBackBuffer() override;
		bool Resize( uint32_t a_Width, uint32_t a_Height ) override;
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		const auto& GetFramebuffer() const { return m_Framebuffer; }

		GLFWwindow* Window = nullptr;

	private:
		Framebuffer m_Framebuffer;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};

	//======================================================================
	// COMMAND LIST IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHICommandList_OpenGLImpl, RHICommandList )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHICommandList_OpenGLImpl, ERHInterfaceType::OpenGL );

		RHICommandList_OpenGLImpl( const DescriptorType & a_Desc );
		virtual bool Release() override { return true; }
		virtual bool IsValid() const override { return true; }
		virtual const void* NativePtr() const override { return nullptr; }

		virtual bool SetGraphicsCommands( const RHIGraphicsCommandBuffer & a_CmdBuffer ) override;
		virtual bool SetComputeCommands( const RHIComputeCommandBuffer & a_CmdBuffer ) override;
		virtual bool IsCompleted() const override { return true; }
		virtual void WaitUntilCompleted() override {}

	private:
		OpenGL::GLUBOWrapper m_InlinedConstantsUBO;
		Array<GLuint> m_UBOs;
		struct State
		{
			uint32_t NumColorTargets = 0;
		} m_State;

		void SetBindingLayout( const RHICommand::SetBindingLayout & a_Data );
		void SetShaderBindings( const RHICommand::SetShaderBindings & a_Data );
		void SetInlinedConstants( const RHICommand::SetInlinedConstants & a_Data );
		void ResourceBarrier( const RHICommand::ResourceBarrier & a_Data );

		// Graphics
		void SetGraphicsPipelineState( const RHICommand::SetGraphicsPipelineState & a_Data );
		void SetRenderTargets( const RHICommand::SetRenderTargets & a_Data );
		void ClearRenderTargets( const RHICommand::ClearRenderTargets & a_Data );
		void SetScissors( const RHICommand::SetScissors & a_Data );
		void SetViewports( const RHICommand::SetViewports & a_Data );
		void SetIndexBuffer( const RHICommand::SetIndexBuffer & a_Data );
		void SetVertexBuffer( const RHICommand::SetVertexBuffer & a_Data );
		void SetPrimitiveTopology( const RHICommand::SetPrimitiveTopology & a_Data );
		void Draw( const RHICommand::Draw & a_Data );
		void DrawIndexed( const RHICommand::DrawIndexed & a_Data );

		// Compute
		void SetComputePipelineState( const RHICommand::SetComputePipelineState & a_Data );
		void DispatchCompute( const RHICommand::DispatchCompute & a_Data );
		void DispatchComputeIndirect( const RHICommand::DispatchComputeIndirect & a_Data );
	};

	//======================================================================
	// DYNAMIC RHI IMPLEMENTATION
	//======================================================================

	class DynamicRHI_OpenGLImpl final : public IDynamicRHI
	{
	public:
		//==============================================
		// Core RHI functions
		// Initialise the RHI with the given configuration.
		virtual bool Init( const RHIConfig& a_Config ) override;
		// Shutdown the RHI.
		virtual bool Shutdown() override;
		// Execute the given command list.
		virtual bool ExecuteCommandList( RHICommandListRef a_CommandList ) override;
		// Returns the type of the Dynamically bound RHI.
		virtual ERHInterfaceType GetRHIType() const override { return ERHInterfaceType::OpenGL; }
		// Returns the static RHI type.
		static constexpr ERHInterfaceType GetStaticRHIType() { return ERHInterfaceType::OpenGL; }
		//==============================================

		//=====================================================
		// Resource creation
		virtual RHIFenceRef CreateFence( const RHIFenceDescriptor& a_Desc ) override;
		virtual RHISamplerRef CreateSampler( const RHISamplerDescriptor& a_Desc ) override;
		virtual RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData ) override;
		virtual RHIBufferRef CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data ) override;
		virtual RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc ) override;
		virtual RHICommandListRef CreateCommandList( const RHICommandListDescriptor& a_Desc ) override;
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc ) override;
		virtual RHIBindingLayoutRef CreateBindingLayout( const RHIBindingLayoutDescriptor& a_Desc ) override;
		virtual RHIBindingSetRef CreateBindingSet( const RHIBindingSetDescriptor& a_Desc ) override;
		virtual RHISwapChainRef CreateSwapChain( const RHISwapChainDescriptor& a_Desc ) override;
		//=====================================================

	#if RHI_DEBUG_ENABLED

		// Dump debug information about the RHI into the console.
		virtual void DumpDebug() override;

	#endif // RHI_DEBUG_ENABLED
	};

	inline DynamicRHI_OpenGLImpl* GetOpenGLRHI()
	{
		RHI_DEV_CHECK( s_DynamicRHI != nullptr && s_DynamicRHI->GetRHIType() == ERHInterfaceType::OpenGL, "Invalid RHI type!" );
		return Cast<DynamicRHI_OpenGLImpl*>( s_DynamicRHI );
	}

#pragma endregion

} // namespace Tridium::OpenGL