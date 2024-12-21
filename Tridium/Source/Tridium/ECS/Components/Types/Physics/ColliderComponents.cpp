#include "tripch.h"
#include "ColliderComponents.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {
	BEGIN_REFLECT_COMPONENT( SphereColliderComponent, Scriptable )
		PROPERTY( m_Center, EditAnywhere | Serialize )
		PROPERTY( m_Rotation, EditAnywhere | Serialize )
		PROPERTY( m_Radius, EditAnywhere | Serialize )
	END_REFLECT( SphereColliderComponent )

	BEGIN_REFLECT_COMPONENT( BoxColliderComponent, Scriptable )
		PROPERTY( m_Center, EditAnywhere | Serialize )
		PROPERTY( m_Rotation, EditAnywhere | Serialize )
		PROPERTY( m_HalfExtents, EditAnywhere | Serialize )
	END_REFLECT_COMPONENT( BoxColliderComponent )

	BEGIN_REFLECT_COMPONENT( CapsuleColliderComponent, Scriptable )
		PROPERTY( m_Center, EditAnywhere | Serialize )
		PROPERTY( m_Rotation, EditAnywhere | Serialize )
		PROPERTY( m_Radius, EditAnywhere | Serialize )
		PROPERTY( m_HalfHeight, EditAnywhere | Serialize )
	END_REFLECT_COMPONENT( CapsuleColliderComponent )

	BEGIN_REFLECT_COMPONENT( CylinderColliderComponent, Scriptable )
		PROPERTY( m_Center, EditAnywhere | Serialize )
		PROPERTY( m_Rotation, EditAnywhere | Serialize )
		PROPERTY( m_Radius, EditAnywhere | Serialize )
		PROPERTY( m_HalfHeight, EditAnywhere | Serialize )
	END_REFLECT_COMPONENT( CylinderColliderComponent )

	BEGIN_REFLECT_COMPONENT( MeshColliderComponent, Scriptable )
		PROPERTY( m_Mesh, EditAnywhere | Serialize )
	END_REFLECT_COMPONENT( MeshColliderComponent )

	SphereColliderComponent::SphereColliderComponent()
	{
	}

	BoxColliderComponent::BoxColliderComponent()
	{
	}

	CapsuleColliderComponent::CapsuleColliderComponent()
	{
	}

	CylinderColliderComponent::CylinderColliderComponent()
	{
	}

	MeshColliderComponent::MeshColliderComponent()
	{
	}
}