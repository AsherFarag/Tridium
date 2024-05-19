#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Rendering/Buffer.h>

namespace Tridium {

	class VertexArray
	{
	public:
		static Ref<VertexArray> Create();
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer( const Ref<VertexBuffer>& a_VertexBuffer ) = 0;
		virtual void SetIndexBuffer( const Ref<IndexBuffer>& a_IndexBuffer ) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;
	};

}