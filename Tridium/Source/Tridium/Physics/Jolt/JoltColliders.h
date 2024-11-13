#pragma once
#include <Tridium/Physics/Colliders.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

namespace Tridium {

	class JoltMeshCollider final : public MeshCollider
	{
	public:
		JoltMeshCollider() = default;
		JoltMeshCollider( const std::vector<Vector3> a_Vertices, const std::vector<uint32_t>& a_Indices, const AABB& a_BoundingBox );
		virtual ~JoltMeshCollider() = default;

		virtual bool IsValid() const override { return m_IsValid; }
		virtual void Generate( const std::vector<Vector3>& a_Vertices, const std::vector<uint32_t>& a_Indices, const AABB& a_BoundingBox ) override;

		JPH::RefConst<JPH::MeshShapeSettings> GetMeshShapeSettings() const { return m_MeshShapeSettings; }
		JPH::RefConst<JPH::MeshShape> GetMeshShape() const { return m_MeshShape; }
		JPH::MassProperties GetMassProperties() const { return m_MassProperties; }

	private:
		bool m_IsValid = false;
		JPH::Ref<JPH::MeshShapeSettings> m_MeshShapeSettings;
		JPH::Ref<JPH::MeshShape> m_MeshShape;
		JPH::MassProperties m_MassProperties;
	};
}