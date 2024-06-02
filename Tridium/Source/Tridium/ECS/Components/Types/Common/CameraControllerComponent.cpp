#include "tripch.h"
#include "CameraControllerComponent.h"
#include <Tridium/Input/Input.h>
#include "TransformComponent.h"
#include <Tridium/ECS/Components/Types/Rendering/CameraComponent.h>

namespace Tridium {

	void CameraControllerComponent::OnUpdate()
	{
		auto& go = GetGameObject();
		if ( !go.HasComponent<TransformComponent>() )
			return;

		auto& transform = GetGameObject().GetComponent<TransformComponent>();

		float dt = Time::DeltaTime();

		constexpr Vector3 up( 0, 1, 0 );
		auto forward = transform.GetForward();
		auto right = glm::cross( forward, up );


		if ( Input::IsKeyPressed( Input::KEY_W ) )
			transform.Position += Vector3(forward.x, 0, forward.z ) * Speed * dt;

		if ( Input::IsKeyPressed( Input::KEY_S ) )
			transform.Position -= Vector3( forward.x, 0, forward.z ) * Speed * dt;

		if ( Input::IsKeyPressed( Input::KEY_A ) )
			transform.Position -= right * Speed * dt;

		if ( Input::IsKeyPressed( Input::KEY_D ) )
			transform.Position += right * Speed * dt;

		// Mouse Scroll Zoom
		if ( Input::GetMouseScrollYOffset() != m_LastMouseScroll )
		{
			if ( CameraComponent* cam = go.TryGetComponent<CameraComponent>() )
			{
				float fov = glm::degrees( cam->SceneCamera.GetPerspectiveFOV() );
				fov -= ( Input::GetMouseScrollYOffset() - m_LastMouseScroll ) * 2;
				if ( fov < 35.f )
					fov = 35.f;
				else if ( fov > 150.f )
					fov = 150.f;

				cam->SceneCamera.SetPerspectiveFOV( glm::radians( fov ) );
			}
		}

		// Mouse Rotation
		Vector2 mouseDelta = Input::GetMousePosition() - m_LastMousePos;
		float yawSign = up.y < 0 ? -1.0f : 1.0f;
		transform.Rotation.y += yawSign * glm::radians( mouseDelta.x ) * LookSensitivity;
		transform.Rotation.x += glm::radians( mouseDelta.y ) * LookSensitivity;

		constexpr float clampZone = glm::radians( 89.f );
		if ( transform.Rotation.x < -clampZone )
			transform.Rotation.x = -clampZone;
		else if ( transform.Rotation.x > clampZone )
			transform.Rotation.x = clampZone;

		m_LastMousePos = Input::GetMousePosition();
		m_LastMouseScroll = Input::GetMouseScrollYOffset();
	}
}