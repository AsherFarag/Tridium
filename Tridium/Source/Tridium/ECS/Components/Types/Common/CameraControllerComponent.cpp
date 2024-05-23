#include "tripch.h"
#include "CameraControllerComponent.h"
#include <Tridium/Input/Input.h>
#include "TransformComponent.h"

namespace Tridium {
	void CameraControllerComponent::OnUpdate()
	{
		auto& transform = GetGameObject().GetTransform();

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

		if ( Input::IsMouseButtonPressed( Input::MOUSE_BUTTON_RIGHT ) )
		{
			Vector2 mouseDelta = Input::GetMousePosition() - m_LastMousePos;
			float yawSign = up.y < 0 ? -1.0f : 1.0f;
			transform.Rotation.y += yawSign * glm::radians( mouseDelta.x ) * LookSensitivity;
			transform.Rotation.x += glm::radians( mouseDelta.y ) * LookSensitivity;
		}

		m_LastMousePos = Input::GetMousePosition();
	}
}