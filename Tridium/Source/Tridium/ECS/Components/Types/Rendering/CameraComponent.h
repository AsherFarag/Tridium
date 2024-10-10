#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Camera.h>

namespace Tridium {

	class CameraComponent : public Component
	{
		REFLECT(CameraComponent);
	public:
		CameraComponent() = default;
		~CameraComponent() = default;

		Matrix4 GetView();

		Camera SceneCamera;
	};

}