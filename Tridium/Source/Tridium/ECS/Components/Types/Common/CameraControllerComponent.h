#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {
	
	class TransformComponent;

	DEFINE_COMPONENT( CameraControllerComponent, ScriptableComponent )
	{
		REFLECT;
	public:
		virtual void OnUpdate() override;

	public:
		float Speed = 1.0f;
		float LookSensitivity = 1.0f;

	private:
		Vector2 m_LastMousePos;
		int m_LastMouseScroll = 0;
	};

}