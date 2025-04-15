#include "tripch.h"
#include "OpenGLTexture.h"

namespace Tridium {

	//bool OpenGLTexture::Commit( const void* a_Params )
	//{
	//	const auto* desc = ParamsToDescriptor<RHITextureDescriptor>( a_Params );
	//	if ( desc->Dimension != ERHITextureDimension::Texture2D || desc->Depth != 1 || desc->Mips != 1 )
	//	{
	//		return false;
	//	}

	//	GLFormat = RHIToGLTextureFormat( desc->Format );
	//	if ( !GLFormat.IsValid() )
	//	{
	//		return false;
	//	}

	//	TextureObj.Create();
	//	TextureObj.SetName( desc->Name );

	//	return TextureObj.Valid();
	//}

	bool OpenGLTexture::Release()
	{
		TextureObj.Release();
		GLFormat = {};

		return true;
	}

	size_t OpenGLTexture::GetSizeInBytes() const
	{
		if ( !IsValid() )
		{
			return 0;
		}

		GLint prevBinding = 0;
		OpenGL1::GetIntegerv( GL_TEXTURE_BINDING_2D, &prevBinding );
		OpenGL1::BindTexture( GL_TEXTURE_2D, TextureObj );
		GLint size = 0;
		OpenGL1::GetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &size );
		OpenGL1::BindTexture( GL_TEXTURE_2D, prevBinding );
		return size;
	}

	bool OpenGLTexture::Commit( const RHITextureDescriptor& a_Desc )
	{
		m_Descriptor = a_Desc;
		return true;
	}

} // namespace Tridium::GL