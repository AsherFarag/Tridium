#include "tripch.h"
#ifdef IS_EDITOR

#include "EditorCamera.h"
#include <Tridium/Input/Input.h>
#include <Tridium/Core/Application.h>

namespace Tridium::Editor {

	void EditorCamera::OnUpdate()
	{
		RecalculateView();

		if ( Focused )
			HandleInput();

		m_LastMousePos = Input::GetMousePosition();
	}

	void EditorCamera::HandleInput()
	{
		float dt = Time::DeltaTime();
		auto up = GetUpDirection();
		auto forward = GetForwardDirection();
		auto right = GetRightDirection();

		bool shift = Input::IsKeyPressed( Input::KEY_LEFT_SHIFT );
		if ( shift != m_WasLeftShiftPressed )
		{
			if ( shift )
				Speed *= 5.f;
			else
				Speed /= 5.f;
		}
		m_WasLeftShiftPressed = shift;

		bool ctrl = Input::IsKeyPressed( Input::KEY_LEFT_CONTROL );
		if ( ctrl != m_WasLeftCtrlPressed )
		{
			if ( ctrl )
				Speed /= 5.f;
			else
				Speed *= 5.f;
		}
		m_WasLeftCtrlPressed = ctrl;

		MoveForward( Input::IsKeyPressed( Input::KEY_W ) - Input::IsKeyPressed( Input::KEY_S ) );
		MoveSideways( Input::IsKeyPressed( Input::KEY_D ) - Input::IsKeyPressed( Input::KEY_A ) );

		if ( Input::IsKeyPressed( Input::KEY_UP ) )
		{
			Position.y += Speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_DOWN ) )
		{
			Position.y -= Speed * dt;
		}

		// Rotation

		if ( Input::IsKeyPressed( Input::KEY_RIGHT ) )
		{
			Yaw += 0.5f * Speed * dt;
		}
		if ( Input::IsKeyPressed( Input::KEY_LEFT ) )
		{
			Yaw -= 0.5f * Speed * dt;
		}

		if ( Input::IsMouseButtonPressed( Input::MOUSE_BUTTON_RIGHT ) )
		{
			MouseRotate( Input::GetMousePosition() - m_LastMousePos );
		}
	}

	void EditorCamera::MoveForward( const float magnitude )
	{
		Position += magnitude * GetForwardDirection() * Speed * (float)Time::DeltaTime();
	}

	void EditorCamera::MoveSideways( const float magnitude )
	{
		Position += magnitude * GetRightDirection() * Speed * (float)Time::DeltaTime();
	}

	void EditorCamera::MouseRotate( const Vector2& mouseDelta )
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		Yaw += yawSign * mouseDelta.x * Sensitivity;
		Pitch += mouseDelta.y * Sensitivity;
	}

	void EditorCamera::RecalculateView()
	{
		Quaternion orientation = GetOrientation();
		m_View = glm::translate( Matrix4( 1.f ), Position ) * glm::toMat4( orientation );
		m_View = glm::inverse( m_View );
	}

	Vector3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate( GetOrientation(), Vector3( 0.0f, 1.0f, 0.0f ) );
	}

	Vector3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate( GetOrientation(), Vector3( 1.0f, 0.0f, 0.0f ) );
	}

	Vector3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate( GetOrientation(), Vector3( 0.0f, 0.0f, -1.0f ) );
	}

	Quaternion EditorCamera::GetOrientation() const
	{
		return Quaternion( Vector3( -Pitch, -Yaw, 0.f ) );
	}

}

#endif // IS_EDITOR