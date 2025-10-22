#include "tripch.h"
#include "CharacterControllerComponent.h"
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Reflection/Reflection.h>
#include <Tridium/Input/Input.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( CharacterControllerComponent, Scriptable )
		BASE( NativeScriptComponent )
		PROPERTY( m_Friction, Serialize | EditAnywhere )
		PROPERTY( m_MovementSpeed, Serialize | EditAnywhere )
		PROPERTY( m_MovementAcceleration, Serialize | EditAnywhere )
		PROPERTY( m_AirMovementControl, Serialize | EditAnywhere )
		PROPERTY( m_JumpForce, Serialize | EditAnywhere )
		PROPERTY( m_LookSensitivity, Serialize | EditAnywhere )
	END_REFLECT_COMPONENT( CharacterControllerComponent )

	void CharacterControllerComponent::OnBeginPlay()
	{
		OldRigidBodyComponent* rigidBody = GetGameObject().TryGetComponent<OldRigidBodyComponent>();
		if ( !rigidBody )
		{
			LOG( LogCategory::GameLogic, Warn, "'%s' does not have a RigidBodyComponent, which the CharacterControllerComponent requires!", GetGameObject().GetTag().c_str() );
			return;
		}
	}
	

	void CharacterControllerComponent::OnUpdate( float a_DeltaTime )
	{
		OldRigidBodyComponent* rigidBody = GetGameObject().TryGetComponent<OldRigidBodyComponent>();
		if ( !rigidBody )
			return;


		rigidBody->SetFriction( m_Friction );

		SharedPtr<IPhysicsScene> scene = rigidBody->GetBodyProxy().GetPhysicsScene().lock();
		Vector3 rayBegin = GetGameObject().GetTransform().Position;
		OldCapsuleColliderComponent* capsuleCollider = GetGameObject().TryGetComponent<OldCapsuleColliderComponent>();
		const float BodyHeight = capsuleCollider ? capsuleCollider->GetHalfHeight() + capsuleCollider->GetRadius() : 1.0f;
		Vector3 rayEnd = rayBegin - Vector3( 0.0f, 2.0f + BodyHeight, 0.0f );
		RayCastResult hitResult = scene->CastRay( rayBegin, rayEnd, ERayCastChannel::Camera, { rigidBody->GetBodyProxy().GetBodyID() });
		bool isGrounded = hitResult.Hit;

		rigidBody->SetAngularVelocity( Vector3( 0.0f ) );

		// Movement input
		Vector2 movementInput = Vector2( 0.0f );
		movementInput.Y -= Input::IsKeyPressed( EInputKey::W );
		movementInput.Y += Input::IsKeyPressed( EInputKey::S );
		movementInput.X += Input::IsKeyPressed( EInputKey::D );
		movementInput.X -= Input::IsKeyPressed( EInputKey::A );
		movementInput = movementInput.Length() > 0.0f ? movementInput.Normalized() : movementInput;
		movementInput *= isGrounded ? 1.0f : m_AirMovementControl;
		AddMovementInput( movementInput * a_DeltaTime );

		// Look input
		Vector2 lookInput = Input::GetMousePosition() - m_LastMousePosition;
		m_LastMousePosition = Input::GetMousePosition();
		AddLookInput( lookInput * a_DeltaTime );

		// Jump
		if ( Input::IsKeyPressed( EInputKey::Space ) && isGrounded && m_CanJump )
		{
			Jump();
			m_CanJump = false;
		}

		if ( !Input::IsKeyPressed( EInputKey::Space ) )
			m_CanJump |= isGrounded;
	}

	void CharacterControllerComponent::OnEndPlay()
	{
	}

	void CharacterControllerComponent::AddMovementInput( const Vector2& a_Input )
	{
		OldRigidBodyComponent* rigidBody = GetGameObject().TryGetComponent<OldRigidBodyComponent>();
		if ( !rigidBody )
			return;

		Vector3 forward = GetGameObject().GetTransform().GetForward();
		Vector3 right = GetGameObject().GetTransform().GetRight();

		Vector3 velocity = rigidBody->GetLinearVelocity();

		velocity += forward * a_Input.Y * m_MovementSpeed;
		velocity += right * a_Input.X * m_MovementSpeed;

		if ( velocity.Length() > m_MovementSpeed )
			velocity = velocity.Normalized() * m_MovementSpeed;

		rigidBody->SetLinearVelocity( velocity );
	}

	void CharacterControllerComponent::AddLookInput( const Vector2& a_Input )
	{
		OldCameraComponent* camera = m_CameraGameObject.TryGetComponent<OldCameraComponent>();
		if ( !camera )
		{
			if ( camera = GetGameObject().TryGetComponentInChildren<OldCameraComponent>() )
			{
				m_CameraGameObject = camera->GetGameObject();
			}
			else 
			{
				LOG( LogCategory::GameLogic, Warn, "'{0}' does not have a Child GameObject with a CameraComponent, which the CharacterControllerComponent requires!", GetGameObject().GetTag().c_str() );
				return;
			}
		}

		OldTransformComponent& transform = GetGameObject().GetTransform();
		Quaternion rotation = transform.Rotation.GetQuaternion();
		Quaternion yawRotation = Quaternion( Vector3( 0.0f, -glm::radians( a_Input.X * m_LookSensitivity ), 0.0f ) );
		rotation = yawRotation * rotation;
		transform.Rotation.SetFromQuaternion( rotation );

		OldTransformComponent& cameraTransform = m_CameraGameObject.GetTransform();
		Vector3 cameraRotation = cameraTransform.Rotation.GetEuler();
		cameraRotation.X -= glm::radians( a_Input.Y * m_LookSensitivity );
		cameraRotation.X = glm::clamp( cameraRotation.X, glm::radians( -89.0f ), glm::radians( 89.0f ) );
		cameraTransform.Rotation.SetFromEuler( cameraRotation );
	}

	void CharacterControllerComponent::Jump()
	{
		OldRigidBodyComponent* rigidBody = GetGameObject().TryGetComponent<OldRigidBodyComponent>();
		if ( !rigidBody )
			return;

		rigidBody->AddImpulse( Vector3( 0.0f, m_JumpForce * 1000.0f, 0.0f ) );
	}

}