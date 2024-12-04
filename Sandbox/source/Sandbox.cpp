#include <Tridium.h>
#include <Tridium/IO/SceneSerializer.h>
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Editor/Panels/Asset/MeshSourceImporterPanel.h>
#include <Tridium/Rendering/SceneRenderer.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Asset/Loaders/TextureLoader.h> 
#include <Tridium/Reflection/Reflection.h>

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
				ActivateMuzzleFlash();
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

		if ( m_GunMuzzleFlashTimer > 0.0f )
		{
			m_GunMuzzleFlashTimer -= Time::DeltaTime();
			if ( m_GunMuzzleFlashTimer <= 0.0f )
			{
				PointLightComponent* light = m_GunMuzzleFlash.TryGetComponent<PointLightComponent>();
				if ( light )
				{
					light->Intensity = 0.0f;
				}
			}

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

	void ActivateMuzzleFlash()
	{
		if ( !m_GunMuzzleFlash.IsValid() )
			return;

		PointLightComponent* light = m_GunMuzzleFlash.TryGetComponent<PointLightComponent>();
		if ( !light )
			return;

		light->Intensity = m_GunMuzzleFlashIntensity;
		m_GunMuzzleFlashTimer = m_GunMuzzleFlashDuration;
	}

protected:
	GameObject m_GunModel;
	GameObject m_GunMuzzleFlash;
	float m_GunMuzzleFlashDuration = 0.1f;
	float m_GunMuzzleFlashIntensity = 100.0f;
	float m_GunMuzzleFlashTimer = 0.0f;

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
	PROPERTY( m_GunModel, Serialize | EditAnywhere )
	PROPERTY( m_GunMuzzleFlash, Serialize | EditAnywhere )
	PROPERTY( m_GunMuzzleFlashDuration, Serialize | EditAnywhere )
	PROPERTY( m_GunMuzzleFlashIntensity, Serialize | EditAnywhere )
	PROPERTY( m_GunRecoilDistance, Serialize | EditAnywhere )
	PROPERTY( m_GunLerp, Serialize | EditAnywhere )
	PROPERTY( m_GunLerpSpeed, Serialize | EditAnywhere )
	PROPERTY( m_FireRate, Serialize | EditAnywhere )
	PROPERTY( m_Force, Serialize | EditAnywhere )
	PROPERTY( m_GunMuzzleFlashTimer )
	PROPERTY( m_DefaultGunPosition, EditAnywhere )
END_REFLECT( ShooterPlayerComponent )


class EnemyAIComponent : public ScriptableComponent
{
	REFLECT( EnemyAIComponent )
public:
	void OnBeginPlay() override
	{
		GetTarget();
	}

	void OnUpdate() override
	{
		if ( !m_Target.IsValid() )
		{
			GetTarget();
			return;
		}

		if ( RigidBodyComponent* rb = GetGameObject().TryGetComponent<RigidBodyComponent>() )
		{
			TransformComponent& tc = GetGameObject().GetTransform();
			TransformComponent& targetTransform = m_Target.GetTransform();

			Vector3 direction = targetTransform.GetWorldPosition() - tc.GetWorldPosition();
			direction.y = 0.0f;
			direction = glm::normalize( direction );

			Vector3 velocity = rb->GetLinearVelocity() + direction * m_MaxSpeed;
			if ( glm::length( velocity ) <= m_MaxSpeed )
			{
				rb->SetLinearVelocity( velocity );
			}
		}
	}

protected:
	void GetTarget()
	{
		GameObject player = GetScene()->FindGameObjectByTag("Player");
		if ( player.IsValid() )
		{
			m_Target = player;
		}
	}

protected:
	GameObject m_Target;
	float m_Speed = 1.0f;
	float m_MaxSpeed = 5.0f;
};

BEGIN_REFLECT_COMPONENT( EnemyAIComponent )
	BASE( ScriptableComponent )
	PROPERTY( m_Target, Serialize | EditAnywhere )
	PROPERTY( m_Speed, Serialize | EditAnywhere )
	PROPERTY( m_MaxSpeed, Serialize | EditAnywhere )
END_REFLECT( EnemyAIComponent )

#include <Tridium/Core/Delegate.h>

struct Base
{

	virtual void Test() { TE_CORE_DEBUG( "Base" ); };
};

struct Derived : public Base
{
	virtual void Test() override { TE_CORE_DEBUG( "Derived" ); };
	std::string var = "Hello";
};

void Test( int a )
{
	TE_CORE_DEBUG( "Test {0}", a );
}

class SandboxGameInstance : public Tridium::GameInstance
{
	virtual void Init() override
	{
		struct Invokable
		{
			void operator()( int x ) const { std::cout << "Invokable: " << x << std::endl; }
		};

		Delegate<void, int> del;
		Invokable obj;
		del.Bind( obj ); // Bind pointer to invokable object
		del(5); // Output: "Invokable: 15"

		Delegate<void, int> del2(Test);
		del2( 10 );

		Derived d;
		Delegate<void> del3;
		del3.Bind<&Derived::Test>( &d );
		del3();

		Delegate<void> del4;
		del4.Bind( [&]() { TE_CORE_DEBUG( "Lamda Derived '{0}'", d.var ); } );
		del4();

		Delegate<void> del5;
		del5.Bind( []() { TE_CORE_DEBUG( "Lamda" ); } );
		del5();

		Delegate<void> del6;
		del6.Bind( +[]() { TE_CORE_DEBUG( "Lamda 2" ); } );
		del6();

		//
	}
};

GameInstance* Tridium::CreateGameInstance()
{
	return new SandboxGameInstance();
}