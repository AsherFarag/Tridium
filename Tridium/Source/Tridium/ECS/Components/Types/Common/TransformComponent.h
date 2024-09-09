#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	DEFINE_COMPONENT( TransformComponent )
	{
		REFLECT;
		friend class SceneSerializer;
	public:
		TransformComponent() = default;
		TransformComponent( const Vector3& a_Position );
		virtual void OnDestroy() override;

		operator Matrix4() { return GetWorldTransform(); }
		operator const Matrix4() const { return GetWorldTransform(); }

		Matrix4 GetWorldTransform() const; /* Returns the transform matrix in world space. */
		Matrix4 GetLocalTransform() const; /* Returns the transform matrix in local space. */
		Vector3 GetForward() const;
		Quaternion GetOrientation() const;

		bool HasParent() const { return m_Parent.IsValid(); }
		GameObject GetParent() const { return m_Parent; }
		void AttachToParent( GameObject a_Parent );
		void DetachFromParent();
		void AttachChild( GameObject a_Child );
		void DetachChild( GameObject a_Child );
		GameObject GetChild( const std::string& a_Tag ) const; /* Slow operation, avoid if possible. */
		std::vector<GameObject>& GetChildren() { return m_Children; }

	public:
		Vector3 Position = Vector3( 0.0f );
		Vector3 Rotation = Vector3( 0.0f );
		Vector3 Scale = Vector3( 1.0f );

	private:
		void SetParent() { m_Parent = GameObject(); }
		void SetParent( const GameObject a_Parent ) { m_Parent = a_Parent; }
		void RemoveChild( GameObject a_Child );

	private:
		// - Heirarchy - 
		GameObject m_Parent; // The gameobject this is a child of.
		std::vector<GameObject> m_Children;
	};
}