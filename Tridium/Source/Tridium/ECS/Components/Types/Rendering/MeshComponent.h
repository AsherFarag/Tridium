#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Material.h>
#include <Tridium/Rendering/Mesh.h>

namespace Tridium {

	class MeshComponent : public Component
	{
		REFLECT( MeshComponent );
	public:
		MeshComponent();
		~MeshComponent() = default;

		AssetHandle GetMesh() const { return m_Mesh; }
		AssetHandle GetMaterial() { return m_Material; }

		void SetMesh( AssetHandle a_Mesh ) { m_Mesh = a_Mesh; }
		void SetMaterial( AssetHandle a_Material ) { m_Material = a_Material; }

	private:
		AssetHandle m_Mesh;
		AssetHandle m_Material;
	};

}