#include "tripch.h"
#include "ColliderComponents.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {
	BEGIN_REFLECT_COMPONENT( OldSphereColliderComponent, Scriptable )
		PROPERTY( m_Center, EditAnywhere | Serialize )
		PROPERTY( m_Rotation, EditAnywhere | Serialize )
		PROPERTY( m_Radius, EditAnywhere | Serialize )
	END_REFLECT( OldSphereColliderComponent )

	BEGIN_REFLECT_COMPONENT( OldBoxColliderComponent, Scriptable )
		PROPERTY( m_Center, EditAnywhere | Serialize )
		PROPERTY( m_Rotation, EditAnywhere | Serialize )
		PROPERTY( m_HalfExtents, EditAnywhere | Serialize )
	END_REFLECT_COMPONENT( OldBoxColliderComponent )

	BEGIN_REFLECT_COMPONENT( OldCapsuleColliderComponent, Scriptable )
		PROPERTY( m_Center, EditAnywhere | Serialize )
		PROPERTY( m_Rotation, EditAnywhere | Serialize )
		PROPERTY( m_Radius, EditAnywhere | Serialize )
		PROPERTY( m_HalfHeight, EditAnywhere | Serialize )
	END_REFLECT_COMPONENT( OldCapsuleColliderComponent )

	BEGIN_REFLECT_COMPONENT( OldCylinderColliderComponent, Scriptable )
		PROPERTY( m_Center, EditAnywhere | Serialize )
		PROPERTY( m_Rotation, EditAnywhere | Serialize )
		PROPERTY( m_Radius, EditAnywhere | Serialize )
		PROPERTY( m_HalfHeight, EditAnywhere | Serialize )
	END_REFLECT_COMPONENT( OldCylinderColliderComponent )

	BEGIN_REFLECT_COMPONENT( OldMeshColliderComponent, Scriptable )
		PROPERTY( m_Mesh, EditAnywhere | Serialize )
	END_REFLECT_COMPONENT( OldMeshColliderComponent )

	OldSphereColliderComponent::OldSphereColliderComponent()
	{
	}

	OldBoxColliderComponent::OldBoxColliderComponent()
	{
	}

	OldCapsuleColliderComponent::OldCapsuleColliderComponent()
	{
	}

	OldCylinderColliderComponent::OldCylinderColliderComponent()
	{
	}

	OldMeshColliderComponent::OldMeshColliderComponent()
	{
	}
}