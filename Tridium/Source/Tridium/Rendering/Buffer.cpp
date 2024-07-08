#include "tripch.h"
#include "Buffer.h"

#include "Tridium/Rendering/RenderingAPI.h"

#include <Platform/OpenGL/OpenGLBuffer.h>

namespace Tridium {

#pragma region - Vertex Buffer -

	Ref<VertexBuffer> VertexBuffer::Create( float* a_Verticies, uint32_t a_Size )
	{
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return MakeRef<OpenGLVertexBuffer>( a_Verticies, a_Size );
			break;
		default:
			return nullptr;
			break;
		}
	
	}

#pragma endregion

#pragma region - Index Buffer -

	Ref<IndexBuffer> IndexBuffer::Create( uint32_t* a_Indicies, uint32_t a_Count )
	{
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return MakeRef<OpenGLIndexBuffer>( a_Indicies, a_Count );
			break;
		default:
			return nullptr;
			break;
		}
	}

#pragma endregion

#pragma region - Buffer Element -

	BufferElement::BufferElement( ShaderDataType type, const std::string& name, uint32_t count, bool normalised )
		: Name( name ), Type( type ), Size( ShaderDataTypeSize( type ) * count ), Offset( 0 ), Normalised( normalised )
	{
	}

	uint32_t BufferElement::GetComponentCount() const
	{
		switch ( Type )
		{
		case ShaderDataType::Float:    return 1;
		case ShaderDataType::Float2:   return 2;
		case ShaderDataType::Float3:   return 3;
		case ShaderDataType::Float4:   return 4;
		case ShaderDataType::Mat3:	   return 3 * 3;
		case ShaderDataType::Mat4:	   return 4 * 4;
		case ShaderDataType::Int:	   return 1;
		case ShaderDataType::Int2:	   return 2;
		case ShaderDataType::Int3:	   return 3;
		case ShaderDataType::Int4:	   return 4;
		case ShaderDataType::Bool:	   return 1;
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