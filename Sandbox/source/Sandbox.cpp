#include <Tridium.h>
#include <Tridium/IO/SceneSerializer.h>
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Tridium/Editor/Panels/Asset/MeshSourceImporterPanel.h>
#include <Tridium/Graphics/Rendering/SceneRenderer.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Asset/Loaders/TextureLoader.h> 
#include <Tridium/Reflection/Reflection.h>

#include <Tridium/Editor/Commands/CommandManager.h>

#include <Tridium/Reflection/FieldReflection.h>

using namespace Tridium;

class ShooterPlayerComponent : public NativeScriptComponent
{
	REFLECT( ShooterPlayerComponent )
public:
	void OnBeginPlay()
	{
		if ( m_GunModel.IsValid() )
		{
			m_DefaultGunPosition = m_GunModel.GetTransform().GetLocalPosition();
		}
	}

	void OnUpdate( float a_DeltaTime )
	{
		m_FireTimer += a_DeltaTime;
		if ( Input::IsMouseButtonPressed( EInputMouseButton::Left ) )
		{
			bool fired = Fire();
			if ( fired )
			{
				m_GunLerp = 1.0f;
				ActivateMuzzleFlash();
			}
		}

		// Zoom
		{
			m_IsZoomed = Input::IsMouseButtonPressed( EInputMouseButton::Right );

			if ( m_IsZoomed )
				m_ZoomLerp += a_DeltaTime * m_ZoomLerpSpeed;
			else
				m_ZoomLerp -= a_DeltaTime * m_ZoomLerpSpeed;

			m_ZoomLerp = Math::Clamp( m_ZoomLerp, 0.0f, 1.0f );

			
			if ( CharacterControllerComponent* controller = GetGameObject().TryGetComponent<CharacterControllerComponent>() )
			{
				if ( CameraComponent* camera = controller->GetCameraGameObject().TryGetComponent<CameraComponent>() )
				{
					float fov = Math::Lerp( m_DefaultFov, m_DefaultFov * m_ZoomFovPercentage, m_ZoomLerp );
					camera->SceneCamera.SetPerspectiveFOV( fov );
				}
			}
		}

		m_GunLerp -= a_DeltaTime * m_GunLerpSpeed;
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
			m_GunMuzzleFlashTimer -= a_DeltaTime;
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

		RayCastResult hit = GetScene()->CastRay( rayBegin, rayEnd, ERayCastChannel::Camera, filter, true, Debug::EDrawDuration::Permanent, 10.0f );
		if ( hit.Hit )
		{
			GameObject hitGameObject = hit.HitGameObject;
			RigidBodyComponent& hitRigidBody = hitGameObject.GetComponent<RigidBodyComponent>();
			LOG( LogCategory::Default, Debug, "Hit: {0}", hitRigidBody.GetGameObject().GetTag() );
			hitRigidBody.AddImpulse( lookDirection * m_Force );
		}

		m_FireTimer = 0.0f;
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


	float m_DefaultFov = 90.0f;
	float m_ZoomFovPercentage = 0.5f;
	float m_ZoomLerp = 0.0f;
	float m_ZoomLerpSpeed = 5.0f;
	bool m_IsZoomed = false;
};

BEGIN_REFLECT_COMPONENT( ShooterPlayerComponent )
	BASE( NativeScriptComponent )
	PROPERTY( m_GunModel, Serialize | EditAnywhere )
	PROPERTY( m_GunMuzzleFlash, Serialize | EditAnywhere )
	PROPERTY( m_GunMuzzleFlashDuration, Serialize | EditAnywhere )
	PROPERTY( m_GunMuzzleFlashIntensity, Serialize | EditAnywhere )
	PROPERTY( m_GunRecoilDistance, Serialize | EditAnywhere )
	PROPERTY( m_GunLerp, VisibleAnywhere )
	PROPERTY( m_GunLerpSpeed, Serialize | EditAnywhere )
	PROPERTY( m_FireRate, Serialize | EditAnywhere )
	PROPERTY( m_Force, Serialize | EditAnywhere )
	PROPERTY( m_GunMuzzleFlashTimer )
	PROPERTY( m_DefaultGunPosition, EditAnywhere )
	PROPERTY( m_DefaultFov, EditAnywhere )
	PROPERTY( m_ZoomFovPercentage, EditAnywhere )
	PROPERTY( m_ZoomLerpSpeed, VisibleAnywhere )
	PROPERTY( m_IsZoomed, VisibleAnywhere )
END_REFLECT( ShooterPlayerComponent )


class EnemyAIComponent : public NativeScriptComponent
{
	REFLECT( EnemyAIComponent )
public:
	void OnBeginPlay()
	{
		GetTarget();
	}

