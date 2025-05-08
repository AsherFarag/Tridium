#include "tripch.h"
#include "Colliders.h"
#include "PhysicsAPI.h"
#include "Jolt/JoltColliders.h"

namespace Tridium {
	SharedPtr<MeshCollider> MeshCollider::Create()
	{
		switch ( s_PhysicsAPI )
		{
		case EPhysicsAPI::Jolt:
			return MakeShared<JoltMeshCollider>();
		}

		ASSERT( false, "Unknown physics API" );
		return nullptr;
	}

	SharedPtr<MeshCollider> MeshCollider::Create( const std::vector<Vector3>& a_Vertices, const std::vector<uint32_t>& a_Indices, const AABB& a_BoundingBox )
    {
		switch ( s_PhysicsAPI )
		{
		case EPhysicsAPI::Jolt:
			return MakeShared<JoltMeshCollider>( a_Vertices, a_Indices, a_BoundingBox );
		}

		ASSERT( false, "Unknown physics API" );
		return nullptr;
    }

}
