#pragma once
#include <Tridium/ECS/Components/Component.h>

#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Buffer.h>
#include <Tridium/Rendering/VertexArray.h>

namespace Tridium {

	// TEMP
	struct Quad
	{
		Ref<Shader> m_Shader;
		Ref<VertexArray> VAO;
		Ref<VertexBuffer> VBO;
		Ref<IndexBuffer> IBO;

		Quad()
		{
			VAO = VertexArray::Create();

			// QUAD
			float vertices[] = {
				-1, -1,  0,		 0.0f, 0.0f,
				-1,  1,  0,		 0.0f, 1.0f,
				 1, -1,  0,		 1.0f, 0.0f,
				 1,  1,  0,		 1.0f, 1.0f
			};

			uint32_t indices[] = { 0,1,2,3,1,2 };

			VBO = VertexBuffer::Create( vertices, sizeof( vertices ) );

			BufferLayout layout =
			{
				{ ShaderDataType::Float3, "aPosition" },
				{ ShaderDataType::Float2, "a" }
			};

			VBO->SetLayout( layout );
			VAO->AddVertexBuffer( VBO );

			IBO = IndexBuffer::Create( indices, sizeof( indices ) / sizeof( uint32_t ) );
			VAO->SetIndexBuffer( IBO );
		}
	};

	class Texture2D;

	DEFINE_COMPONENT( SpriteComponent )
	{
	public:
		SpriteComponent();
		SpriteComponent(const Ref<Texture2D>& a_Texture);
		~SpriteComponent() = default;

		inline Quad& GetMesh() { return m_Mesh; }
		inline Ref<Texture2D>& GetTexture() { return m_Texture; }
		inline void SetTexture(const  Ref<Texture2D>& a_Texture ) { m_Texture = a_Texture; }
		inline Ref<Shader>& GetShader() { return m_Shader; }

	private:
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_Texture;
		Quad m_Mesh;
	};

}