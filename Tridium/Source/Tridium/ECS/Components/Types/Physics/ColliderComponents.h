#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	class SphereColliderComponent : public Component
	{
		REFLECT( SphereColliderComponent )
	public:
		SphereColliderComponent();

		void SetRadius( float a_Radius ) { m_Radius = a_Radius; }
		float GetRadius() const { return m_Radius; }

		void SetCenter( const Vector3& a_Center ) { m_Center = a_Center; }
		const Vector3& GetCenter() const { return m_Center; }

		void SetRotation( const Rotator& a_Rotation ) { m_Rotation = a_Rotation; }
		const Rotator& GetRotation() const { return m_Rotation; }

	protected:
		Vector3 m_Center = { 0.0f, 0.0f, 0.0f };
		Rotator m_Rotation = { 0.0f, 0.0f, 0.0f };
		float m_Radius = 0.5f;
	};

	class BoxColliderComponent : public Component
	{
		REFLECT( BoxColliderComponent )
	public:
		BoxColliderComponent();

		void SetHalfExtents( const Vector3& a_HalfExtents ) { m_HalfExtents = a_HalfExtents; }
		const Vector3& GetHalfExtents() const { return m_HalfExtents; }

		void SetCenter( const Vector3& a_Center ) { m_Center = a_Center; }
		const Vector3& GetCenter() const { return m_Center; }

		void SetRotation( const Rotator& a_Rotation ) { m_Rotation = a_Rotation; }
		const Rotator& GetRotation() const { return m_Rotation; }

	protected:
		Vector3 m_Center = { 0.0f, 0.0f, 0.0f };
		Rotator m_Rotation = { 0.0f, 0.0f, 0.0f };
		Vector3 m_HalfExtents = { 0.5f, 0.5f, 0.5f };
	};

	class CapsuleColliderComponent : public Component
	{
		REFLECT( CapsuleColliderComponent )
	public:
		CapsuleColliderComponent();

		void SetRadius( float a_Radius ) { m_Radius = a_Radius; }
		float GetRadius() const { return m_Radius; }

		void SetHalfHeight( float a_HalfHeight ) { m_HalfHeight = a_HalfHeight; }
		float GetHalfHeight() const { return m_HalfHeight; }

		void SetCenter( const Vector3& a_Center ) { m_Center = a_Center; }
		const Vector3& GetCenter() const { return m_Center; }

		void SetRotation( const Rotator& a_Rotation ) { m_Rotation = a_Rotation; }
		const Rotator& GetRotation() const { return m_Rotation; }

	protected:
		Vector3 m_Center = { 0.0f, 0.0f, 0.0f };
		Rotator m_Rotation = { 0.0f, 0.0f, 0.0f };
		float m_Radius = 0.5f;
		float m_HalfHeight = 0.5f;
	};

	class CylinderColliderComponent : public Component
	{
		REFLECT( CylinderColliderComponent )
	public:
		CylinderColliderComponent();

		void SetRadius( float a_Radius ) { m_Radius = a_Radius; }
		float GetRadius() const { return m_Radius; }

		void SetHalfHeight( float a_HalfHeight ) { m_HalfHeight = a_HalfHeight; }
		float GetHalfHeight() const { return m_HalfHeight; }

		void SetCenter( const Vector3& a_Center ) { m_Center = a_Center; }
		const Vector3& GetCenter() const { return m_Center; }

		void SetRotation( const Rotator& a_Rotation ) { m_Rotation = a_Rotation; }
		const Rotator& GetRotation() const { return m_Rotation; }

	protected:
		Vector3 m_Center = { 0.0f, 0.0f, 0.0f };
		Rotator m_Rotation = { 0.0f, 0.0f, 0.0f };
		float m_Radius = 0.5f;
		float m_HalfHeight = 0.5f;
	};

	class MeshColliderComponent : public Component
	{
		REFLECT( MeshColliderComponent )
	public:
		MeshColliderComponent();

		void SetMesh( StaticMeshHandle a_Mesh ) { m_Mesh = a_Mesh; }
		StaticMeshHandle GetMesh() const { return m_Mesh; }

	protected:
		StaticMeshHandle m_Mesh;
	};
}