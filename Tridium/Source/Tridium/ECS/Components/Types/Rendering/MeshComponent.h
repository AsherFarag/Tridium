#pragma once
#include <Tridium/ECS/Components/NativeScriptComponent.h>
#include <Tridium/Graphics/oldRendering/Material.h>
#include <Tridium/Graphics/oldRendering/Mesh.h>

namespace Tridium {

	class OldStaticMeshComponent : public NativeScriptComponent
	{
		REFLECT( OldStaticMeshComponent );
	public:
		OldStaticMeshComponent();
		~OldStaticMeshComponent() = default;

		StaticMeshHandle Mesh;
		std::vector<MaterialHandle> Materials;
		bool CastShadows = true;
	};

}