#pragma once
#include "OpenGLUtil.h"

DECLARE_LOG_CATEGORY( OpenGL );

#if RHI_DEBUG_ENABLED
#define OPENGL_SET_DEBUG_NAME( _ObjectType, _Handle, _Name ) \
	do { \
		if ( ::Tridium::RHIQuery::IsDebug() && !_Name.empty() ) \
		{ \
			OpenGL4::ObjectLabel( GL_TEXTURE, _Handle, _Name.size(), static_cast<const GLchar*>( _Name.data() ) ); \
		} \
	} while ( false )
#else
#define OPENGL_SET_DEBUG_NAME( _ObjectType, _Handle, _Name )
#endif