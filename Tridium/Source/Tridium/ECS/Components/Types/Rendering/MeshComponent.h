#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Mesh.h>

namespace Tridium {

	class Shader;

	DEFINE_COMPONENT( MeshComponent )
	{
	public:
		MeshComponent();
		MeshComponent(const MeshHandle& meshHandle);
		~MeshComponent() = default;

		inline MeshHandle GetMesh() { return m_Mesh; }
		inline MaterialHandle GetMaterial() { return m_Material; }

		void SetMesh( const MeshHandle& meshHandle );
		void SetMaterial( const MaterialHandle& materialHandle );

	private:
		MeshHandle m_Mesh = {};
		MaterialHandle m_Material;
	};

}