#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	class Mesh;
	class Material;

	DEFINE_COMPONENT( MeshComponent )
	{
	public:
		MeshComponent() = default;
		MeshComponent(const Ref<Mesh>& mesh);
		~MeshComponent() = default;

		const Ref<Mesh>& GetMesh() const { return m_Mesh; }
		const Ref<Material>& GetMaterial() { return m_Material; }

		void SetMesh( const Ref<Mesh>& mesh ) { m_Mesh = mesh; }
		void SetMaterial( const Ref<Material>& material ) { m_Material = material; }

	private:
		Ref<Mesh> m_Mesh;
		Ref<Material> m_Material;
	};

}