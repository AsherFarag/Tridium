#include <Tridium.h>
#include <Tridium/IO/SceneSerializer.h>
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Editor/Panels/Asset/MeshSourceImporterPanel.h>
#include <Tridium/Rendering/SceneRenderer.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Asset/Loaders/TextureLoader.h> 

using namespace Tridium;

class ShooterPlayerComponent : public ScriptableComponent
{
	REFLECT( ShooterPlayerComponent )
public:
	void OnBeginPlay() override
	{
		if ( m_GunModel.IsValid() )
		{
			m_DefaultGunPosition = m_GunModel.GetTransform().GetLocalPosition();
		}
	}

	void OnUpdate() override
	{
		m_FireTimer += Time::DeltaTime();
		if ( Input::IsMouseButtonPressed( Input::MOUSE_BUTTON_LEFT ) )
		{
			bool fired = Fire();
			if ( fired )
			{
				m_GunLerp = 1.0f;
			}
		}

		m_GunLerp -= Time::DeltaTime() * m_GunLerpSpeed;
		m_GunLerp = Math::Clamp( m_GunLerp, 0.0f, 1.0f );

		if ( m_GunModel.IsValid() )
		{
			TransformComponent& tc = m_GunModel.GetTransform();

			Vector3 recoilPosition = m_DefaultGunPosition - Vector3( 0.0f, 1.0f, 0.0f ) *m_GunRecoilDistance;
			Vector3 newPosition = Math::Lerp( m_DefaultGunPosition, recoilPosition, m_GunLerp );
			tc.Position = newPosition;
		}
	}

protected:
	bool Fire()
	{
		if ( m_FireTimer < m_FireRate )
		{
			return false;
		}

		CharacterControllerComponent* controller = GetGameObject().TryGetComponent<CharacterControllerComponent>();
		if ( !controller || !controller->GetCameraGameObject().IsValid() )
			return false;

		TransformComponent& cameraTransform = controller->GetCameraGameObject().GetTransform();
		// Camera view forward direction is currently flipped to the actual transform forward direction
		// This is a temporary fix until the camera component is fixed
		Vector3 lookDirection = cameraTransform.GetOrientation() * Vector3( 0.0f, 0.0f, -1.0f );

		Vector3 rayBegin = cameraTransform.GetWorldPosition();
		Vector3 rayEnd = rayBegin + lookDirection * 1000.0f;

		PhysicsBodyFilter filter;
		if ( auto* rigidBody = GetGameObject().TryGetComponent<RigidBodyComponent>() )
			filter = *rigidBody;

		RayCastResult hit = GetScene()->CastRay( rayBegin, rayEnd, ERayCastChannel::Camera, filter );
		if ( hit.Hit )
		{
			if ( RigidBodyComponent* hitRigidBody = hit.GetHitRigidBody() )
			{
				TE_CORE_DEBUG( "Hit: {0}", hitRigidBody->GetGameObject().GetTag() );
				hitRigidBody->AddImpulse( lookDirection * m_Force );
			}
		}

		m_FireTimer -= m_FireRate;
		return true;
	}

protected:
	GameObject m_GunModel;

	Vector3 m_DefaultGunPosition = Vector3( 0.0f, 0.0f, 0.0f );

	float m_GunRecoilDistance = 0.1f;
	float m_GunLerp = 0.0f;
	float m_GunLerpSpeed = 5.0f;
	float m_FireRate = 0.3f;
	float m_FireTimer = 0.0f;
	float m_Force = 1000.0f;
};

BEGIN_REFLECT_COMPONENT( ShooterPlayerComponent )
	BASE( ScriptableComponent )
	PROPERTY( m_GunModel, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( m_GunRecoilDistance, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( m_GunLerpSpeed, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( m_FireRate, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( m_Force, FLAGS( Serialize, EditAnywhere ) )
END_REFLECT( ShooterPlayerComponent )

class SandboxGameInstance : public Tridium::GameInstance
{
	virtual void Init() override
	{
	}
};

Tridium::GameInstance* Tridium::CreateGameInstance()
{
	return new SandboxGameInstance();
}