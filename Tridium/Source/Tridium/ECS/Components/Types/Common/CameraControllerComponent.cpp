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
		if ( !go.HasComponent<OldTransformComponent>() )
			return;
		
		constexpr auto up = Vector3::Up();
		auto& transform = GetGameObject().GetComponent<OldTransformComponent>();
		auto forward = transform.GetForward();
		auto right = Math::Cross( forward, up );


		if ( Input::IsKeyPressed( EInputKey::W ) )
			transform.Position -= Vector3(forward.X, 0, forward.Z ) * Speed * dt;

		if ( Input::IsKeyPressed( EInputKey::S ) )
			transform.Position += Vector3( forward.X, 0, forward.Z ) * Speed * dt;

		if ( Input::IsKeyPressed( EInputKey::A ) )
			transform.Position += right * Speed * dt;

		if ( Input::IsKeyPressed( EInputKey::D ) )
			transform.Position -= right * Speed * dt;

		if ( Input::IsKeyPressed( EInputKey::Space ) )
			transform.Position += up * Speed * dt;

		if ( Input::IsKeyPressed( EInputKey::LeftShift ) )
			transform.Position -= up * Speed * dt;

		// Mouse Scroll Zoom
		//if ( Input::GetMouseScrollYOffset() != m_LastMouseScroll )
		//{
		//	if ( OldCameraComponent* cam = go.TryGetComponent<OldCameraComponent>() )
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
		constexpr float yawSign = up.Y < 0 ? -1.0f : 1.0f;

		Vector3 euler = Math::Degrees( transform.Rotation.Euler );

		euler.Y -= yawSign * mouseDelta.X * LookSensitivity;
		euler.X -= mouseDelta.Y * LookSensitivity;

		constexpr float clampZone = 89.f;
		if ( euler.X < -clampZone )
			euler.X = -clampZone;
		else if ( euler.X > clampZone )
			euler.X = clampZone;

		transform.Rotation.SetFromEuler( Math::Radians( euler ) );

		m_LastMousePos = Input::GetMousePosition();
		m_LastMouseScroll = Input::GetMouseScrollYOffset();
	}

	BEGIN_REFLECT_COMPONENT( CameraControllerComponent )
		BASE( NativeScriptComponent )
		PROPERTY( Speed, Serialize | EditAnywhere )
		PROPERTY( LookSensitivity, Serialize | EditAnywhere )
	END_REFLECT_COMPONENT( CameraControllerComponent );
}