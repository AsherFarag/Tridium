#include "tripch.h"
#include "CameraControllerComponent.h"
#include <Tridium/Input/Input.h>
#include "TransformComponent.h"
#include <Tridium/ECS/Components/Types/Rendering/CameraComponent.h>
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

	CameraControllerComponent::CameraControllerComponent()
		: LookSensitivity( 0.75f ), Speed( 1.0f )
	{

	}

	void CameraControllerComponent::OnUpdate( float dt )
	{
		auto go = GetGameObject();
		if ( !go.HasComponent<TransformComponent>() )
			return;

		auto& transform = GetGameObject().GetComponent<TransformComponent>();

		constexpr Vector3 up( 0, 1, 0 );
		auto forward = transform.GetForward();
		auto right = glm::cross( forward, up );


		if ( Input::IsKeyPressed( Input::KEY_W ) )
			transform.Position -= Vector3(forward.x, 0, forward.z ) * Speed * dt;

		if ( Input::IsKeyPressed( Input::KEY_S ) )
			transform.Position += Vector3( forward.x, 0, forward.z ) * Speed * dt;

		if ( Input::IsKeyPressed( Input::KEY_A ) )
			transform.Position += right * Speed * dt;

		if ( Input::IsKeyPressed( Input::KEY_D ) )
			transform.Position -= right * Speed * dt;

		if ( Input::IsKeyPressed( Input::KEY_SPACE ) )
			transform.Position += up * Speed * dt;

		if ( Input::IsKeyPressed( Input::KEY_LEFT_SHIFT ) )
			transform.Position -= up * Speed * dt;

		// Mouse Scroll Zoom
		//if ( Input::GetMouseScrollYOffset() != m_LastMouseScroll )
		//{
		//	if ( CameraComponent* cam = go.TryGetComponent<CameraComponent>() )
		//	{
		//		float fov = glm::degrees( cam->SceneCamera.GetPerspectiveFOV() );
		//		fov -= ( Input::GetMouseScrollYOffset() - m_LastMouseScroll ) * 2;
		//		if ( fov < 35.f )
		//			fov = 35.f;
		//		else if ( fov > 150.f )
		//			fov = 150.f;

		//		cam->SceneCamera.SetPerspectiveFOV( glm::radians( fov ) );
		//	}
		//}

		// Mouse Rotation
		Vector2 mouseDelta = Input::GetMousePosition() - m_LastMousePos;
		float yawSign = up.y < 0 ? -1.0f : 1.0f;

		Vector3 euler = glm::degrees( transform.Rotation.Euler );

		euler.y -= yawSign * mouseDelta.x * LookSensitivity;
		euler.x -= mouseDelta.y * LookSensitivity;

		constexpr float clampZone = 89.f;
		if ( euler.x < -clampZone )
			euler.x = -clampZone;
		else if ( euler.x > clampZone )
			euler.x = clampZone;

		transform.Rotation.SetFromEuler( glm::radians( euler ) );

		m_LastMousePos = Input::GetMousePosition();
		m_LastMouseScroll = Input::GetMouseScrollYOffset();
	}

	BEGIN_REFLECT_COMPONENT( CameraControllerComponent )
		BASE( NativeScriptComponent )
		PROPERTY( Speed, Serialize | EditAnywhere )
		PROPERTY( LookSensitivity, Serialize | EditAnywhere )
	END_REFLECT_COMPONENT( CameraControllerComponent );
}