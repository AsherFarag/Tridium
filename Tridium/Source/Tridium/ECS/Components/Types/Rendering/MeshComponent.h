#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	class Mesh;
	class Material;

	DEFINE_COMPONENT( MeshComponent )
	{
	public:
		MeshComponent() = default;
		MeshComponent(const SharedPtr<Mesh>& mesh);
		~MeshComponent() = default;

		const SharedPtr<Mesh>& GetMesh() const { return m_Mesh; }
		const SharedPtr<Material>& GetMaterial() { return m_Material; }

		void SetMesh( const SharedPtr<Mesh>& mesh ) { m_Mesh = mesh; }
		void SetMaterial( const SharedPtr<Material>& material ) { m_Material = material; }

	private:
		SharedPtr<Mesh> m_Mesh;
		SharedPtr<Material> m_Material;
	};

}