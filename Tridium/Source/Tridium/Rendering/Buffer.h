#pragma once
#include "Shader.h"


namespace Tridium {

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Offset;
		uint32_t Size;
		bool Normalised;

		BufferElement() {}
		BufferElement( ShaderDataType type, const std::string& name, bool normalised = false );

		uint32_t GetComponentCount() const;
	};



	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout( const std::initializer_list<BufferElement>& a_Elements );

		inline const auto& GetElements() const { return m_Elements; }
		inline const uint32_t GetStride() const { return m_Stride; }

		auto begin() { return m_Elements.begin(); }
		auto end() { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride();

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride;
	};



	class VertexBuffer
	{
	public:
		static Ref<VertexBuffer> Create(float* a_Verticies, uint32_t a_Size);
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout( const BufferLayout& a_Layout ) = 0;
	};



	class IndexBuffer
	{
	public:
		static Ref<IndexBuffer> Create( uint32_t* a_Indicies, uint32_t a_Count );
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;
	};

}