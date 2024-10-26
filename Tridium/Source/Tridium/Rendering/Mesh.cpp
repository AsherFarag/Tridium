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

		m_VAO = VertexArray::Create();

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Bitangent" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float2, "a_UV" }
		};

		m_VBO = VertexBuffer::Create( (float*)( m_Vertices.data() ), (uint32_t)( m_Vertices.size() * sizeof( Vertex ) ) );
		m_VBO->SetLayout( layout );
		m_VAO->AddVertexBuffer( m_VBO );

		m_IBO = IndexBuffer::Create( m_Indices.data(), (uint32_t)( m_Indices.size() ) );
		m_VAO->SetIndexBuffer( m_IBO );
	}

	MeshSource::MeshSource( const std::vector<Vertex>& a_Vertices, const std::vector<uint32_t>& a_Indices, const std::vector<SubMesh>& a_SubMeshes )
		: m_Vertices( a_Vertices ), m_Indices( a_Indices ), m_SubMeshes( a_SubMeshes )
	{
		m_Handle = AssetHandle::Create();

		m_VAO = VertexArray::Create();

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Bitangent" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float2, "a_UV" }
		};

		m_VBO = VertexBuffer::Create( (float*)( m_Vertices.data() ), (uint32_t)( m_Vertices.size() * sizeof( Vertex ) ) );
		m_VBO->SetLayout( layout );
		m_VAO->AddVertexBuffer( m_VBO );

		m_IBO = IndexBuffer::Create( m_Indices.data(), (uint32_t)( m_Indices.size() ) );
		m_VAO->SetIndexBuffer( m_IBO );
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

	void MeshFactory::Init()
	{
		GetDefaultQuad();
		GetDefaultCube();
		GetDefaultSphere();
		GetDefaultCylinder();
		GetDefaultCone();
		GetDefaultTorus();
	}

	SharedPtr<MeshSource> MeshFactory::CreateQuad( const Vector2& a_Size )
	{
		Vector2 size = a_Size / 2.0f;

		std::vector<Vertex> vertices;
		vertices.resize( 4 );
		vertices[0].Position = { -size.x, -size.y, 0.0f };
		vertices[1].Position = { -size.x,  size.y, 0.0f };
		vertices[2].Position = { size.x, -size.y, 0.0f };
		vertices[3].Position = { size.x,  size.y, 0.0f };

		// Set normals for each vertex (pointing out of the quad)
		for ( auto& vertex : vertices ) {
			vertex.Normal = { 0.0f, 0.0f, 1.0f };
		}

		// Define indices for two triangles that make up the quad, in counterclockwise order
		std::vector<uint32_t> indices = { 0, 1, 2, 1, 3, 2 };

		SharedPtr<MeshSource> meshSource = MakeShared<MeshSource>( vertices, indices, Matrix4( 1.0f ) );
		return meshSource;
	}

	SharedPtr<MeshSource> MeshFactory::CreateCube( const Vector3& a_Size )
    {
		Vector3 size = a_Size / 2.0f;

		std::vector<Vertex> vertices( 24 );  // 4 vertices per face, 6 faces

		// Front face
		vertices[0].Position = { -size.x, -size.y, size.z };
		vertices[1].Position = { size.x, -size.y, size.z };
		vertices[2].Position = { size.x, size.y, size.z };
		vertices[3].Position = { -size.x, size.y, size.z };
		for ( int i = 0; i < 4; ++i ) {
			vertices[i].Normal = { 0.0f, 0.0f, 1.0f };
			vertices[i].UV = { ( i % 2 == 0 ) ? 0.0f : 1.0f, ( i < 2 ) ? 0.0f : 1.0f };
		}

		// Back face
		vertices[4].Position = { size.x, -size.y, -size.z };
		vertices[5].Position = { -size.x, -size.y, -size.z };
		vertices[6].Position = { -size.x, size.y, -size.z };
		vertices[7].Position = { size.x, size.y, -size.z };
		for ( int i = 4; i < 8; ++i ) {
			vertices[i].Normal = { 0.0f, 0.0f, -1.0f };
			vertices[i].UV = { ( ( i - 4 ) % 2 == 0 ) ? 0.0f : 1.0f, ( i < 6 ) ? 0.0f : 1.0f };
		}

		// Top face
		vertices[8].Position = { -size.x, size.y, size.z };
		vertices[9].Position = { size.x, size.y, size.z };
		vertices[10].Position = { size.x, size.y, -size.z };
		vertices[11].Position = { -size.x, size.y, -size.z };
		for ( int i = 8; i < 12; ++i ) {
			vertices[i].Normal = { 0.0f, 1.0f, 0.0f };
			vertices[i].UV = { ( ( i - 8 ) % 2 == 0 ) ? 0.0f : 1.0f, ( i < 10 ) ? 0.0f : 1.0f };
		}

		// Bottom face
		vertices[12].Position = { -size.x, -size.y, -size.z };
		vertices[13].Position = { size.x, -size.y, -size.z };
		vertices[14].Position = { size.x, -size.y, size.z };
		vertices[15].Position = { -size.x, -size.y, size.z };
		for ( int i = 12; i < 16; ++i ) {
			vertices[i].Normal = { 0.0f, -1.0f, 0.0f };
			vertices[i].UV = { ( ( i - 12 ) % 2 == 0 ) ? 0.0f : 1.0f, ( i < 14 ) ? 0.0f : 1.0f };
		}

		// Right face
		vertices[16].Position = { size.x, -size.y, size.z };
		vertices[17].Position = { size.x, -size.y, -size.z };
		vertices[18].Position = { size.x, size.y, -size.z };
		vertices[19].Position = { size.x, size.y, size.z };
		for ( int i = 16; i < 20; ++i ) {
			vertices[i].Normal = { 1.0f, 0.0f, 0.0f };
			vertices[i].UV = { ( ( i - 16 ) % 2 == 0 ) ? 0.0f : 1.0f, ( i < 18 ) ? 0.0f : 1.0f };
		}

		// Left face
		vertices[20].Position = { -size.x, -size.y, -size.z };
		vertices[21].Position = { -size.x, -size.y, size.z };
		vertices[22].Position = { -size.x, size.y, size.z };
		vertices[23].Position = { -size.x, size.y, -size.z };
		for ( int i = 20; i < 24; ++i ) {
			vertices[i].Normal = { -1.0f, 0.0f, 0.0f };
			vertices[i].UV = { ( ( i - 20 ) % 2 == 0 ) ? 0.0f : 1.0f, ( i < 22 ) ? 0.0f : 1.0f };
		}

		// Define indices for the cube
		std::vector<uint32_t> indices = {
			0, 1, 2,  2, 3, 0,       // Front face
			4, 5, 6,  6, 7, 4,       // Back face
			8, 9, 10, 10, 11, 8,     // Top face
			12, 13, 14, 14, 15, 12,  // Bottom face
			16, 17, 18, 18, 19, 16,  // Right face
			20, 21, 22, 22, 23, 20   // Left face
		};

		// Calculate tangents and bitangents
		CalculateTangents( vertices, indices );

		// Create and return the mesh source
		SharedPtr<MeshSource> meshSource = MakeShared<MeshSource>( vertices, indices, Matrix4( 1.0f ) );
		return meshSource;
    }

	SharedPtr<MeshSource> MeshFactory::CreateSphere( float a_Radius, uint32_t a_Stacks, uint32_t a_Slices )
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		// Create vertices
		vertices.reserve( ( a_Stacks + 1 ) * ( a_Slices + 1 ) );
		for ( uint32_t i = 0; i <= a_Stacks; i++ )
		{
			float V = i / (float)a_Stacks;
			float phi = V * glm::pi<float>();

			for ( uint32_t j = 0; j <= a_Slices; j++ )
			{
				float U = j / (float)a_Slices;
				float theta = U * ( glm::pi<float>() * 2 );

				float x = cosf( theta ) * sinf( phi );
				float y = cosf( phi );
				float z = sinf( theta ) * sinf( phi );

				Vertex vertex;
				vertex.Position = { -x * a_Radius, -y * a_Radius, -z * a_Radius };
				vertex.Normal = { -x, -y, -z };
				vertex.UV = { U, V };
				vertices.push_back( vertex );
			}
		}

		// Create indices
		indices.reserve( a_Stacks * a_Slices * 6 );
		for ( uint32_t i = 0; i < a_Stacks; i++ )
		{
			for ( uint32_t j = 0; j < a_Slices; j++ )
			{
				indices.push_back( i * ( a_Slices + 1 ) + j );
				indices.push_back( ( i + 1 ) * ( a_Slices + 1 ) + j );
				indices.push_back( ( i + 1 ) * ( a_Slices + 1 ) + ( j + 1 ) );

				indices.push_back( i * ( a_Slices + 1 ) + j );
				indices.push_back( ( i + 1 ) * ( a_Slices + 1 ) + ( j + 1 ) );
				indices.push_back( i * ( a_Slices + 1 ) + ( j + 1 ) );
			}
		}

		CalculateTangents( vertices, indices );

		SharedPtr<MeshSource> meshSource = MakeShared<MeshSource>( vertices, indices, Matrix4( 1.0 ) );
		return meshSource;
	}

	SharedPtr<MeshSource> MeshFactory::CreateCylinder( float a_BaseRadius, float a_TopRadius, float a_Height, uint32_t a_Stacks, uint32_t a_Slices )
	{
		std::vector<Vector3> normals;
		std::vector<Vector3> unitCircleVertices;

		// Get unit circle
		{
			float sectorStep = ( 2 * glm::pi<float>() ) / a_Slices;
			float sectorAngle;  // radian
			unitCircleVertices.reserve( a_Slices + 1 );
			for ( int i = 0; i <= a_Slices; ++i )
			{
				sectorAngle = i * sectorStep;
				unitCircleVertices.emplace_back( cosf( sectorAngle ), sinf( sectorAngle ), 0.0f );
			}
		}

		// Get side normals
		{
			float sectorStep = 2 * glm::pi<float>() / a_Slices;
			float sectorAngle;  // radian

			// compute the normal vector at 0 degree first
			// tanA = (baseRadius-topRadius) / height
			float zAngle = atan2( a_BaseRadius - a_TopRadius, a_Height );
			float x0 = cos( zAngle );     // nx
			float y0 = 0;               // ny
			float z0 = sin( zAngle );     // nz

			// rotate (x0,y0,z0) per sector angle
			normals.reserve( a_Slices );
			for ( int i = 0; i <= a_Slices; ++i )
			{
				sectorAngle = i * sectorStep;
				normals.emplace_back( cos( sectorAngle ) * x0 - sin( sectorAngle ) * y0, sin( sectorAngle ) * x0 + cos( sectorAngle ) * y0, z0 );
			}
		}


		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		vertices.reserve( ( a_Stacks + 1 ) * ( a_Slices + 1 ) );
		indices.reserve( a_Stacks * a_Slices * 6 );

		float radius;

		// put vertices of side cylinder to array by scaling unit circle
		for ( int i = 0; i <= a_Stacks; ++i )
		{
			float z = -( a_Height * 0.5f ) + (float)i / a_Stacks * a_Height;      // vertex position z
			radius = a_BaseRadius + (float)i / a_Stacks * ( a_TopRadius - a_BaseRadius );     // lerp
			float t = 1.0f - (float)i / a_Stacks;   // top-to-bottom

			for ( int j = 0; j <= a_Slices; ++j )
			{
				Vector3& u = unitCircleVertices[j];

				Vertex& vertex = vertices.emplace_back();
				vertex.Position = { u.x * radius, u.y * radius, z };
				vertex.Normal = normals[j];
				vertex.UV = { (float)j / a_Slices, t };
			}
		}

		// remember where the base.top vertices start
		uint32_t baseVertexIndex = vertices.size();

		// put vertices of base of cylinder
		float z = -a_Height * 0.5f;
		{
			Vertex& vertex = vertices.emplace_back();
			vertex.Position = Vector3( 0.0f, 0.0f, z );
			vertex.Normal = Vector3( 0.0f, 0.0f, -1.0f );
			vertex.UV = Vector2( 0.5f, 0.5f );
		}

		for ( int i = 0; i < a_Slices; ++i )
		{
			Vector3& u = unitCircleVertices[i];

			Vertex& vertex = vertices.emplace_back();
			vertex.Position = { u.x * a_BaseRadius, u.y * a_BaseRadius, z };
			vertex.Normal = { 0.0f, 0.0f, -1.0f };
			vertex.UV = { -u.x * 0.5f + 0.5f, -u.y * 0.5f + 0.5f }; // flip horizontal
		}

		// remember where the base vertices start
		uint32_t topVertexIndex = vertices.size();

		// put vertices of top of cylinder
		z = a_Height * 0.5f;
		{
			Vertex& vertex = vertices.emplace_back();
			vertex.Position = Vector3( 0.0f, 0.0f, z );
			vertex.Normal = Vector3( 0.0f, 0.0f, 1.0f );
			vertex.UV = Vector2( 0.5f, 0.5f );
		}

		for ( int i = 0; i < a_Slices; ++i )
		{
			Vector3& u = unitCircleVertices[i];

			Vertex& vertex = vertices.emplace_back();
			vertex.Position = { u.x * a_TopRadius, u.y * a_TopRadius, z };
			vertex.Normal = { 0.0f, 0.0f, 1.0f };
			vertex.UV = { u.x * 0.5f + 0.5f, -u.y * 0.5f + 0.5f };
		}

		// put indices for sides
		uint32_t k1, k2;
		for ( int i = 0; i < a_Stacks; ++i )
		{
			k1 = i * ( a_Slices + 1 );     // beginning of current stack
			k2 = k1 + a_Slices + 1;      // beginning of next stack

			for ( int j = 0; j < a_Slices; ++j, ++k1, ++k2 )
			{
				// 2 triangles per sector
				indices.push_back( k1 );
				indices.push_back( k1 + 1 );
				indices.push_back( k2 );

				indices.push_back( k2 );
				indices.push_back( k1 + 1 );
				indices.push_back( k2 + 1 );
			}
		}

		// remember where the base indices start
		uint32_t baseIndex = indices.size();

		// put indices for base
		for ( int i = 0, k = baseVertexIndex + 1; i < a_Slices; ++i, ++k )
		{
			if ( i < ( a_Slices - 1 ) )
			{
				indices.push_back( baseVertexIndex );
				indices.push_back( k + 1 );
				indices.push_back( k );
			}
			else    // last triangle
			{
				indices.push_back( baseVertexIndex );
				indices.push_back( baseVertexIndex + 1 );
				indices.push_back( k );
			}
		}

		// remember where the base indices start
		uint32_t topIndex = indices.size();

		for ( int i = 0, k = topVertexIndex + 1; i < a_Slices; ++i, ++k )
		{
			if ( i < ( a_Slices - 1 ) )
			{
				indices.push_back( topVertexIndex );
				indices.push_back( k );
				indices.push_back( k + 1 );
			}
			else
			{
				indices.push_back( topVertexIndex );
				indices.push_back( k );
				indices.push_back( topVertexIndex + 1 );
			}
		}

#if 0
		float sectorStep = ( 2 * glm::pi<float>() ) / a_Slices;
		float sectorAngle;  // radian

		std::vector<Vector3> unitCircleVertices;
		unitCircleVertices.reserve( a_Slices + 1 );
		for ( int i = 0; i <= a_Slices; ++i )
		{
			sectorAngle = i * sectorStep;
			unitCircleVertices.emplace_back( cosf( sectorAngle ), sinf( sectorAngle ), 0.0f );
		}

		// Put side vertices to arrays
		for ( int i = 0; i < 2; ++i )
		{
			float h = -a_Height / 2.0f + i * a_Height;           // z value; -h/2 to h/2
			float t = 1.0f - i;                              // vertical tex coord; 1 to 0


			for ( int j = 0; j <= a_Slices; ++j )
			{
				Vector3 u = unitCircleVertices[j];
				Vertex& vertex = vertices.emplace_back();

				vertex.Position = { u.x * a_BaseRadius, u.y * a_BaseRadius, h };
				vertex.Normal = u;
				vertex.UV = { (float)j / a_Slices, t };
			}
		}

		// The starting index for the base/top surface
		// NOTE: it is used for generating indices later 
		uint32_t baseCenterIndex = (uint32_t)(vertices.size());
		uint32_t topCenterIndex = baseCenterIndex + a_Slices + 1; // include center vertex

		// Put base and top vertices to arrays
		for ( int i = 0; i < 2; ++i )
		{
			float h = -a_Height / 2.0f + i * a_Height;           // z value; -h/2 to h/2
			float nz = ( i == 0 ) ? -1.0f : 1.0f;               // normal z; -1 to 1

			// center point
			Vertex& vertex = vertices.emplace_back();
			vertex.Position = { 0.0f, 0.0f, h };
			vertex.Normal = { 0.0f, 0.0f, nz };
			vertex.UV = { 0.5f, 0.5f };

			for ( int j = 0; j < a_Slices; ++j )
			{
				Vector3 u = unitCircleVertices[j];
				Vertex& vertex = vertices.emplace_back();

				vertex.Position = { u.x * ( ( i == 0 ) ? a_BaseRadius : a_TopRadius ), u.y * ( ( i == 0 ) ? a_BaseRadius : a_TopRadius ), h };
				vertex.Normal = { 0.0f, 0.0f, nz };
				vertex.UV = { u.x * 0.5f + 0.5f, u.y * 0.5f + 0.5f };
			}
		}

		// generate CCW index list of cylinder triangles
		int k1 = 0;                         // 1st vertex index at base
		int k2 = a_Slices + 1;           // 1st vertex index at top

		// indices for the side surface
		for ( int i = 0; i < a_Slices; ++i, ++k1, ++k2 )
		{
			// 2 triangles per sector
			// k1 => k1+1 => k2
			indices.push_back( k1 );
			indices.push_back( k1 + 1 );
			indices.push_back( k2 );

			// k2 => k1+1 => k2+1
			indices.push_back( k2 );
			indices.push_back( k1 + 1 );
			indices.push_back( k2 + 1 );
		}
		
		// indices for the base surface
		//NOTE: baseCenterIndex and topCenterIndices are pre-computed during vertex generation
		//      please see the previous code snippet
		for ( int i = 0, k = baseCenterIndex + 1; i < a_Slices; ++i, ++k )
		{
			if ( i < a_Slices - 1 )
			{
				indices.push_back( baseCenterIndex );
				indices.push_back( k + 1 );
				indices.push_back( k );
			}
			else // last triangle
			{
				indices.push_back( baseCenterIndex );
				indices.push_back( baseCenterIndex + 1 );
				indices.push_back( k );
			}
		}

		// indices for the top surface
		for ( int i = 0, k = topCenterIndex + 1; i < a_Slices; ++i, ++k )
		{
			if ( i < a_Slices - 1 )
			{
				indices.push_back( topCenterIndex );
				indices.push_back( k );
				indices.push_back( k + 1 );
			}
			else // last triangle
			{
				indices.push_back( topCenterIndex );
				indices.push_back( k );
				indices.push_back( topCenterIndex + 1 );
			}
		}

#endif

		CalculateTangents( vertices, indices );

		SharedPtr<MeshSource> meshSource = MakeShared<MeshSource>( vertices, indices, Matrix4( 1.0 ) );
		return meshSource;
	}

	SharedPtr<MeshSource> MeshFactory::CreateTorus( float a_Radius, float a_Radius2, uint32_t a_Stacks, uint32_t a_Slices )
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		// Create vertices
		vertices.reserve( ( a_Stacks + 1 ) * ( a_Slices + 1 ) );
		for ( uint32_t i = 0; i <= a_Stacks; i++ )
		{
			float V = i / (float)a_Stacks;
			float phi = V * glm::pi<float>() * 2;

			for ( uint32_t j = 0; j <= a_Slices; j++ )
			{
				float U = j / (float)a_Slices;
				float theta = U * glm::pi<float>() * 2;

				float x = ( a_Radius + a_Radius2 * cosf( theta ) ) * cosf( phi );
				float y = ( a_Radius + a_Radius2 * cosf( theta ) ) * sinf( phi );
				float z = a_Radius2 * sinf( theta );

				Vertex vertex;
				vertex.Position = { x, y, z };
				vertex.Normal = glm::normalize( glm::vec3( x, y, z ) );
				vertex.UV = { U, V };
				vertices.push_back( vertex );
			}
		}

		// Create indices
		indices.reserve( a_Stacks * a_Slices * 6 );
		for ( uint32_t i = 0; i < a_Stacks; i++ )
		{
			for ( uint32_t j = 0; j < a_Slices; j++ )
			{
				indices.push_back( i * ( a_Slices + 1 ) + j );
				indices.push_back( ( i + 1 ) * ( a_Slices + 1 ) + j );
				indices.push_back( ( i + 1 ) * ( a_Slices + 1 ) + ( j + 1 ) );

				indices.push_back( i * ( a_Slices + 1 ) + j );
				indices.push_back( ( i + 1 ) * ( a_Slices + 1 ) + ( j + 1 ) );
				indices.push_back( i * ( a_Slices + 1 ) + ( j + 1 ) );
			}
		}


		CalculateTangents( vertices, indices );

		SharedPtr<MeshSource> meshSource = MakeShared<MeshSource>( vertices, indices, Matrix4( 1.0 ) );
		return meshSource;
	}

	AssetHandle MeshFactory::GetDefaultQuad()
	{
		static SharedPtr<MeshSource> s_QuadSource = CreateQuad();
		static AssetHandle s_QuadSourceHandle = AssetHandle::Create();
		static bool s_QuadSourceInit = AssetManager::AddMemoryOnlyAsset( s_QuadSourceHandle, s_QuadSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_QuadSourceHandle );
		static AssetHandle s_StaticMeshHandle = 1;
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	AssetHandle MeshFactory::GetDefaultCube()
	{
		static SharedPtr<MeshSource> s_CubeSource = CreateCube();
		static AssetHandle s_CubeSourceHandle = AssetHandle::Create();
		static bool s_CubeSourceInit = AssetManager::AddMemoryOnlyAsset( s_CubeSourceHandle, s_CubeSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_CubeSourceHandle );
		static AssetHandle s_StaticMeshHandle = 2;
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	AssetHandle MeshFactory::GetDefaultSphere()
	{
		static SharedPtr<MeshSource> s_SphereSource = CreateSphere();
		static AssetHandle s_SphereSourceHandle = AssetHandle::Create();
		static bool s_SphereSourceInit = AssetManager::AddMemoryOnlyAsset( s_SphereSourceHandle, s_SphereSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_SphereSourceHandle );
		static AssetHandle s_StaticMeshHandle = 3;
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	AssetHandle MeshFactory::GetDefaultCylinder()
	{
		static SharedPtr<MeshSource> s_CylinderSource = CreateCylinder();
		static AssetHandle s_CylinderSourceHandle = AssetHandle::Create();
		static bool s_CylinderSourceInit = AssetManager::AddMemoryOnlyAsset( s_CylinderSourceHandle, s_CylinderSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_CylinderSourceHandle );
		static AssetHandle s_StaticMeshHandle = 4;
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	AssetHandle MeshFactory::GetDefaultCone()
	{
		static SharedPtr<MeshSource> s_ConeSource = CreateCone();
		static AssetHandle s_ConeSourceHandle = AssetHandle::Create();
		static bool s_ConeSourceInit = AssetManager::AddMemoryOnlyAsset( s_ConeSourceHandle, s_ConeSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_ConeSourceHandle );
		static AssetHandle s_StaticMeshHandle = 5;
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	AssetHandle MeshFactory::GetDefaultTorus()
	{
		static SharedPtr<MeshSource> s_TorusSource = CreateTorus();
		static AssetHandle s_TorusSourceHandle = AssetHandle::Create();
		static bool s_TorusSourceInit = AssetManager::AddMemoryOnlyAsset( s_TorusSourceHandle, s_TorusSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_TorusSourceHandle );
		static AssetHandle s_StaticMeshHandle = 6;
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	void CalculateTangents( std::vector<Vertex>& a_Vertices, const std::vector<uint32_t>& a_Indices )
	{
		for ( size_t i = 0; i < a_Indices.size(); i += 3 ) {
			// Get the vertices of the triangle
			Vertex& v0 = a_Vertices[a_Indices[i]];
			Vertex& v1 = a_Vertices[a_Indices[i + 1]];
			Vertex& v2 = a_Vertices[a_Indices[i + 2]];

			// Positions
			Vector3 p0 = v0.Position;
			Vector3 p1 = v1.Position;
			Vector3 p2 = v2.Position;

			// Texture coordinates
			Vector2 uv0 = v0.UV;
			Vector2 uv1 = v1.UV;
			Vector2 uv2 = v2.UV;

			// Edges of the triangle
			Vector3 edge1 = p1 - p0;
			Vector3 edge2 = p2 - p0;

			// Delta UV
			Vector2 deltaUV1 = uv1 - uv0;
			Vector2 deltaUV2 = uv2 - uv0;

			// Calculate the inverse determinant
			float f = 1.0f / ( deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x );

			// Calculate tangent and bitangent
			Vector3 tangent;
			tangent.x = f * ( deltaUV2.y * edge1.x - deltaUV1.y * edge2.x );
			tangent.y = f * ( deltaUV2.y * edge1.y - deltaUV1.y * edge2.y );
			tangent.z = f * ( deltaUV2.y * edge1.z - deltaUV1.y * edge2.z );
			tangent = normalize( tangent );

			Vector3 bitangent;
			bitangent.x = f * ( -deltaUV2.x * edge1.x + deltaUV1.x * edge2.x );
			bitangent.y = f * ( -deltaUV2.x * edge1.y + deltaUV1.x * edge2.y );
			bitangent.z = f * ( -deltaUV2.x * edge1.z + deltaUV1.x * edge2.z );
			bitangent = normalize( bitangent );

			// Assign the tangent and bitangent to each vertex of the triangle
			v0.Tangent = tangent;
			v1.Tangent = tangent;
			v2.Tangent = tangent;

			v0.Bitangent = bitangent;
			v1.Bitangent = bitangent;
			v2.Bitangent = bitangent;
		}
	}
}