#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	class CharacterControllerComponent : public ScriptableComponent
	{
		REFLECT( CharacterControllerComponent );
	public:
		CharacterControllerComponent() = default;

		virtual void OnBeginPlay() override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		void AddMovementInput( const Vector2& a_Input );
		void AddLookInput( const Vector2& a_Input );
		void Jump();

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
		Vector2 m_LastMousePosition = Vector2( 0.0f );
	};

	BEGIN_REFLECT_COMPONENT( CharacterControllerComponent )
		BASE( ScriptableComponent )
		PROPERTY( m_Friction, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( m_MovementSpeed, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( m_MovementAcceleration, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( m_AirMovementControl, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( m_JumpForce, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( m_LookSensitivity, FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( CharacterControllerComponent );
}