#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/oldAsset/Asset.h>

namespace Tridium {

	class OldSphereColliderComponent : public Component
	{
		REFLECT( OldSphereColliderComponent )
	public:
		OldSphereColliderComponent();

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

	class OldBoxColliderComponent : public Component
	{
		REFLECT( OldBoxColliderComponent )
	public:
		OldBoxColliderComponent();

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

	class OldCapsuleColliderComponent : public Component
	{
		REFLECT( OldCapsuleColliderComponent )
	public:
		OldCapsuleColliderComponent();

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

	class OldCylinderColliderComponent : public Component
	{
		REFLECT( OldCylinderColliderComponent )
	public:
		OldCylinderColliderComponent();

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

	class OldMeshColliderComponent : public Component
	{
		REFLECT( OldMeshColliderComponent )
	public:
		OldMeshColliderComponent();

		void SetMesh( StaticMeshHandle a_Mesh ) { m_Mesh = a_Mesh; }
		StaticMeshHandle GetMesh() const { return m_Mesh; }

	protected:
		StaticMeshHandle m_Mesh;
	};
}