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
		auto up = GetUpDirection();
		auto forward = GetForwardDirection();
		auto right = GetRightDirection();

		if ( Input::IsKeyPressed( Input::KEY_W ) )
		{
			m_Transform->Position += forward * Speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_S ) )
		{
			m_Transform->Position -= forward * Speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_A ) )
		{
			m_Transform->Position -= right * Speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_D ) )
		{
			m_Transform->Position += right * Speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_UP ) )
		{
			m_Transform->Position.y += Speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_DOWN ) )
		{
			m_Transform->Position.y -= Speed * dt;
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

				Quaternion Quat = glm::normalize( glm::cross( glm::angleAxis( -PitchDelta, right ), glm::angleAxis( -YawDelta, up ) ) );
				m_Transform->Rotation += glm::eulerAngles(Quat);

			}
		}

		m_LastMousePos = Input::GetMousePosition();
	}
	Vector3 FlyCameraComponent::GetUpDirection() const
	{
		return glm::rotate( GetOrientation(), Vector3( 0.0f, 1.0f, 0.0f ) );
	}

	Vector3 FlyCameraComponent::GetRightDirection() const
	{
		return glm::rotate( GetOrientation(), Vector3( 1.0f, 0.0f, 0.0f ) );
	}

	Vector3 FlyCameraComponent::GetForwardDirection() const
	{
		return glm::rotate( GetOrientation(), Vector3( 0.0f, 0.0f, -1.0f ) );
	}

	Quaternion FlyCameraComponent::GetOrientation() const
	{
		return Quaternion( Vector3( m_Transform->Rotation.x, m_Transform->Rotation.y, 0.f ) );
	}

}