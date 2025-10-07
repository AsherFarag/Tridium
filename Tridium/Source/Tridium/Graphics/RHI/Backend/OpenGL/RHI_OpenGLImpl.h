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
#include <Tridium/Graphics/RHI/RHIStateTracker.h>

#include "OpenGL4.h"
#include "OpenGLToRHI.h"
#include "OpenGLWrappers.h"


DECLARE_LOG_CATEGORY( OpenGL );

#if RHI_DEBUG_ENABLED
	#define OPENGL_SET_DEBUG_NAME( _ObjectType, _Handle, _Name ) \
		do { \
			if ( ::Tridium::RHI::IsDebug() && !_Name.empty() ) \
			{ \
				OpenGL4::ObjectLabel( GL_TEXTURE, _Handle, _Name.size(), Cast<const GLchar*>( _Name.data() ) ); \
			} \
		} while ( false )
#else
	#define OPENGL_SET_DEBUG_NAME( _ObjectType, _Handle, _Name )
#endif

namespace Tridium::OpenGL {

	struct ScopedTextureBinding
	{
		GLenum Target;
		GLint PreviousHandle;

		ScopedTextureBinding( GLenum a_Target, GLuint a_Handle )
			: Target( a_Target )
		{
			GLenum bindingTarget = 0;
			switch ( Target )
			{
			case GL_TEXTURE_1D: bindingTarget = GL_TEXTURE_BINDING_1D; break;
			case GL_TEXTURE_2D: bindingTarget = GL_TEXTURE_BINDING_2D; break;
			case GL_TEXTURE_3D: bindingTarget = GL_TEXTURE_BINDING_3D; break;
			case GL_TEXTURE_2D_MULTISAMPLE: bindingTarget = GL_TEXTURE_BINDING_2D_MULTISAMPLE; break;
			case GL_TEXTURE_CUBE_MAP: bindingTarget = GL_TEXTURE_BINDING_CUBE_MAP; break;
			case GL_TEXTURE_1D_ARRAY: bindingTarget = GL_TEXTURE_BINDING_1D_ARRAY; break;
			case GL_TEXTURE_2D_ARRAY: bindingTarget = GL_TEXTURE_BINDING_2D_ARRAY; break;
			case GL_TEXTURE_CUBE_MAP_ARRAY: bindingTarget = GL_TEXTURE_BINDING_CUBE_MAP_ARRAY; break;
			case GL_TEXTURE_BUFFER: bindingTarget = GL_TEXTURE_BINDING_BUFFER; break;
			case GL_TEXTURE_RECTANGLE: bindingTarget = GL_TEXTURE_BINDING_RECTANGLE; break;
			default: ASSERT( false, "Unsupported texture target for binding: {0}", Target ); break;
			}

			OpenGL1::GetIntegerv( bindingTarget, &PreviousHandle );
			OpenGL1::BindTexture( Target, a_Handle );
		}

		~ScopedTextureBinding()
		{
			OpenGL1::BindTexture( Target, PreviousHandle );
		}
	};;

	//=====================================================================
	// OpenGL Texture Format
	//  Wrapper that holds OpenGL texture format information for a given RHI format
	struct GLTextureFormat
	{
		GLenum InternalFormat = GL_NONE;
		GLenum Format = GL_NONE;
		GLenum Type = GL_NONE;

		bool Valid() const
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

		bool Init( const RHISwapChainDesc& a_Desc );
		void Resize( uint32_t a_Width, uint32_t a_Height );

		~Framebuffer()
		{
			if ( ShaderID )
			{
				OpenGL3::DeleteProgram( ShaderID );
			}
		}
	};


	struct GLSamplerDesc
	{
		GLenum MinFilter = GL_LINEAR;
		GLenum MagFilter = GL_LINEAR;
		GLenum AddressU = GL_REPEAT;
		GLenum AddressV = GL_REPEAT;
		GLenum AddressW = GL_REPEAT;
		GLenum ComparisonFunc = GL_NONE;
		bool IsAnisotropic = false;
		float MaxAnisotropy = 1.0f;
		float MipLodBias = 0.0f;
		float MinLod = -1000.0f;
		float MaxLod = 1000.0f;
		GLfloat BorderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		static GLSamplerDesc From( const RHISampler& a_Sampler, class DynamicRHI_OpenGLImpl* a_RHI );
		void ApplyToTexture( GLenum a_Target, GLuint a_Obj );
		void ApplyToSampler( GLuint a_Obj );

	};

#pragma region OPENGL RHI IMPLEMENTATIONS

