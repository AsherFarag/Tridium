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
		float LookSensitivity;

	private:
		Vector2 m_LastMousePos;
		int m_LastMouseScroll = 0;
	};

	BEGIN_REFLECT_COMPONENT( CameraControllerComponent )
		BASE( ScriptableComponent )
		PROPERTY( Speed, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( LookSensitivity, FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( CameraControllerComponent );
}