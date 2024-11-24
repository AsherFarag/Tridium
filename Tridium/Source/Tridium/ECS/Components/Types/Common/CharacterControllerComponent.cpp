#include "tripch.h"
#include "CharacterControllerComponent.h"
#include <Tridium/ECS/Components/Types.h>

namespace Tridium {

	void CharacterControllerComponent::OnBeginPlay()
	{
		RigidBodyComponent* rigidBody = GetGameObject().TryGetComponent<RigidBodyComponent>();
		if ( !rigidBody )
		{
			TE_CORE_WARN( "'%s' does not have a RigidBodyComponent, which the CharacterControllerComponent requires!", GetGameObject().GetTag().c_str() );
			return;
		}
	}

	void CharacterControllerComponent::OnUpdate()
	{
		RigidBodyComponent* rigidBody = GetGameObject().TryGetComponent<RigidBodyComponent>();
		if ( !rigidBody )
			return;


		rigidBody->SetFriction( m_Friction );

		const float deltaTime = Time::DeltaTime();

		SharedPtr<PhysicsScene> scene = rigidBody->GetBodyProxy().GetPhysicsScene().lock();
		Vector3 rayBegin = GetGameObject().GetTransform().Position;
		CapsuleColliderComponent* capsuleCollider = GetGameObject().TryGetComponent<CapsuleColliderComponent>();
		const float BodyHeight = capsuleCollider ? capsuleCollider->GetHalfHeight() + capsuleCollider->GetRadius() : 1.0f;
		Vector3 rayEnd = rayBegin - Vector3( 0.0f, 2.0f + BodyHeight, 0.0f );
		RayCastResult hitResult = scene->CastRay( rayBegin, rayEnd, ERayCastChannel::Camera, { rigidBody->GetBodyProxy().GetBodyID() });
		bool isGrounded = hitResult.Hit;

		rigidBody->SetAngularVelocity( Vector3( 0.0f ) );

		// Movement input
		Vector2 movementInput = Vector2( 0.0f );
		movementInput.y -= Input::IsKeyPressed( Input::KEY_W );
		movementInput.y += Input::IsKeyPressed( Input::KEY_S );
		movementInput.x += Input::IsKeyPressed( Input::KEY_D );
		movementInput.x -= Input::IsKeyPressed( Input::KEY_A );
		movementInput = glm::length( movementInput ) > 0.0f ? glm::normalize( movementInput ) : movementInput;
		movementInput *= isGrounded ? 1.0f : m_AirMovementControl;
		AddMovementInput( movementInput * deltaTime );

		// Look input
		Vector2 lookInput = Input::GetMousePosition() - m_LastMousePosition;
		m_LastMousePosition = Input::GetMousePosition();
		AddLookInput( lookInput * deltaTime );

		// Jump
		if ( Input::IsKeyPressed( Input::KEY_SPACE ) && isGrounded && m_CanJump )
		{
			Jump();
			m_CanJump = false;
		}

		if ( !Input::IsKeyPressed( Input::KEY_SPACE ) )
			m_CanJump |= isGrounded;
	}

	void CharacterControllerComponent::OnDestroy()
	{
	}

	void CharacterControllerComponent::AddMovementInput( const Vector2& a_Input )
	{
		RigidBodyComponent* rigidBody = GetGameObject().TryGetComponent<RigidBodyComponent>();
		if ( !rigidBody )
			return;

		Vector3 forward = GetGameObject().GetTransform().GetForward();
		Vector3 right = GetGameObject().GetTransform().GetRight();

		Vector3 velocity = rigidBody->GetLinearVelocity();

		velocity += forward * a_Input.y * m_MovementSpeed;
		velocity += right * a_Input.x * m_MovementSpeed;

		if ( glm::length( velocity ) > m_MovementSpeed )
			velocity = glm::normalize( velocity ) * m_MovementSpeed;

		rigidBody->SetLinearVelocity( velocity );
	}

	void CharacterControllerComponent::AddLookInput( const Vector2& a_Input )
	{
		CameraComponent* camera = GetGameObject().TryGetComponent<CameraComponent>();
		if ( !camera )
			return;

		camera->Pitch += a_Input.y * m_LookSensitivity;
		camera->Pitch = CLAMP( -89.0f, 89.0f, camera->Pitch );

		TransformComponent& transform = GetGameObject().GetTransform();
		Quaternion rotation = transform.Rotation.GetQuaternion();
		Quaternion yawRotation = Quaternion( Vector3( 0.0f, -glm::radians( a_Input.x * m_LookSensitivity ), 0.0f ) );
		rotation = yawRotation * rotation;
		transform.Rotation.SetFromQuaternion( rotation );
	}

	void CharacterControllerComponent::Jump()
	{
		RigidBodyComponent* rigidBody = GetGameObject().TryGetComponent<RigidBodyComponent>();
		if ( !rigidBody )
			return;

		rigidBody->AddImpulse( Vector3( 0.0f, m_JumpForce * 1000.0f, 0.0f ) );
	}

}