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

	SharedPtr<VertexArray> MeshFactory::CreateQuad()
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

	SharedPtr<VertexArray> MeshFactory::CreateCube()
    {
		SharedPtr<VertexArray> vao = VertexArray::Create();

		Vector3 size = { 1.0f, 1.0f, 1.0f };

		std::vector<Vertex> vertices;
		vertices.resize( 8 );
		vertices[0].Position = { -size.x / 2.0f, -size.y / 2.0f,  size.z / 2.0f };
		vertices[1].Position = { size.x / 2.0f, -size.y / 2.0f,  size.z / 2.0f };
		vertices[2].Position = { size.x / 2.0f,  size.y / 2.0f,  size.z / 2.0f };
		vertices[3].Position = { -size.x / 2.0f,  size.y / 2.0f,  size.z / 2.0f };
		vertices[4].Position = { -size.x / 2.0f, -size.y / 2.0f, -size.z / 2.0f };
		vertices[5].Position = { size.x / 2.0f, -size.y / 2.0f, -size.z / 2.0f };
		vertices[6].Position = { size.x / 2.0f,  size.y / 2.0f, -size.z / 2.0f };
		vertices[7].Position = { -size.x / 2.0f,  size.y / 2.0f, -size.z / 2.0f };

		vertices[0].Normal = { -1.0f, -1.0f,  1.0f };
		vertices[1].Normal = { 1.0f, -1.0f,  1.0f };
		vertices[2].Normal = { 1.0f,  1.0f,  1.0f };
		vertices[3].Normal = { -1.0f,  1.0f,  1.0f };
		vertices[4].Normal = { -1.0f, -1.0f, -1.0f };
		vertices[5].Normal = { 1.0f, -1.0f, -1.0f };
		vertices[6].Normal = { 1.0f,  1.0f, -1.0f };
		vertices[7].Normal = { -1.0f,  1.0f, -1.0f };

		std::vector<uint32_t> indices;
		indices.resize( 12 * 3 );
		indices[0] = 0; indices[1] = 1; indices[2] = 2;
		indices[3] = 2; indices[4] = 3; indices[5] = 0;
		indices[6] = 1; indices[7] = 5; indices[8] = 6;
		indices[9] = 6; indices[10] = 2; indices[11] = 1;
		indices[12] = 7; indices[13] = 6; indices[14] = 5;
		indices[15] = 5; indices[16] = 4; indices[17] = 7;
		indices[18] = 4; indices[19] = 0; indices[20] = 3;
		indices[21] = 3; indices[22] = 7; indices[23] = 4;
		indices[24] = 4; indices[25] = 5; indices[26] = 1;
		indices[27] = 1; indices[28] = 0; indices[29] = 4;
		indices[30] = 3; indices[31] = 2; indices[32] = 6;
		indices[33] = 6; indices[34] = 7; indices[35] = 3;

		SharedPtr<VertexBuffer> vbo = VertexBuffer::Create( &vertices.data()->Position.x, vertices.size() * sizeof(Vertex));

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Bitangent" },
			{ ShaderDataType::Float2, "a_UV" },
		};

		vbo->SetLayout( layout );
		vao->AddVertexBuffer( vbo );

		SharedPtr<IndexBuffer> ibo = IndexBuffer::Create( indices.data(), indices.size() );
		vao->SetIndexBuffer( ibo );

		return vao;
    }

	AssetHandle MeshFactory::GetQuad()
	{
		static AssetHandle s_QuadHandle = 1u;
		//static bool s_QuadCreated = ( AssetManager::AddMemoryOnlyAsset( s_QuadHandle, SharedPtr<StaticMesh>( CreateQuad() ) ), true );
		return s_QuadHandle;
	}

	AssetHandle MeshFactory::GetCube()
	{
		static AssetHandle s_CubeHandle = 2u;
		//static bool s_CubeCreated = ( AssetManager::AddMemoryOnlyAsset( s_CubeHandle, SharedPtr<StaticMesh>( CreateCube() ) ), true );
		return s_CubeHandle;
	}
}