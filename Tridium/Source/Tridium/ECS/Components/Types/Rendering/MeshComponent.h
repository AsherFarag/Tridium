#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Material.h>
#include <Tridium/Rendering/Mesh.h>

namespace Tridium {

	class StaticMeshComponent : public Component
	{
		REFLECT( StaticMeshComponent );
	public:
		StaticMeshComponent();
		~StaticMeshComponent() = default;

		StaticMeshHandle Mesh;
		std::vector<MaterialHandle> Materials;
		bool CastShadows = true;
	};

}