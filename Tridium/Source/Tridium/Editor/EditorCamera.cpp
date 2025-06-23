#include "tripch.h"
#if IS_EDITOR

#include "EditorCamera.h"
#include <Tridium/Input/Input.h>
#include <Tridium/Core/Application.h>

namespace Tridium {

	EditorCamera::EditorCamera() {}

	void EditorCamera::OnUpdate()
	{
		RecalculateView();

		Lerp();

		if ( m_IsMoving ) {
			m_TimeMoving += Time::DeltaTime();
			m_TimeMoving = Math::Min( m_TimeMoving, m_MaxTimeMoving );

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
		m_LerpData.CurrLerpTime = 0.0f;
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

		if ( Input::IsKeyPressed( EInputKey::LeftShift ) )
		{
			speed *= 5.0f;
		}

		if ( Input::IsKeyPressed( EInputKey::LeftControl ) )
		{
			speed /= 5.f;
		}

		int forwardMag = Input::IsKeyPressed( EInputKey::W ) - Input::IsKeyPressed( EInputKey::S );
		MoveForward( forwardMag, speed );
		int sidewaysMag = Input::IsKeyPressed( EInputKey::D ) - Input::IsKeyPressed( EInputKey::A );
		MoveSideways( sidewaysMag, speed );

		if ( forwardMag != 0 || sidewaysMag != 0 )
			m_IsMoving = true;

		if ( Input::IsKeyPressed( EInputKey::Up ) )
		{
			m_IsMoving = true;
			Position.Y += speed * dt;
		}

		if ( Input::IsKeyPressed( EInputKey::Down ) )
		{
			m_IsMoving = true;
			Position.Y -= speed * dt;
		}

		// Rotation

		if ( Input::IsKeyPressed( EInputKey::Right ) )
		{
			Yaw += 0.5f * speed * dt;
		}
		if ( Input::IsKeyPressed( EInputKey::Left ) )
		{
			Yaw -= 0.5f * speed * dt;
		}

		if ( Input::IsMouseButtonPressed( EInputMouseButton::Right ) )
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
		float yawSign = GetUpDirection().Y < 0 ? -1.0f : 1.0f;
		Yaw += yawSign * mouseDelta.X * Sensitivity;
		Pitch += mouseDelta.Y * Sensitivity;
		Pitch = glm::clamp( Pitch, glm::radians( -89.0f ), glm::radians( 89.0f ) );
	}

	void EditorCamera::RecalculateView()
	{
		Quaternion orientation = GetOrientation();
		m_View = Math::Translate( Position ) * Math::ToMat4( orientation );
		m_View = Math::Inverse( m_View );
	}

	void EditorCamera::Lerp()
	{
		if ( m_LerpData.IsLerping )
		{
			m_LerpData.CurrLerpTime += Time::DeltaTime();
			m_LerpData.CurrLerpTime = Math::Min( m_LerpData.LerpTime, m_LerpData.CurrLerpTime );
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
		return Math::Rotate( GetOrientation(), Vector3::Up() );
	}

	Vector3 EditorCamera::GetRightDirection() const
	{
		return Math::Rotate( GetOrientation(), Vector3::Right() );
	}

	Vector3 EditorCamera::GetForwardDirection() const
	{
		return Math::Rotate( GetOrientation(), Vector3::Forward() );
	}

	Quaternion EditorCamera::GetOrientation() const
	{
		return Quaternion( Vector3( -Pitch, -Yaw, 0.f ) );
	}

	Matrix4 EditorCamera::GetTransform() const
	{
		Matrix4 rotationMatrix = glm::toMat4( Quaternion( Vector3( Pitch, Yaw, 0.0f ) ) );

		constexpr Matrix4 identity = Matrix4( 1.0f );

		return Math::Translate( identity, Position )
			* rotationMatrix
			* Math::Scale( identity, Scale );
	}

	void EditorCamera::SetViewMatrix( const Matrix4& view )
	{
		// Extract look dir from the view matrix
		Vector3 lookDir = glm::normalize( glm::vec3( view[0][2], view[1][2], view[2][2] ) );
		// Extract up dir from the view matrix
		Vector3 upDir = glm::normalize( glm::vec3( view[0][1], view[1][1], view[2][1] ) );

		// Calculate pitch and yaw from the look dir
		Pitch = Math::ASin( lookDir.Y );
		Yaw = Math::ATan( lookDir.X, lookDir.Z );

		m_View = view;
	}

}

#endif // IS_EDITOR