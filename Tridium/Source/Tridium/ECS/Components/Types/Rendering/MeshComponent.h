#pragma once
#include <Tridium/ECS/Components/NativeScriptComponent.h>
#include <Tridium/Graphics/Rendering/Material.h>
#include <Tridium/Graphics/Rendering/Mesh.h>

namespace Tridium {

	class StaticMeshComponent : public NativeScriptComponent
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