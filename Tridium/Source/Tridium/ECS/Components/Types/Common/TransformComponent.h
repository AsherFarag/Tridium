#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Math/Rotator.h>

namespace Tridium {

	class Scene;

	class TransformComponent : public Component
	{
		REFLECT(TransformComponent);
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
		Vector3 GetWorldPosition() const { return GetWorldTransform()[3]; }
		Vector3 GetLocalPosition() const { return Position; }
		Vector3 GetWorldScale() const;
		Vector3 GetLocalScale() const { return Scale; }

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
		Rotator Rotation = Rotator( Vector3( 0.0f ) );
		Vector3 Scale = Vector3( 1.0f );

	private:
		void SetParent( GameObject a_Parent = GameObject() ) { m_Parent = a_Parent; }
		void RemoveChild( GameObject a_Child );

	private:
		// - Heirarchy - 
		GameObject m_Parent; // The gameobject this is a child of.
		std::vector<GameObject> m_Children;

		friend bool IO::DeserializeFromText<Scene>( const YAML::Node& a_Node, Scene& a_Data );
	};
}