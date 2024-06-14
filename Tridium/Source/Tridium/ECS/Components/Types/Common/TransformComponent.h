#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	DEFINE_COMPONENT( TransformComponent )
	{
		REFLECT;

	public:

		TransformComponent() = default;
		TransformComponent( const TransformComponent& ) = default;
		TransformComponent( const Vector3& a_Position );

		operator Matrix4&() { return GetTransform(); }
		operator const Matrix4&() const { return GetTransform(); }

		Matrix4 GetTransform() const;
		Vector3 GetForward() const;
		Quaternion GetOrientation() const;

		Vector3 Position = Vector3( 0.0f );
		Vector3 Rotation = Vector3( 0.0f );
		Vector3 Scale = Vector3( 1.0f );

	private:
		// - Heirarchy - 
		TransformComponent* Parent = nullptr; // The gameobject this is a child of.
		TransformComponent* Owner = nullptr; // The gameobject that owns this, m_Owner has no parent gameobject.
		std::unordered_map<std::string, TransformComponent*> Children;
	};
}