	//=================================================================================================
	// RHITexture_OpenGLImpl
	//=================================================================================================
	class RHITexture_OpenGLImpl : public IRHITexture
	{
	public:

		//=============================================================================================
		RHI_OBJECT_IMPLEMENTATION_BODY( RHITexture_OpenGLImpl, OpenGL, ERHInterfaceType::OpenGL );
		RHITexture_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType & a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData );
		~RHITexture_OpenGLImpl() override { Release(); }

		//=============================================================================================
		virtual bool Release() override;
		virtual const void* NativePtr() const { return &m_GLHandle; }
		virtual bool Valid() const override { return m_GLHandle != 0; }
		virtual RHITextureSubresourceData MapSubresource( const RHITextureSlice& a_Slice ) override;
		virtual void UnmapSubresource( const RHITextureSlice& a_Slice ) override;

		//=============================================================================================
		GLuint GLHandle() const { return m_GLHandle; }
		GLenum GLTarget() const { return m_GLTarget; }
		const GLTextureFormat& GLFormat() const { return m_GLFormat; }
		size_t GetTotalSizeInBytes() const;
		RHIBufferRange GetSubresourceRange( const RHITextureSlice& a_Slice ) const;

	protected:

		//=============================================================================================
		GLuint m_GLHandle;
		GLTextureFormat m_GLFormat{};
		GLenum m_GLTarget = GL_NONE;

