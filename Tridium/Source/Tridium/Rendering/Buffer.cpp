#include "tripch.h"
#include "Buffer.h"

#include "Tridium/Rendering/RenderingAPI.h"

#include <Platform/OpenGL/OpenGLBuffer.h>

namespace Tridium {

#pragma region - Vertex Buffer -

	SharedPtr<VertexBuffer> VertexBuffer::Create( float* a_Verticies, uint32_t a_Size )
	{
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return MakeShared<OpenGLVertexBuffer>( a_Verticies, a_Size );
			break;
		default:
			return nullptr;
			break;
		}
	
	}

#pragma endregion

#pragma region - Index Buffer -

	SharedPtr<IndexBuffer> IndexBuffer::Create( uint32_t* a_Indicies, uint32_t a_Count )
	{
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return MakeShared<OpenGLIndexBuffer>( a_Indicies, a_Count );
			break;
		default:
			return nullptr;
			break;
		}
	}

#pragma endregion

#pragma region - Buffer Element -

	BufferElement::BufferElement( EShaderDataType type, const std::string& name, uint32_t count, bool normalised )
		: Name( name ), Type( type ), Size( ShaderDataTypeSize( type ) * count ), Offset( 0 ), Normalised( normalised )
	{
	}

	uint32_t BufferElement::GetComponentCount() const
	{
		switch ( Type )
		{
		case EShaderDataType::Float:    return 1;
		case EShaderDataType::Float2:   return 2;
		case EShaderDataType::Float3:   return 3;
		case EShaderDataType::Float4:   return 4;
		case EShaderDataType::Mat3:	   return 3 * 3;
		case EShaderDataType::Mat4:	   return 4 * 4;
		case EShaderDataType::Int:	   return 1;
		case EShaderDataType::Int2:	   return 2;
		case EShaderDataType::Int3:	   return 3;
		case EShaderDataType::Int4:	   return 4;
		case EShaderDataType::Bool:	   return 1;
		}

		TE_CORE_ASSERT( false, "Unkown ShaderDataType!" );
		return 0;
	}

#pragma endregion

#pragma region - Buffer Layout -

	BufferLayout::BufferLayout( const std::initializer_list<BufferElement>& a_Elements )
		: m_Elements( a_Elements )
	{
		CalculateOffsetsAndStride();
	}

	void BufferLayout::CalculateOffsetsAndStride()
	{
		uint32_t offset = 0;
		m_Stride = 0;
		for ( auto& element : m_Elements )
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}

#pragma endregion

}