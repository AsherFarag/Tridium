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
	{
		m_Handle = AssetHandle::Create();

		SubMesh& submesh = m_SubMeshes.emplace_back();
		submesh.Vertices = a_Vertices;
		submesh.Indices = a_Indices;
		submesh.Transform = a_Transform;

		submesh.VAO = VertexArray::Create();

		BufferLayout layout =
		{
			{ EShaderDataType::Float3, "a_Position" },
			{ EShaderDataType::Float3, "a_Normal" },
			{ EShaderDataType::Float3, "a_Bitangent" },
			{ EShaderDataType::Float3, "a_Tangent" },
			{ EShaderDataType::Float2, "a_UV" }
		};

		submesh.VBO = VertexBuffer::Create( (float*)( submesh.Vertices.data() ), (uint32_t)( submesh.Vertices.size() * sizeof( Vertex ) ) );
		submesh.VBO->SetLayout( layout );
		submesh.VAO->AddVertexBuffer( submesh.VBO );

		submesh.IBO = IndexBuffer::Create( submesh.Indices.data(), (uint32_t)( submesh.Indices.size() ) );
		submesh.VAO->SetIndexBuffer( submesh.IBO );
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	// StaticMesh
	/////////////////////////////////////////////////////////////////////////////////////////

	StaticMesh::StaticMesh( MeshSourceHandle a_MeshSource )
		: m_MeshSource( a_MeshSource )
	{
		m_Handle = AssetHandle::Create();
		
		if ( auto meshSourceRef = AssetManager::GetAsset<MeshSource>( a_MeshSource ) )
		{
			SetSubMeshes( meshSourceRef );

			const std::vector<MaterialHandle>& meshMaterials = meshSourceRef->GetMaterials();
			uint32_t numMaterials = static_cast<uint32_t>( meshMaterials.size() );
			m_Materials.resize( numMaterials );
			for ( uint32_t i = 0; i < numMaterials; i++ )
				m_Materials[i] = meshMaterials[i];
		}
	}

	StaticMesh::StaticMesh( MeshSourceHandle a_MeshSource, const std::vector<uint32_t>& a_SubMeshes )
		: m_MeshSource( a_MeshSource )
	{
		m_Handle = AssetHandle::Create();

		if ( auto meshSourceRef = AssetManager::GetAsset<MeshSource>( a_MeshSource ) )
		{
			if ( !a_SubMeshes.empty() )
				SetSubMeshes( a_SubMeshes );
			else
				SetSubMeshes( meshSourceRef );

			const std::vector<MaterialHandle>& meshMaterials = meshSourceRef->GetMaterials();
			uint32_t numMaterials = static_cast<uint32_t>( meshMaterials.size() );
			m_Materials.resize( numMaterials );
			for ( uint32_t i = 0; i < numMaterials; i++ )
				m_Materials[i] = meshMaterials[i];
		}
	}

	void StaticMesh::SetSubMeshes( const std::vector<uint32_t>& a_SubMeshes )
	{
		m_SubMeshes = a_SubMeshes;
	}

	void StaticMesh::SetSubMeshes( SharedPtr<MeshSource> a_MeshSource )
	{
		const auto& submeshes = a_MeshSource->GetSubMeshes();
		m_SubMeshes.resize( submeshes.size() );
		for ( uint32_t i = 0; i < submeshes.size(); i++ )
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

		// Get unit circle in X-Z plane
		{
			float sectorStep = ( 2 * glm::pi<float>() ) / a_Slices;
			float sectorAngle;  // radian
			unitCircleVertices.reserve( a_Slices + 1 );
			for ( int i = 0; i <= a_Slices; ++i )
			{
				sectorAngle = i * sectorStep;
				unitCircleVertices.emplace_back( cosf( sectorAngle ), 0.0f, sinf( sectorAngle ) ); // X-Z circle
			}
		}

		// Get side normals
		{
			float sectorStep = 2 * glm::pi<float>() / a_Slices;
			float sectorAngle;  // radian
			float yAngle = atan2( a_BaseRadius - a_TopRadius, a_Height ); // Now affects Y-axis
			float x0 = cos( yAngle );     // nx
			float y0 = sin( yAngle );     // ny
			float z0 = 0.0f;            // nz

			// Rotate normal per sector angle
			normals.reserve( a_Slices );
			for ( int i = 0; i <= a_Slices; ++i )
			{
				sectorAngle = i * sectorStep;
				normals.emplace_back( cos( sectorAngle ) * x0, y0, sin( sectorAngle ) * x0 );
			}
		}

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		vertices.reserve( ( a_Stacks + 1 ) * ( a_Slices + 1 ) );
		indices.reserve( a_Stacks * a_Slices * 6 );

		float radius;

		// Generate vertices along the Y-axis for cylinder sides
		for ( int i = 0; i <= a_Stacks; ++i )
		{
			float y = -( a_Height * 0.5f ) + (float)i / a_Stacks * a_Height; // vertex position along Y
			radius = a_BaseRadius + (float)i / a_Stacks * ( a_TopRadius - a_BaseRadius ); // lerp radius
			float t = 1.0f - (float)i / a_Stacks; // top-to-bottom

			for ( int j = 0; j <= a_Slices; ++j )
			{
				Vector3& u = unitCircleVertices[j];

				Vertex& vertex = vertices.emplace_back();
				vertex.Position = { u.x * radius, y, u.z * radius }; // X-Y-Z format
				vertex.Normal = normals[j];
				vertex.UV = { (float)j / a_Slices, t };
			}
		}

		// Index base vertices
		uint32_t baseVertexIndex = vertices.size();

		// Base of cylinder
		float y = -a_Height * 0.5f;
		{
			Vertex& vertex = vertices.emplace_back();
			vertex.Position = Vector3( 0.0f, y, 0.0f );
			vertex.Normal = Vector3( 0.0f, -1.0f, 0.0f );
			vertex.UV = Vector2( 0.5f, 0.5f );
		}

		for ( int i = 0; i < a_Slices; ++i )
		{
			Vector3& u = unitCircleVertices[i];

			Vertex& vertex = vertices.emplace_back();
			vertex.Position = { u.x * a_BaseRadius, y, u.z * a_BaseRadius };
			vertex.Normal = { 0.0f, -1.0f, 0.0f };
			vertex.UV = { -u.x * 0.5f + 0.5f, -u.z * 0.5f + 0.5f }; // flip horizontal
		}

		uint32_t topVertexIndex = vertices.size();

		// Top of cylinder
		y = a_Height * 0.5f;
		{
			Vertex& vertex = vertices.emplace_back();
			vertex.Position = Vector3( 0.0f, y, 0.0f );
			vertex.Normal = Vector3( 0.0f, 1.0f, 0.0f );
			vertex.UV = Vector2( 0.5f, 0.5f );
		}

		for ( int i = 0; i < a_Slices; ++i )
		{
			Vector3& u = unitCircleVertices[i];

			Vertex& vertex = vertices.emplace_back();
			vertex.Position = { u.x * a_TopRadius, y, u.z * a_TopRadius };
			vertex.Normal = { 0.0f, 1.0f, 0.0f };
			vertex.UV = { u.x * 0.5f + 0.5f, -u.z * 0.5f + 0.5f };
		}

		// put indices for sides
		uint32_t k1, k2;
		for ( int i = 0; i < a_Stacks; ++i )
		{
			k1 = i * ( a_Slices + 1 );     // beginning of current stack
			k2 = k1 + a_Slices + 1;      // beginning of next stack

			for ( int j = 0; j < a_Slices; ++j, ++k1, ++k2 )
			{
				// Invert the winding order
				indices.push_back( k1 );
				indices.push_back( k2 );
				indices.push_back( k1 + 1 );

				indices.push_back( k2 );
				indices.push_back( k2 + 1 );
				indices.push_back( k1 + 1 );
			}
		}

		// put indices for base
		for ( int i = 0, k = baseVertexIndex + 1; i < a_Slices; ++i, ++k )
		{
			if ( i < ( a_Slices - 1 ) )
			{
				indices.push_back( baseVertexIndex );
				indices.push_back( k );
				indices.push_back( k + 1 );
			}
			else    // last triangle
			{
				indices.push_back( baseVertexIndex );
				indices.push_back( k );
				indices.push_back( baseVertexIndex + 1 );
			}
		}

		// put indices for top
		for ( int i = 0, k = topVertexIndex + 1; i < a_Slices; ++i, ++k )
		{
			if ( i < ( a_Slices - 1 ) )
			{
				indices.push_back( topVertexIndex );
				indices.push_back( k + 1 );
				indices.push_back( k );
			}
			else
			{
				indices.push_back( topVertexIndex );
				indices.push_back( topVertexIndex + 1 );
				indices.push_back( k );
			}
		}

		CalculateTangents( vertices, indices );

		SharedPtr<MeshSource> meshSource = MakeShared<MeshSource>( vertices, indices, Matrix4( 1.0 ) );
		return meshSource;
	}

	SharedPtr<MeshSource> MeshFactory::CreateCapsule( float a_Radius, float a_Height, uint32_t a_Stacks, uint32_t a_Slices, uint32_t a_SphereStacks, uint32_t a_SphereSlices )
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		vertices.reserve( ( a_Stacks + 1 ) * ( a_Slices + 1 ) + ( a_SphereStacks + 1 ) * ( a_SphereSlices + 1 ) * 2 );
		indices.reserve( a_Stacks * a_Slices * 6 + a_SphereStacks * a_SphereSlices * 6 * 2 );

		float halfHeight = a_Height / 2.0f;

		// --- Top Hemisphere ---
		for ( uint32_t i = 0; i <= a_SphereStacks / 2; ++i ) {
			float V = i / (float)( a_SphereStacks / 2 );
			float phi = V * glm::half_pi<float>();

			for ( uint32_t j = 0; j <= a_SphereSlices; ++j ) {
				float U = j / (float)a_SphereSlices;
				float theta = U * ( glm::pi<float>() * 2 );

				float x = cosf( theta ) * sinf( phi );
				float y = sinf( phi );
				float z = sinf( theta ) * sinf( phi );

				Vertex vertex;
				vertex.Position = { x * a_Radius, halfHeight + cosf( phi ) * a_Radius, z * a_Radius };
				vertex.Normal = { x, cosf( phi ), z };
				vertex.UV = { U, V * 0.5f };
				vertices.push_back( vertex );
			}
		}

		// --- Cylinder Section ---
		for ( uint32_t i = 0; i <= a_Stacks; ++i ) {
			float V = i / (float)a_Stacks;
			float y = halfHeight - ( V * a_Height );

			for ( uint32_t j = 0; j <= a_Slices; ++j ) {
				float U = j / (float)a_Slices;
				float theta = U * ( glm::pi<float>() * 2 );

				float x = cosf( theta );
				float z = sinf( theta );

				Vertex vertex;
				vertex.Position = { x * a_Radius, y, z * a_Radius };
				vertex.Normal = { x, 0.0f, z };
				vertex.UV = { U, 0.5f + V * 0.5f };
				vertices.push_back( vertex );
			}
		}

		// --- Bottom Hemisphere ---
		for ( uint32_t i = 0; i <= a_SphereStacks / 2; ++i ) {
			float V = i / (float)( a_SphereStacks / 2 );
			float phi = glm::half_pi<float>() + V * glm::half_pi<float>();

			for ( uint32_t j = 0; j <= a_SphereSlices; ++j ) {
				float U = j / (float)a_SphereSlices;
				float theta = U * ( glm::pi<float>() * 2 );

				float x = cosf( theta ) * sinf( phi );
				float y = sinf( phi );
				float z = sinf( theta ) * sinf( phi );

				Vertex vertex;
				vertex.Position = { x * a_Radius, -halfHeight + cosf( phi ) * a_Radius, z * a_Radius };
				vertex.Normal = { x, cosf( phi ), z };
				vertex.UV = { U, 0.5f + V * 0.5f };
				vertices.push_back( vertex );
			}
		}

		// Generate indices with flipped winding order
		// --- Top Hemisphere ---
		for ( uint32_t i = 0; i < a_SphereStacks / 2; ++i ) {
			for ( uint32_t j = 0; j < a_SphereSlices; ++j ) {
				uint32_t first = i * ( a_SphereSlices + 1 ) + j;
				uint32_t second = first + a_SphereSlices + 1;

				indices.push_back( second );
				indices.push_back( first );
				indices.push_back( first + 1 );

				indices.push_back( second + 1 );
				indices.push_back( second );
				indices.push_back( first + 1 );
			}
		}

		// --- Cylinder Section ---
		uint32_t baseIndex = ( a_SphereStacks / 2 + 1 ) * ( a_SphereSlices + 1 );
		for ( uint32_t i = 0; i < a_Stacks; ++i ) {
			for ( uint32_t j = 0; j < a_Slices; ++j ) {
				uint32_t first = baseIndex + i * ( a_Slices + 1 ) + j;
				uint32_t second = first + a_Slices + 1;

				indices.push_back( second );
				indices.push_back( first );
				indices.push_back( first + 1 );

				indices.push_back( second + 1 );
				indices.push_back( second );
				indices.push_back( first + 1 );
			}
		}

		// --- Bottom Hemisphere ---
		baseIndex += ( a_Stacks + 1 ) * ( a_Slices + 1 );
		for ( uint32_t i = 0; i < a_SphereStacks / 2; ++i ) {
			for ( uint32_t j = 0; j < a_SphereSlices; ++j ) {
				uint32_t first = baseIndex + i * ( a_SphereSlices + 1 ) + j;
				uint32_t second = first + a_SphereSlices + 1;

				indices.push_back( second );
				indices.push_back( first );
				indices.push_back( first + 1 );

				indices.push_back( second + 1 );
				indices.push_back( second );
				indices.push_back( first + 1 );
			}
		}

		CalculateTangents( vertices, indices );

		SharedPtr<MeshSource> meshSource = MakeShared<MeshSource>( vertices, indices, Matrix4( 1.0f ) );
		return meshSource;
	}



	SharedPtr<MeshSource> MeshFactory::CreateTorus( float a_Radius, float a_Radius2, uint32_t a_Stacks, uint32_t a_Slices )
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		// Create Vertices
		for ( uint32_t i = 0; i <= a_Stacks; i++ )
		{
			float V = i / (float)a_Stacks;
			float phi = V * glm::pi<float>() * 2;

			// Center of the torus ring for this stack
			float cx = a_Radius * cosf( phi );
			float cy = a_Radius * sinf( phi );
			float cz = 0.0f;

			for ( uint32_t j = 0; j <= a_Slices; j++ )
			{
				float U = j / (float)a_Slices;
				float theta = U * glm::pi<float>() * 2;

				float x = ( a_Radius + a_Radius2 * cosf( theta ) ) * cosf( phi );
				float y = ( a_Radius + a_Radius2 * cosf( theta ) ) * sinf( phi );
				float z = a_Radius2 * sinf( theta );

				Vertex vertex;
				vertex.Position = { x, y, z };

				// Calculate the normal as the vector from the center of the ring to the vertex
				glm::vec3 center = { cx, cy, cz };
				glm::vec3 normal = glm::normalize( glm::vec3( x, y, z ) - center );
				vertex.Normal = normal;

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
		static AssetHandle s_StaticMeshHandle = ( s_StaticMesh->SetHandle( AssetManager::GetNextMemoryAssetHandle() ), s_StaticMesh->GetHandle() );
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	AssetHandle MeshFactory::GetDefaultCube()
	{
		static SharedPtr<MeshSource> s_CubeSource = CreateCube();
		static AssetHandle s_CubeSourceHandle = AssetHandle::Create();
		static bool s_CubeSourceInit = AssetManager::AddMemoryOnlyAsset( s_CubeSourceHandle, s_CubeSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_CubeSourceHandle );
		static AssetHandle s_StaticMeshHandle = ( s_StaticMesh->SetHandle( AssetManager::GetNextMemoryAssetHandle() ), s_StaticMesh->GetHandle() );
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	AssetHandle MeshFactory::GetDefaultSphere()
	{
		static SharedPtr<MeshSource> s_SphereSource = CreateSphere();
		static AssetHandle s_SphereSourceHandle = AssetHandle::Create();
		static bool s_SphereSourceInit = AssetManager::AddMemoryOnlyAsset( s_SphereSourceHandle, s_SphereSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_SphereSourceHandle );
		static AssetHandle s_StaticMeshHandle = ( s_StaticMesh->SetHandle( AssetManager::GetNextMemoryAssetHandle() ), s_StaticMesh->GetHandle() );
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	AssetHandle MeshFactory::GetDefaultCylinder()
	{
		static SharedPtr<MeshSource> s_CylinderSource = CreateCylinder();
		static AssetHandle s_CylinderSourceHandle = AssetHandle::Create();
		static bool s_CylinderSourceInit = AssetManager::AddMemoryOnlyAsset( s_CylinderSourceHandle, s_CylinderSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_CylinderSourceHandle );
		static AssetHandle s_StaticMeshHandle = ( s_StaticMesh->SetHandle( AssetManager::GetNextMemoryAssetHandle() ), s_StaticMesh->GetHandle() );
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	AssetHandle MeshFactory::GetDefaultCapsule()
	{
		static SharedPtr<MeshSource> s_CapsuleSource = CreateCapsule();
		static AssetHandle s_CapsuleSourceHandle = AssetHandle::Create();
		static bool s_CapsuleSourceInit = AssetManager::AddMemoryOnlyAsset( s_CapsuleSourceHandle, s_CapsuleSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_CapsuleSourceHandle );
		static AssetHandle s_StaticMeshHandle = ( s_StaticMesh->SetHandle( AssetManager::GetNextMemoryAssetHandle() ), s_StaticMesh->GetHandle() );
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	AssetHandle MeshFactory::GetDefaultCone()
	{
		static SharedPtr<MeshSource> s_ConeSource = CreateCone();
		static AssetHandle s_ConeSourceHandle = AssetHandle::Create();
		static bool s_ConeSourceInit = AssetManager::AddMemoryOnlyAsset( s_ConeSourceHandle, s_ConeSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_ConeSourceHandle );
		static AssetHandle s_StaticMeshHandle = ( s_StaticMesh->SetHandle( AssetManager::GetNextMemoryAssetHandle() ), s_StaticMesh->GetHandle() );
		static bool s_StaticMeshInit = AssetManager::AddMemoryOnlyAsset( s_StaticMeshHandle, s_StaticMesh );

		return s_StaticMeshHandle;
	}

	AssetHandle MeshFactory::GetDefaultTorus()
	{
		static SharedPtr<MeshSource> s_TorusSource = CreateTorus();
		static AssetHandle s_TorusSourceHandle = AssetHandle::Create();
		static bool s_TorusSourceInit = AssetManager::AddMemoryOnlyAsset( s_TorusSourceHandle, s_TorusSource );

		static SharedPtr<StaticMesh> s_StaticMesh = MakeShared<StaticMesh>( s_TorusSourceHandle );
		static AssetHandle s_StaticMeshHandle = ( s_StaticMesh->SetHandle( AssetManager::GetNextMemoryAssetHandle() ), s_StaticMesh->GetHandle() );
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