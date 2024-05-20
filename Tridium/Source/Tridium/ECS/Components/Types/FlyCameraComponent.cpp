#include "tripch.h"
#include "FlyCameraComponent.h"
#include <Tridium/Core/Core.h>
#include <Tridium/ECS/Components/Types/TransformComponent.h>

namespace Tridium {

	void FlyCameraComponent::OnConstruct()
	{
		m_Transform = &GetGameObject().GetTransform();
	}

	void FlyCameraComponent::OnDestroy()
	{
	}

	void FlyCameraComponent::OnUpdate()
	{
		if ( !m_Transform )
			return;
		float dt = Time::DeltaTime();
		auto forward = m_Transform->GetForward();
		auto right = glm::cross( m_Transform->GetForward(), Vector3( 0, 1, 0 ) );

		if ( Input::IsKeyPressed( Input::KEY_W ) )
		{
			m_Transform->Translation += forward * Speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_S ) )
		{
			m_Transform->Translation -= forward * Speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_A ) )
		{
			m_Transform->Translation -= right * Speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_D ) )
		{
			m_Transform->Translation += right * Speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_UP ) )
		{
			m_Transform->Translation.y += Speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_DOWN ) )
		{
			m_Transform->Translation.y -= Speed * dt;
		}

		// Rotation

		if ( Input::IsKeyPressed( Input::KEY_RIGHT ) )
		{
			m_Transform->Rotation.y += 0.5f * Speed * dt;
		}
		if ( Input::IsKeyPressed( Input::KEY_LEFT ) )
		{
			m_Transform->Rotation.y -= 0.5f * Speed * dt;
		}

		if ( Input::IsMouseButtonPressed( Input::MOUSE_BUTTON_RIGHT ) )
		{
			float MouseDeltaX = Input::GetMouseX() - m_LastMousePos.x;
			float MouseDeltaY = Input::GetMouseY() - m_LastMousePos.y;

			if ( MouseDeltaX != 0.f || MouseDeltaY != 0.f )
			{
				float PitchDelta = glm::radians(MouseDeltaY * Sensitivity);
				float YawDelta = glm::radians( MouseDeltaX * Sensitivity);

				Quaternion Quat = glm::normalize( glm::cross( glm::angleAxis( PitchDelta, Vector3( 0, 0, 1 ) ), glm::angleAxis( YawDelta, Vector3(0,1,0) ) ) );
				m_Transform->Rotation += glm::eulerAngles(Quat);

			}
		}

		m_LastMousePos = Input::GetMousePosition();
	}

}