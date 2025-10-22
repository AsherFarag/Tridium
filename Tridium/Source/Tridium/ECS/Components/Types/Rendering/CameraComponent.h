#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Graphics/oldRendering/Camera.h>

namespace Tridium {

	class OldCameraComponent : public Component
	{
		REFLECT(OldCameraComponent);
	public:
		OldCameraComponent() = default;
		~OldCameraComponent();

		Matrix4 GetView();

		Camera SceneCamera;
		float Pitch = 0.0f; // In Degrees
		float Yaw = 0.0f; // In Degrees
		bool IsMainCamera = false;
	};


}