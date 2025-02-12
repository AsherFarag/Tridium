#pragma once
#include <Tridium/ECS/Components/NativeScriptComponent.h>

namespace Tridium {
	
	class TransformComponent;

	class CameraControllerComponent : public NativeScriptComponent
	{
		REFLECT( CameraControllerComponent );
	public:
		CameraControllerComponent();
		void OnUpdate( float a_DeltaTime );

	public:
		float Speed = 1.0f;
		float LookSensitivity = 0.1f;

	private:
		Vector2 m_LastMousePos;
		int m_LastMouseScroll = 0;
	};
}