	void OnUpdate( float a_DeltaTime )
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
	BASE( NativeScriptComponent )
	PROPERTY( m_Target, Serialize | EditAnywhere )
	PROPERTY( m_Speed, Serialize | EditAnywhere )
	PROPERTY( m_MaxSpeed, Serialize | EditAnywhere )
END_REFLECT( EnemyAIComponent )

class SandboxGameInstance : public Tridium::GameInstance
{
	virtual void Init() override
	{
	}
};

#if 0

template<typename T>
void PrintTypeName()
{
	std::cout << "Unknown, " << std::endl;
}

template<>
void PrintTypeName<int>()
{
	std::cout << "int, " << std::endl;
}

template<>
void PrintTypeName<float>()
{
	std::cout << "float, " << std::endl;
}

template<>
void PrintTypeName<bool>()
{
	std::cout << "bool, " << std::endl;
}

#include <Tridium/Reflection/FieldReflection.h>
#include <Tridium/Reflection/Reflect.h>

struct MyComponent
{
	REFLECT_TEST( MyComponent );
	int& GetInt()
	{
		std::cout << "\n GetInt was called " << std::format( "Value: {}", IntField.GetInternalValue() ) << std::endl;
		return IntField.GetInternalValue();
	}
	void SetInt( int a_Value )
	{
		std::cout << "\n SetInt was called " << std::format( "Old Value: {}, New Value: {}", IntField.GetInternalValue(), a_Value ) << std::endl;
		IntField.GetInternalValue() = a_Value;
	}
	void PrintFloat()
	{
		std::cout << "Float: " << FloatField.GetInternalValue() << std::endl;
	}

	Field<int, EditAnywhere, Range<0.1f, 10.015f>, Property<&GetInt, &SetInt>>
		IntField{ this, 10 };
	Field<float, Range<0.1f, 10.0f>>
		FloatField{ 5.0f };
	Field<bool, EditAnywhere>
		BoolField;
};

template<>
struct CustomReflector<MyComponent>
{
	Field<int, EditAnywhere, Range<0.1f, 10.0f>> IntField;
	Function<&MyComponent::PrintFloat> PrintFloat;
};

template<typename T>
void PrintTypeName( const T& a_Value )
{
	using ValueType = std::decay_t<decltype(a_Value)>;
	std::cout << GetTypeName<ValueType>();
}

template<typename _ValueType, IsFieldAttribute... _MetaAttributes>
void PrintFieldType( const Field<_ValueType, _MetaAttributes...>& a_Field )
{
	using FieldType = std::decay_t<decltype(a_Field)>;
	std::cout << GetTypeName<typename FieldType::ValueType>();
	if constexpr ( FieldType::template HasMeta<RangeAttribute>() )
	{
		std::cout << ", Range{ Min: " << FieldType::template GetMeta<RangeAttribute>().Min << ", Max: " << FieldType::template GetMeta<RangeAttribute>().Max << " }";
	}
}
#endif
void test()
{
	//MyComponent myComponent{};
	//myComponent.IntField = 33;
	//CustomReflector<MyComponent>{}.PrintFloat( myComponent );
	//std::cout << "Int Value " << myComponent.IntField << std::endl;
	//ForEachField( myComponent,
	//	[]( StringView a_Name, const auto& a_Field )
	//	{
	//		std::cout << "Field: " << a_Name << ", Type: ";
	//		PrintFieldType( a_Field );
	//		std::cout << std::endl;
	//	}
	//);

	//ForEachField( CustomReflector<MyComponent>{},
	//	[&]( StringView a_Name, const auto& a_Field )
	//	{
	//		using FieldType = std::decay_t<decltype(a_Field)>;
	//		if constexpr ( IsFunction<FieldType> )
	//		{
	//			std::cout << "Function: " << a_Name << ", Result: ";
	//			a_Field.Invoke( myComponent );
	//		}
	//		else
	//		{
	//			std::cout << "Field: " << a_Name;
	//		}
	//		std::cout << std::endl;
	//	}
	//);
}

struct Test
{
	Test()
	{
		test();
	}
};

Test testInstance;

GameInstance* Tridium::CreateGameInstance()
{

	return new SandboxGameInstance();
}