		bool CommitAsStagingTexture();

	};

	//=================================================================================================
	// RHIBuffer_OpenGLImpl
	//=================================================================================================
	class RHIBuffer_OpenGLImpl : public IRHIBuffer
	{
	public:

		//=============================================================================================
		RHI_OBJECT_IMPLEMENTATION_BODY( RHIBuffer_OpenGLImpl, OpenGL, ERHInterfaceType::OpenGL );
		RHIBuffer_OpenGLImpl( IDynamicRHI* a_Device, const RHIBufferDesc & a_Desc, Span<const uint8_t> a_Data = {} );
		~RHIBuffer_OpenGLImpl() override { Release(); }

		//=============================================================================================
		virtual bool Release() override { BufferObj.Release(); return true; }
		virtual bool Valid() const override { return BufferObj.Valid(); }
		virtual const void* NativePtr() const override { return BufferObj.NativePtr(); }
		virtual const void* Map() override;
		virtual void Unmap() override;

		//=============================================================================================
		OpenGL::GLBufferWrapper BufferObj{};
	};

	//======================================================================
	// BINDING LAYOUT IMPLEMENTATION
	//======================================================================

	class RHIBindingLayout_OpenGLImpl : public IRHIBindingLayout
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHIBindingLayout_OpenGLImpl, OpenGL, ERHInterfaceType::OpenGL );
		RHIBindingLayout_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType & a_Desc );
		~RHIBindingLayout_OpenGLImpl() override { Release(); }

		bool Release() override { return true; }
		bool Valid() const override { return true; }
		const void* NativePtr() const override { return nullptr; }


	};

	//======================================================================
	// BINDING SET IMPLEMENTATION
	//======================================================================

	class RHIBindingSet_OpenGLImpl : public IRHIBindingSet
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHIBindingSet_OpenGLImpl, OpenGL, ERHInterfaceType::OpenGL );
		RHIBindingSet_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType & a_Desc );
		~RHIBindingSet_OpenGLImpl() override { Release(); }

		bool Release() override { return true; }
		bool Valid() const override { return true; }
		const void* NativePtr() const override { return nullptr; }
	};

	//======================================================================
	// SHADER IMPLEMENTATION
	//======================================================================

	class RHIShaderModule_OpenGLImpl : public IRHIShaderModule
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHIShaderModule_OpenGLImpl, OpenGL, ERHInterfaceType::OpenGL );
		RHIShaderModule_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType & a_Desc );
		~RHIShaderModule_OpenGLImpl() override { Release(); }

		bool Release() override;
		bool Valid() const override { return m_ShaderID != 0; }
		const void* NativePtr() const override { return &m_ShaderID; }

		GLuint GetGLHandle() const { return m_ShaderID; }

	private:
		GLuint m_ShaderID = 0;
	};

	//======================================================================
	// GRAPHICS PIPELINE STATE IMPLEMENTATION
	//======================================================================

	using UniformLocation = GLint;
	constexpr UniformLocation c_InvalidUniformLocation = -1;

	struct Uniform
	{
		UniformLocation BindingPoint = c_InvalidUniformLocation;
		UniformLocation BlockIndex = c_InvalidUniformLocation; // Valid only if IsBlock is true
		GLenum Type = GL_NONE; // OpenGL type (e.g., GL_FLOAT, GL_INT, etc.)
		bool IsBlock = false; // Indicates if this uniform is a block (UBO) or a regular uniform
	};

	struct UniformLayout
	{
		FixedArray<UnorderedMap<uint32_t, Uniform>, RHIConstants::MaxBindingLayouts> Layouts;
		Uniform InlinedConstants; // Special uniform for inlined constants UBO

		void SetLayout( uint32_t a_LayoutIndex, const IRHIBindingLayout& a_Layout, GLuint a_ShaderProgramID );
	};

	class RHIGraphicsPipelineState_OpenGLImpl : public IRHIGraphicsPipelineState
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHIGraphicsPipelineState_OpenGLImpl, OpenGL, ERHInterfaceType::OpenGL );
		RHIGraphicsPipelineState_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType & a_Desc );
		~RHIGraphicsPipelineState_OpenGLImpl() override { Release(); }

		bool Release() override;
		bool Valid() const override;
		const void* NativePtr() const override { return nullptr; }

		GLuint GetShaderProgramID() const { return m_ShaderProgramID; }
		GLuint GetVAO() const { return m_VAO; }
		bool ApplyVertexLayoutToVAO( GLuint a_VAO ) const;
		GLint TryGetUniformLocation( hash_t a_NameHash ) const;

		UniformLayout UniformLayout{};

	private:
		GLuint m_ShaderProgramID = 0;
		GLuint m_VAO = 0;
		UnorderedMap<hash_t, GLint> m_UnifromLocations;
		UnorderedMap<StringView, Array<StringView>> m_CombinedSamplers; // Map of texture name to an array of sampler name
	};

	//======================================================================
	// SWAPCHAIN IMPLEMENTATION
	//======================================================================

	class RHISwapChain_OpenGLImpl : public IRHISwapChain
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHISwapChain_OpenGLImpl, OpenGL, ERHInterfaceType::OpenGL );
		RHISwapChain_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType & a_Desc );
		~RHISwapChain_OpenGLImpl() override { Release(); }

		bool Release() override;
		bool Valid() const override { return Window != nullptr; }
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

	class RHICommandList_OpenGLImpl : public IRHICommandList
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHICommandList_OpenGLImpl, OpenGL, ERHInterfaceType::OpenGL );
		RHICommandList_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType & a_Desc );
		~RHICommandList_OpenGLImpl() override { Release(); }

		bool Release() override;
		bool Valid() const override { return m_InlinedConstantsUBO.Valid(); }
		const void* NativePtr() const override { return nullptr; }

		bool IsImmediate() const override { return Desc().EnableImmediateExecution; }

		bool Open() override;
		bool Close() override;
		void ClearState() override;

		void ResourceBarriers( Span<const RHIResourceBarrier> a_Barriers, RHI_DEBUG_SRC_LOC_PARAM ) override;

		void UpdateBuffer( IRHIBuffer& a_Buffer, const void* a_Data, size_t a_DataSizeBytes, size_t a_DstOffsetBytes = 0, RHI_DEBUG_SRC_LOC_PARAM ) override;
		void CopyBuffer( IRHIBuffer& a_DstBuffer, size_t a_DstOffsetBytes, IRHIBuffer& a_SrcBuffer, RHIBufferRange a_SrcRange, RHI_DEBUG_SRC_LOC_PARAM ) override;

		void UpdateTexture( IRHITexture& a_Texture, const RHITextureSlice& a_DstSlice, RHITextureSubresourceData a_Data, RHI_DEBUG_SRC_LOC_PARAM ) override;
		void CopyTexture( IRHITexture& a_DstTexture, const RHITextureSlice& a_DstSlice, IRHITexture& a_SrcTexture, const RHITextureSlice& a_SrcSlice, RHI_DEBUG_SRC_LOC_PARAM ) override;

		void SetInlinedConstants( const void* a_Data, uint32_t a_SizeBytes, uint32_t a_DstOffsetBytes = 0, RHI_DEBUG_SRC_LOC_PARAM ) override;

		void SetGraphicsState( const RHIGraphicsState& a_GraphicsState, bool a_ClearViewportState, RHI_DEBUG_SRC_LOC_PARAM ) override;
		void ClearRenderTargets( ERHIClearFlags a_Flags, RHIClearValue a_ClearValue, int32_t a_ColorAttachmentIndex = -1, RHI_DEBUG_SRC_LOC_PARAM ) override;
		void SetViewportState( const RHIViewportState& a_Viewports, RHI_DEBUG_SRC_LOC_PARAM ) override;
		void Draw( const RHIDrawArgs& a_DrawArgs, RHI_DEBUG_SRC_LOC_PARAM ) override;

		void PushDebugGroup( StringView a_Name ) override;
		void PopDebugGroup() override;
		void InsertDebugMarker( StringView a_Name ) override;

		// OpenGL-specific functions
		// Used by DynamicRHI_OpenGLImpl to the command list
		void Flush();

	private:
		GLUBOWrapper m_InlinedConstantsUBO{};
		GLFramebufferWrapper m_FramebufferObj{};
		Array<RHIObjectRef> m_ReferencedObjects{};

		RHIGraphicsState m_CurrentGraphicsState{};
		bool m_GraphicsStateValid = false;
		RHIViewportState m_ViewportState{};
		RHIComputeState m_CurrentComputeState{};
		bool m_ComputeStateValid = false;

		//=======================================================================
		// Command Buffer
		//  Buffer to queue commands for execution
		//  This is so OpenGL can have deferred execution of commands.
		//  Is not used if the RHICommandList is immediate.
		struct CommandBuffer
		{
			struct UpdateBuffer
			{
				RHIBufferRef Buffer;
				const void* Data;
				size_t DataSizeBytes;
				size_t DstOffsetBytes;
			};

			struct CopyBuffer
			{
				RHIBufferRef DstBuffer;
				size_t DstOffsetBytes;
				RHIBufferRef SrcBuffer;
				RHIBufferRange SrcRange;
			};

			struct UpdateTexture
			{
				RHITextureRef Texture;
				RHITextureSlice DstSlice;
				RHITextureSubresourceData Data;
			};

			struct CopyTexture
			{
				RHITextureRef DstTexture;
				RHITextureSlice DstSlice;
				RHITextureRef SrcTexture;
				RHITextureSlice SrcSlice;
			};

			struct SetInlinedConstants
			{
				FixedArray<uint8_t, RHIConstants::MaxInlinedConstantsSize> Data;
				uint32_t DstOffsetBytes;
				uint32_t SizeBytes;
			};

			struct SetGraphicsState
			{
				RHIGraphicsState GraphicsState;
				bool ClearViewportState;
			};

			struct ClearRenderTargets
			{
				ERHIClearFlags Flags;
				RHIClearValue ClearValue;
				int32_t ColorAttachmentIndex;
			};

			struct SetViewportState
			{
				RHIViewportState Viewports;
			};

			struct Draw
			{
				RHIDrawArgs DrawArgs;
			};

			struct PushDebugGroup
			{
				StringView Name;
			};

			struct PopDebugGroup
			{
			};

			struct InsertDebugMarker
			{
				StringView Name;
			};

			using Command = Variant<
				UpdateBuffer,
				CopyBuffer,
				UpdateTexture,
				CopyTexture,
				SetInlinedConstants,
				SetGraphicsState,
				ClearRenderTargets,
				SetViewportState,
				Draw,
				PushDebugGroup,
				PopDebugGroup,
				InsertDebugMarker
			>;

			// Matches the index into the Command variant
			enum class CommandType
			{
				UpdateBuffer = 0,
				CopyBuffer,
				UpdateTexture,
				CopyTexture,
				SetInlinedConstants,
				SetGraphicsState,
				ClearRenderTargets,
				SetViewportState,
				Draw,
				PushDebugGroup,
				PopDebugGroup,
				InsertDebugMarker
			};

			Array<Command> Commands{};

			void Clear() { Commands.Clear(); }
		};

		struct Deferred
		{
			CommandBuffer CommandBuffer{};
		} m_Deferred{};

	private:
		void BindGraphicsPipelineState( const RHIGraphicsPipelineState_OpenGLImpl& a_GraphicsPipelineState );
		void BindFramebuffer( const RHIFramebuffer& a_Framebuffer );
		void BindGraphicsBindings( const RHIGraphicsState& a_GraphicsState );

		void FlushCommandBuffer();

		// Command Implementations
		void UpdateBuffer_Impl( IRHIBuffer& a_Buffer, const void* a_Data, size_t a_DataSizeBytes, size_t a_DstOffsetBytes );
		void CopyBuffer_Impl( IRHIBuffer& a_DstBuffer, size_t a_DstOffsetBytes, IRHIBuffer& a_SrcBuffer, RHIBufferRange a_SrcRange );
		void UpdateTexture_Impl( IRHITexture& a_Texture, const RHITextureSlice& a_DstSlice, RHITextureSubresourceData a_Data );
		void CopyTexture_Impl( IRHITexture& a_DstTexture, const RHITextureSlice& a_DstSlice, IRHITexture& a_SrcTexture, const RHITextureSlice& a_SrcSlice );
		void SetInlinedConstants_Impl( const void* a_Data, uint32_t a_SizeBytes, uint32_t a_DstOffsetBytes = 0 );
		void SetGraphicsState_Impl( const RHIGraphicsState& a_GraphicsState, bool a_ClearViewportState );
		void ClearRenderTargets_Impl( ERHIClearFlags a_Flags, RHIClearValue a_ClearValue, int32_t a_ColorAttachmentIndex = -1 );
		void SetViewportState_Impl( const RHIViewportState& a_Viewports );
		void Draw_Impl( const RHIDrawArgs& a_DrawArgs );
		void PushDebugGroup_Impl( StringView a_Name );
		void PopDebugGroup_Impl();
		void InsertDebugMarker_Impl( StringView a_Name );
	};

	//======================================================================
	// DYNAMIC RHI IMPLEMENTATION
	//======================================================================

	struct GLResourceCache
	{
		UnorderedMap<RHISampler, GLSamplerWrapper> Samplers;
		UnorderedMap<RHISampler, GLSamplerWrapper> DepthSamplers;
		RHITextureRef NullTexture2D = nullptr; // 1x1 Black texture

		GLuint GetOrCreateSampler( const RHISampler& a_Sampler, bool a_IsDepth );
	};

	class DynamicRHI_OpenGLImpl final : public IDynamicRHI
	{
	public:

		//=============================================================================================
		// Core RHI functions
		bool Init( const RHIConfig& a_Config ) override;
		bool Shutdown() override;
		void BeginFrame() override;
		void EndFrame() override;
		RHIFenceValue ExecuteCommandLists( Span<IRHICommandList* const> a_CommandLists, ERHICommandQueueType a_QueueType ) override;
		bool WaitForIdle() override;
		void WaitForFence( ERHICommandQueueType a_QueueType, RHIFenceValue a_FenceValue ) override;
		void CollectGarbage() override;
		ERHInterfaceType GetRHIType() const override { return ERHInterfaceType::OpenGL; }
		static constexpr ERHInterfaceType GetStaticRHIType() { return ERHInterfaceType::OpenGL; }
		//=============================================================================================

		//=============================================================================================
		// Resource creation
		RHITextureRef CreateTexture( const RHITextureDesc& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData ) override;
		RHIBufferRef CreateBuffer( const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data ) override;
		RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDesc& a_Desc ) override;
		RHICommandListRef CreateCommandList( const RHICommandListDesc& a_Desc ) override;
		RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDesc& a_Desc ) override;
		RHIBindingLayoutRef CreateBindingLayout( const RHIBindingLayoutDesc& a_Desc ) override;
		RHIBindingSetRef CreateBindingSet( const RHIBindingSetDesc& a_Desc ) override;
		RHISwapChainRef CreateSwapChain( const RHISwapChainDesc& a_Desc ) override;
		//=============================================================================================

		//=============================================================================================
		// Miscellaneous
		IRHISwapChain* GetSwapChain() const override { return m_SwapChain.get(); }
		GPUInfo GetGPUInfo() const override;
		bool QueryRHIStats( RHIStats& o_Stats ) const override;
		//=============================================================================================

	#if RHI_DEBUG_ENABLED
		// Dump debug information about the RHI into the console.
		void DumpDebug() override;
	#endif // RHI_DEBUG_ENABLED

		//======================================================
		// OpenGL-specific functions

		auto& ResourceCache() { return m_ResourceCache; }
		const auto& ResourceCache() const { return m_ResourceCache; }

	private:
		RHISwapChainRef m_SwapChain{};
		GLResourceCache m_ResourceCache;
	};

	inline DynamicRHI_OpenGLImpl* GetOpenGLRHI()
	{
		RHI_DEV_CHECK( s_DynamicRHI != nullptr && s_DynamicRHI->GetRHIType() == ERHInterfaceType::OpenGL, "Invalid RHI type!" );
		return Cast<DynamicRHI_OpenGLImpl*>( s_DynamicRHI );
	}

#pragma endregion

} // namespace Tridium::OpenGL