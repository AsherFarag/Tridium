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

	BEGIN_REFLECT_COMPONENT( CameraComponent )
		BASE( Component )
		PROPERTY( SceneCamera, FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( CameraComponent )

}