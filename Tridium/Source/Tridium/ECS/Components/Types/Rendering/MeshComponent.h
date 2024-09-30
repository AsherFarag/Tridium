#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Material.h>
#include <Tridium/Rendering/Mesh.h>

namespace Tridium {

	DEFINE_COMPONENT( MeshComponent )
	{
		REFLECT;
	public:
		MeshComponent() = default;
		MeshComponent(const AssetRef<Mesh>& mesh);
		~MeshComponent() = default;

		const AssetRef<Mesh>& GetMesh() const { return m_Mesh; }
		const AssetRef<Material>& GetMaterial() { return m_Material; }

		void SetMesh( const AssetRef<Mesh>& a_Mesh ) { m_Mesh = a_Mesh; }
		void SetMaterial( const AssetRef<Material>& a_Material ) { m_Material = a_Material; }

	private:
		AssetRef<Mesh> m_Mesh;
		AssetRef<Material> m_Material;
	};

}