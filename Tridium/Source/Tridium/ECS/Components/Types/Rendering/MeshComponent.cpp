#include "tripch.h"
#include "MeshComponent.h"
#include <Tridium/Rendering/VertexArray.h>
namespace Tridium {

	BEGIN_REFLECT_COMPONENT( MeshComponent )
		BASE( Component )
		PROPERTY( m_Mesh, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( m_Material, FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( MeshComponent )

	MeshComponent::MeshComponent( const AssetRef<Mesh>& mesh )
		: m_Mesh( mesh )
	{
	}
	MeshComponent::MeshComponent()
	{
		static Mesh* mesh = new Mesh();
		{
			mesh->m_VAO = VertexArray::Create();
			float vertices[] = {
				// Front face
				-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   // Bottom-left (normal pointing outwards along z-axis)
				-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   // Top-left
				 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   // Bottom-right
				 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   // Top-right

				 // Back face
				 -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   // Bottom-left (normal pointing inwards along z-axis)
				 -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   // Top-left
				  0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   // Bottom-right
				  0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   // Top-right

				  // Left face
				  -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   // Bottom-left (normal pointing leftwards along x-axis)
				  -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   // Top-left
				  -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   // Bottom-right
				  -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   // Top-right

				  // Right face
				   0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   // Bottom-left (normal pointing rightwards along x-axis)
				   0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   // Top-left
				   0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   // Bottom-right
				   0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   // Top-right

				   // Top face
				   -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   // Top-left (normal pointing upwards along y-axis)
				   -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   // Top-right
					0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   // Bottom-left
					0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   // Bottom-right

					// Bottom face
					-0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   // Bottom-left (normal pointing downwards along y-axis)
					-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   // Bottom-right
					 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   // Top-left
					 0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f    // Top-right
			};

			uint32_t indices[] = {
				// Front face
				0, 1, 2,   1, 2, 3,
				// Back face
				4, 5, 6,   5, 6, 7,
				// Left face
				8, 9, 10,  9, 10, 11,
				// Right face
				12, 13, 14, 13, 14, 15,
				// Top face
				16, 17, 18, 17, 18, 19,
				// Bottom face
				20, 21, 22, 21, 22, 23
			};

			mesh->m_VBO = VertexBuffer::Create( vertices, sizeof( vertices ) );

			BufferLayout layout =
			{
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float3, "a_Normal" },
			};

			mesh->m_VBO->SetLayout( layout );
			mesh->m_VAO->AddVertexBuffer( mesh->m_VBO );

			mesh->m_IBO = IndexBuffer::Create( indices, sizeof( indices ) / sizeof( uint32_t ) );
			mesh->m_VAO->SetIndexBuffer( mesh->m_IBO );
		};

		static AssetRef<Mesh> meshref = mesh;

		m_Mesh = meshref;
	}
}