#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Graphics/Rendering/Camera.h>

namespace Tridium {

	class CameraComponent : public Component
	{
		REFLECT(CameraComponent);
	public:
		CameraComponent() = default;
		~CameraComponent();

		Matrix4 GetView();

		Camera SceneCamera;
		float Pitch = 0.0f; // In Degrees
		float Yaw = 0.0f; // In Degrees
		bool IsMainCamera = false;
	};


}