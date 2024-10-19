#include "tripch.h"
#include "Mesh.h"
#include <Tridium/Core/Application.h>

#include <Tridium/Rendering/Buffer.h>
#include <Tridium/Rendering/VertexArray.h>
#include <Tridium/Asset/AssetManager.h>

namespace Tridium {

	/////////////////////////////////////////////////////////////////////////////////////////
	// MeshSource
	/////////////////////////////////////////////////////////////////////////////////////////

	MeshSource::MeshSource( const std::vector<Vertex>& a_Vertices, const std::vector<uint32_t>& a_Indices, const Matrix4& a_Transform )
		: m_Vertices( a_Vertices ), m_Indices( a_Indices )
	{
		m_Handle = AssetHandle::Create();

		SubMesh submesh;
		submesh.BaseVertex = 0;
		submesh.BaseIndex = 0;
		submesh.NumIndicies = (uint32_t)a_Indices.size() * 3u;
		submesh.Transform = a_Transform;
		m_SubMeshes.push_back( submesh );

		m_VBO = VertexBuffer::Create( (float*)( m_Vertices.data() ), (uint32_t)( m_Vertices.size() * sizeof( Vertex ) ) );
		m_IBO = IndexBuffer::Create( m_Indices.data(), (uint32_t)( m_Indices.size() ) );
	}

