#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Camera.h>

namespace Tridium {

	DEFINE_COMPONENT( CameraComponent )
	{
		REFLECT;
	public:
		CameraComponent() = default;
		~CameraComponent() = default;

		Matrix4 GetView();

		Camera SceneCamera;
	};

}