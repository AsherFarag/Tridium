#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {
	
	class TransformComponent;

	class CameraControllerComponent : public ScriptableComponent
	{
		REFLECT( CameraControllerComponent );
	public:
		CameraControllerComponent();
		virtual void OnUpdate() override;

	public:
		float Speed = 1.0f;
		float LookSensitivity = 0.1f;

	private:
		Vector2 m_LastMousePos;
		int m_LastMouseScroll = 0;
	};
}