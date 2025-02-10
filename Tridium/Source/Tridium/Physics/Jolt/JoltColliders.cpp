#include "tripch.h"
#include "JoltColliders.h"
#include "JoltUtil.h"

namespace Tridium {

	JoltMeshCollider::JoltMeshCollider( const std::vector<Vector3> a_Vertices, const std::vector<uint32_t>& a_Indices, const AABB& a_BoundingBox )
	{
		Generate( a_Vertices, a_Indices, a_BoundingBox );
	}

	void JoltMeshCollider::Generate( const std::vector<Vector3>& a_Vertices, const std::vector<uint32_t>& a_Indices, const AABB& a_BoundingBox )
	{
		// Convert the vertices to Jolt's format
		JPH::VertexList vertices;
		vertices.reserve( a_Vertices.size() );
		for ( const Vector3& vertex : a_Vertices )
		{
			vertices.emplace_back( vertex.x, vertex.y, vertex.z );
		}

		// Convert the indices to Jolt's format
		JPH::IndexedTriangleList triangles;
		triangles.reserve( a_Indices.size() / 3 );
		for ( size_t i = 0; i < a_Indices.size(); i += 3 )
		{
			triangles.emplace_back( a_Indices[i], a_Indices[i + 1], a_Indices[i + 2] );
		}

		m_MeshShapeSettings = new JPH::MeshShapeSettings( vertices, triangles );

		JPH::Shape::ShapeResult result;
		m_MeshShape = new JPH::MeshShape( *m_MeshShapeSettings, result );

		if ( result.HasError() )
		{
			LOG( LogCategory::Physics, Error, "Failed to create mesh collider: {}", result.GetError().c_str() );
			m_IsValid = false;
			m_MeshShape = nullptr;
			m_MeshShapeSettings = nullptr;
			return;
		}

		m_IsValid = true;

		// Calculate mass
		JPH::Vec3 size = Util::ToJoltVec3( a_BoundingBox.GetSize() );
		m_MassProperties.SetMassAndInertiaOfSolidBox( size, 1000.0f ); // 1000 kg/m^3
	}

}
