#pragma once
#include <Tridium/ECS/Components/Component.h>

#include <Tridium/Rendering/Mesh.h>

#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Buffer.h>
#include <Tridium/Rendering/VertexArray.h>

namespace Tridium {
	
	// TEMP
	struct OldMesh
	{
		Ref<Shader> m_Shader;
		Ref<VertexArray> VAO;
		Ref<VertexBuffer> VBO;
		Ref<IndexBuffer> IBO;

		OldMesh()
		{
			VAO = VertexArray::Create();

			// TRI
			//float vertices[ 3 * 3 + 3 * 4 ] = {
			//	0.f, 0.5f, 0.f,		 0.2f, 0.2f, 1.0f, 1,
			//	0.5f, -0.5f, 0.f,	 0.2f, 1.0f, 0.2f, 1,
			//	-0.5f, -0.5f, 0.f,	 1.0f, 0.2f, 0.2f, 1,
			//};

			//// QUAD
			//float vertices[ 6 * 3 + 6 * 4 ] = {
			//	-1, -1,  0,		 0.4f, 0.4f, 1.0f, 1,
			//	-1,  1,  0,		 0.4f, 1.0f, 0.4f, 1,
			//	 1, -1,  0,		 1.0f, 0.4f, 0.4f, 1,
			//	 1,  1,  0,		 1.0f, 1.0f, 0.4f, 1
			//};

			float vertices[] = {
				// Position         // Color
				-0.5f, -0.5f,  0.5f,         0.4f, 0.4f, 1.0f, 1,
				 0.5f, -0.5f,  0.5f,         0.4f, 1.0f, 0.4f, 1,
				 0.5f,  0.5f,  0.5f,         1.0f, 0.4f, 0.4f, 1,
				-0.5f,  0.5f,  0.5f,         1.0f, 1.0f, 0.4f, 1,
				-0.5f, -0.5f, -0.5f,         1.0f, 0.4f, 0.4f, 1,
				 0.5f, -0.5f, -0.5f,         0.4f, 1.0f, 0.4f, 1,
				 0.5f,  0.5f, -0.5f,         0.4f, 0.4f, 1.0f, 1,
				-0.5f,  0.5f, -0.5f,         1.0f, 1.0f, 0.4f, 1
			};

			// Indices for the cube
			unsigned int indices[] = {
				// Front face
				0, 1, 2,
				2, 3, 0,
				// Back face
				4, 5, 6,
				6, 7, 4,
				// Left face
				4, 0, 3,
				3, 7, 4,
				// Right face
				1, 5, 6,
				6, 2, 1,
				// Top face
				3, 2, 6,
				6, 7, 3,
				// Bottom face
				4, 5, 1,
				1, 0, 4
			};

			VBO = VertexBuffer::Create( vertices, sizeof( vertices ) );

			BufferLayout layout =
			{
				{ ShaderDataType::Float3, "aPosition" },
				{ ShaderDataType::Float4, "aColor" }
			};

			VBO->SetLayout( layout );
			VAO->AddVertexBuffer( VBO );

			//uint32_t indicies[ 3 ] = { 0,1,2 };
			//uint32_t indicies[ 6 ] = { 0,1,2,3,1,2 };
			IBO = IndexBuffer::Create( indices, sizeof( indices ) / sizeof( uint32_t ) );
			VAO->SetIndexBuffer( IBO );
		}
	};

	DEFINE_COMPONENT( MeshComponent )
	{
	public:
		MeshComponent();
		MeshComponent(const MeshHandle& meshHandle);
		~MeshComponent() = default;

		inline MeshHandle& GetMesh() { return m_Mesh; }
		inline Ref<Shader>& GetShader() { return m_Shader; }

		void SetMesh( const MeshHandle& meshHandle );

	private:
		Ref<Shader> m_Shader;
		MeshHandle m_Mesh = {};
	};

}