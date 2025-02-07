#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Graphics/Rendering/Buffer.h>

namespace Tridium {

	class VertexArray
	{
	public:
		static SharedPtr<VertexArray> Create();
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer( const SharedPtr<VertexBuffer>& a_VertexBuffer ) = 0;
		virtual void SetIndexBuffer( const SharedPtr<IndexBuffer>& a_IndexBuffer ) = 0;

		virtual const std::vector<SharedPtr<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const SharedPtr<IndexBuffer>& GetIndexBuffer() const = 0;
	};

}