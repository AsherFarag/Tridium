#include "tripch.h"
#ifdef IS_EDITOR

#include "EditorCamera.h"
#include <Tridium/Input/Input.h>
#include <Tridium/Core/Application.h>

namespace Tridium::Editor {

	EditorCamera::EditorCamera() {}

	void EditorCamera::OnUpdate()
	{
		RecalculateView();

		Lerp();

		if ( m_IsMoving ) {
			m_TimeMoving += Time::DeltaTime();
			m_TimeMoving = MIN( m_TimeMoving, m_MaxTimeMoving );

			m_TimeMovingSpeedMultiplier = 1.0f + ( m_MaxTimeMovingSpeedMultiplier - 1.0f ) * ( m_TimeMoving / m_MaxTimeMoving );
		}
		else 
		{
			m_TimeMovingSpeedMultiplier = 1.0f;
			m_TimeMoving = 0.0f;
		}

		if ( Focused && !m_LerpData.IsLerping )
			HandleInput();

		m_LastMousePos = Input::GetMousePosition();
	}

	void EditorCamera::LerpTo( const Vector3& pos )
	{
		m_LerpData.CurrLerpTime - 0.0f;
		m_LerpData.IsLerping = true;
		m_LerpData.LerpToPos = pos;
	}

	void EditorCamera::HandleInput()
	{
		m_IsMoving = false;

		float dt = Time::DeltaTime();
		Vector3 up = GetUpDirection();
		Vector3 forward = GetForwardDirection();
		Vector3 right = GetRightDirection();

		float speed = Speed;
		speed *= m_TimeMovingSpeedMultiplier;

		if ( Input::IsKeyPressed( Input::KEY_LEFT_SHIFT ) )
		{
			speed *= 5.0f;
		}

		if ( Input::IsKeyPressed( Input::KEY_LEFT_CONTROL ) )
		{
			speed /= 5.f;
		}

		int forwardMag = Input::IsKeyPressed( Input::KEY_W ) - Input::IsKeyPressed( Input::KEY_S );
		MoveForward( forwardMag, speed );
		int sidewaysMag = Input::IsKeyPressed( Input::KEY_D ) - Input::IsKeyPressed( Input::KEY_A );
		MoveSideways( sidewaysMag, speed );

		if ( forwardMag != 0 || sidewaysMag != 0 )
			m_IsMoving = true;

		if ( Input::IsKeyPressed( Input::KEY_UP ) )
		{
			m_IsMoving = true;
			Position.y += speed * dt;
		}

		if ( Input::IsKeyPressed( Input::KEY_DOWN ) )
		{
			m_IsMoving = true;
			Position.y -= speed * dt;
		}

		// Rotation

		if ( Input::IsKeyPressed( Input::KEY_RIGHT ) )
		{
			Yaw += 0.5f * speed * dt;
		}
		if ( Input::IsKeyPressed( Input::KEY_LEFT ) )
		{
			Yaw -= 0.5f * speed * dt;
		}

		if ( Input::IsMouseButtonPressed( Input::MOUSE_BUTTON_RIGHT ) )
		{
			MouseRotate( Input::GetMousePosition() - m_LastMousePos );
		}
	}

	void EditorCamera::MoveForward( const float magnitude, const float speed )
	{
		Position += magnitude * GetForwardDirection() * speed * (float)Time::DeltaTime();
	}

	void EditorCamera::MoveSideways( const float magnitude, const float speed )
	{
		Position += magnitude * GetRightDirection() * speed * (float)Time::DeltaTime();
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

	void EditorCamera::Lerp()
	{
		if ( m_LerpData.IsLerping )
		{
			m_LerpData.CurrLerpTime += Time::DeltaTime();
			m_LerpData.CurrLerpTime = MIN( m_LerpData.LerpTime, m_LerpData.CurrLerpTime );
			Position = Position + ( m_LerpData.LerpToPos - Position ) * ( m_LerpData.CurrLerpTime / m_LerpData.LerpTime );

			if ( m_LerpData.CurrLerpTime == m_LerpData.LerpTime )
			{
				m_LerpData.CurrLerpTime = 0.0f;
				m_LerpData.IsLerping = false;
			}
		}
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

	Matrix4 EditorCamera::GetTransform() const
	{
		Matrix4 rotationMatrix = glm::toMat4( Quaternion( Vector3( Pitch, Yaw, 0.0f ) ) );

		constexpr Matrix4 identity = Matrix4( 1.0f );

		return glm::translate( identity, Position )
			* rotationMatrix
			* glm::scale( identity, Scale );
	}

}

#endif // IS_EDITOR