	MeshSource::MeshSource( const std::vector<Vertex>& a_Vertices, const std::vector<uint32_t>& a_Indices, const std::vector<SubMesh>& a_SubMeshes )
		: m_Vertices( a_Vertices ), m_Indices( a_Indices ), m_SubMeshes( a_SubMeshes )
	{
		m_Handle = AssetHandle::Create();

		m_VBO = VertexBuffer::Create( (float*)( m_Vertices.data() ), (uint32_t)( m_Vertices.size() * sizeof( Vertex ) ) );
		m_IBO = IndexBuffer::Create( m_Indices.data(), (uint32_t)( m_Indices.size() ) );
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	// StaticMesh
	/////////////////////////////////////////////////////////////////////////////////////////

	StaticMesh::StaticMesh( AssetHandle a_MeshSource )
		: m_MeshSource( a_MeshSource )
	{
		m_Handle = AssetHandle::Create();
		
		if ( auto meshSourceRef = AssetManager::GetAsset<MeshSource>( a_MeshSource ) )
		{
			SetSubMeshes( meshSourceRef );
			m_VAO = meshSourceRef->GetVAO();
			m_IBO = meshSourceRef->GetIBO();

			const std::vector<AssetHandle>& meshMaterials = meshSourceRef->GetMaterials();
			uint32_t numMaterials = static_cast<uint32_t>( meshMaterials.size() );
			for ( uint32_t i = 0; i < numMaterials; i++ )
				m_Materials[i] = meshMaterials[i];
		}
	}

	StaticMesh::StaticMesh( AssetHandle a_MeshSource, const std::vector<uint32_t>& a_SubMeshes )
		: m_MeshSource( a_MeshSource )
	{
		m_Handle = AssetHandle::Create();

		if ( auto meshSourceRef = AssetManager::GetAsset<MeshSource>( a_MeshSource ) )
		{
			if ( !a_SubMeshes.empty() )
				SetSubMeshes( a_SubMeshes );
			else
				SetSubMeshes( meshSourceRef );

			const std::vector<AssetHandle>& meshMaterials = meshSourceRef->GetMaterials();
			uint32_t numMaterials = static_cast<uint32_t>( meshMaterials.size() );
			for ( uint32_t i = 0; i < numMaterials; i++ )
				m_Materials.push_back( meshMaterials[i] );
		}
	}

	void StaticMesh::SetSubMeshes( const std::vector<uint32_t>& a_SubMeshes )
	{
		m_SubMeshes = a_SubMeshes;
	}

	void StaticMesh::SetSubMeshes( SharedPtr<MeshSource> a_MeshSource )
	{
		const auto& subMeshes = a_MeshSource->GetSubMeshes();
		m_SubMeshes.resize( subMeshes.size() );
		for ( uint32_t i = 0; i < subMeshes.size(); ++i )
			m_SubMeshes[i] = i;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	// MeshFactory
	/////////////////////////////////////////////////////////////////////////////////////////

    StaticMesh* MeshFactory::CreateQuad()
    {
		return nullptr;
		//StaticMesh* mesh = new StaticMesh();
  //      mesh->m_VAO = VertexArray::Create();

  //      float vertices[] = {
  //          -0.5f, -0.5f,  0, 0.0f, 0.0f,
  //          -0.5f,  0.5f,  0, 0.0f, 1.0f,
  //           0.5f, -0.5f,  0, 1.0f, 0.0f,
  //           0.5f,  0.5f,  0, 1.0f, 1.0f
  //      };

  //      uint32_t indices[] = { 0,1,2,1,2,3 };

  //      mesh->m_VBO = VertexBuffer::Create( vertices, sizeof( vertices ) );

  //      BufferLayout layout =
  //      {
  //          { ShaderDataType::Float3, "aPosition" },
  //          { ShaderDataType::Float2, "aUV" },
  //      };

  //      mesh->m_VBO->SetLayout( layout );
  //      mesh->m_VAO->AddVertexBuffer( mesh->m_VBO );

  //      mesh->m_IBO = IndexBuffer::Create( indices, sizeof( indices ) / sizeof( uint32_t ) );
  //      mesh->m_VAO->SetIndexBuffer( mesh->m_IBO );

		//return mesh;
    }

    StaticMesh* MeshFactory::CreateCube()
    {
		return nullptr;
		//StaticMesh* mesh = new StaticMesh();
		//mesh->m_VAO = VertexArray::Create();
		//float vertices[] = {
		//	// Front face
		//	-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   // Bottom-left (normal pointing outwards along z-axis)
		//	-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   // Top-left
		//	 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   // Bottom-right
		//	 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   // Top-right

		//	 // Back face
		//	 -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   // Bottom-left (normal pointing inwards along z-axis)
		//	 -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   // Top-left
		//	  0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   // Bottom-right
		//	  0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   // Top-right

		//	  // Left face
		//	  -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   // Bottom-left (normal pointing leftwards along x-axis)
		//	  -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   // Top-left
		//	  -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   // Bottom-right
		//	  -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   // Top-right

		//	  // Right face
		//	   0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   // Bottom-left (normal pointing rightwards along x-axis)
		//	   0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   // Top-left
		//	   0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   // Bottom-right
		//	   0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   // Top-right

		//	   // Top face
		//	   -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   // Top-left (normal pointing upwards along y-axis)
		//	   -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   // Top-right
		//		0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   // Bottom-left
		//		0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   // Bottom-right

		//		// Bottom face
		//		-0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   // Bottom-left (normal pointing downwards along y-axis)
		//		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   // Bottom-right
		//		 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   // Top-left
		//		 0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f    // Top-right
		//};

		//uint32_t indices[] = {
		//	// Front face
		//	0, 1, 2,   1, 2, 3,
		//	// Back face
		//	4, 5, 6,   5, 6, 7,
		//	// Left face
		//	8, 9, 10,  9, 10, 11,
		//	// Right face
		//	12, 13, 14, 13, 14, 15,
		//	// Top face
		//	16, 17, 18, 17, 18, 19,
		//	// Bottom face
		//	20, 21, 22, 21, 22, 23
		//};

		//mesh->m_VBO = VertexBuffer::Create( vertices, sizeof( vertices ) );

		//BufferLayout layout =
		//{
		//	{ ShaderDataType::Float3, "a_Position" },
		//	{ ShaderDataType::Float3, "a_Normal" },
		//};

		//mesh->m_VBO->SetLayout( layout );
		//mesh->m_VAO->AddVertexBuffer( mesh->m_VBO );

		//mesh->m_IBO = IndexBuffer::Create( indices, sizeof( indices ) / sizeof( uint32_t ) );
		//mesh->m_VAO->SetIndexBuffer( mesh->m_IBO );

		//return mesh;
    }

	AssetHandle MeshFactory::GetQuad()
	{
		static AssetHandle s_QuadHandle = 1u;
		static bool s_QuadCreated = ( AssetManager::AddMemoryOnlyAsset( s_QuadHandle, SharedPtr<StaticMesh>( CreateQuad() ) ), true );
		return s_QuadHandle;
	}

	AssetHandle MeshFactory::GetCube()
	{
		static AssetHandle s_CubeHandle = 2u;
		static bool s_CubeCreated = ( AssetManager::AddMemoryOnlyAsset( s_CubeHandle, SharedPtr<StaticMesh>( CreateCube() ) ), true );
		return s_CubeHandle;
	}
}