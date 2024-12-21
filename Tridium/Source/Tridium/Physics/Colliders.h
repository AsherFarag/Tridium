#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Math/AABB.h>

namespace Tridium {

	class MeshCollider
	{
	public:
		static SharedPtr<MeshCollider> Create();
		static SharedPtr<MeshCollider> Create( const std::vector<Vector3>& a_Vertices, const std::vector<uint32_t>& a_Indices, const AABB& a_BoundingBox );
		virtual ~MeshCollider() = default;

		virtual bool IsValid() const = 0;
		virtual void Generate( const std::vector<Vector3>& a_Vertices, const std::vector<uint32_t>& a_Indices, const AABB& a_BoundingBox ) = 0;
	};

}