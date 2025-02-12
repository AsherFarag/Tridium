#pragma once
#include <Tridium/ECS/Components/NativeScriptComponent.h>

namespace Tridium {

	class CharacterControllerComponent : public NativeScriptComponent
	{
		REFLECT( CharacterControllerComponent );
	public:
		CharacterControllerComponent() = default;

		void OnBeginPlay();
		void OnUpdate( float a_DeltaTime );
		void OnEndPlay();

		void AddMovementInput( const Vector2& a_Input );
		void AddLookInput( const Vector2& a_Input );
		void Jump();

		GameObject GetCameraGameObject() const { return m_CameraGameObject; }
		void SetCameraGameObject( GameObject a_CameraGameObject ) { m_CameraGameObject = a_CameraGameObject; }

	protected:
		float m_Friction = 10.0f; // The friction of the character
		float m_MovementSpeed = 3.0f; // The maximum speed the character can accelerate to
		float m_MovementAcceleration = 1.0f; // The rate at which the character accelerates
		float m_AirMovementControl = 0.1f; // The rate at which the character can control movement in the air
		float m_JumpForce = 5.0f; // The force applied when the character jumps
		float m_LookSensitivity = 0.1f; // The sensitivity of the mouse when looking around

		// TEMP
		bool m_CanJump = true;

	private:
		GameObject m_CameraGameObject;
		Vector2 m_LastMousePosition = Vector2( 0.0f );
	};
}