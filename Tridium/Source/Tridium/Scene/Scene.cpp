#include "tripch.h"
#include "Scene.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/ECS/Components/Types.h>


// TEMP!!!!
#include <Tridium/Physics/Jolt/JoltPhysicsScene.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CompoundShape.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>


namespace Tridium {

	static std::vector<JPH::Ref<JPH::Shape>> s_Shapes;

	Scene::Scene(const std::string& name)
		: m_Name( name ), m_SceneRenderer(*this)
	{
		m_PhysicsScene = PhysicsScene::Create();
	}

	Scene::~Scene()
	{
	}

	void Scene::OnBegin()
	{
		// Initialize Physics Scene
		{
			m_PhysicsScene->Init();

			// Add all GameObjects with RigidBodyComponent to the physics scene
			auto view = m_Registry.view<RigidBodyComponent, TransformComponent>();
			for ( auto entity : view )
			{
				auto& rb = view.get<RigidBodyComponent>( entity );
				auto& tc = view.get<TransformComponent>( entity );

				JPH::Ref<JPH::MutableCompoundShapeSettings> compoundSettings = new JPH::MutableCompoundShapeSettings();

				if ( auto* sc = TryGetComponentFromGameObject<SphereColliderComponent>( entity ) )
				{
					JPH::Ref<JPH::SphereShape> sphereShape = new JPH::SphereShape( sc->GetRadius() );
					s_Shapes.push_back( sphereShape.GetPtr() );
					JPH::Vec3 center = JPH::Vec3( sc->GetCenter().x, sc->GetCenter().y, sc->GetCenter().z );
					compoundSettings->AddShape( center, JPH::Quat::sIdentity(), sphereShape );
				}

				UniquePtr<JPH::BoxShape> boxShape;
				if ( auto* bc = TryGetComponentFromGameObject<BoxColliderComponent>( entity ) )
				{
					Vector3 scale = tc.GetWorldScale();
					JPH::Vec3 halfExtents = JPH::Vec3( bc->GetHalfExtents().x * scale.x, bc->GetHalfExtents().y * scale.y, bc->GetHalfExtents().z * scale .z);
					JPH::Ref<JPH::BoxShape> boxShape = new JPH::BoxShape( halfExtents );
					s_Shapes.push_back( boxShape.GetPtr() );
					JPH::Vec3 center = JPH::Vec3( bc->GetCenter().x, bc->GetCenter().y, bc->GetCenter().z );
					compoundSettings->AddShape( center, JPH::Quat::sIdentity(), boxShape );
				}

				JPH::Vec3 position = { tc.Position.x, tc.Position.y, tc.Position.z };
				JPH::Quat rotation = { tc.Rotation.Quat.x, tc.Rotation.Quat.y, tc.Rotation.Quat.z, tc.Rotation.Quat.w };
				JPH::EMotionType motionType = (JPH::EMotionType)rb.GetMotionType();
				JPH::BodyCreationSettings bodySettings( compoundSettings, position, rotation, motionType, (JPH::ObjectLayer)1 );

				bodySettings.mRestitution = 0.5f;

				rb.BodyID = m_PhysicsScene->AddPhysicsBody(&bodySettings);
			}
		}
	}

	void Scene::OnUpdate()
	{
		// Update physics
		{
			m_PhysicsScene->Tick( 1.0f / 60.0f );

			JPH::PhysicsSystem& physicsSystem = static_cast<JoltPhysicsScene*>( m_PhysicsScene.get() )->m_PhysicsSystem;
			JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

			auto view = m_Registry.view<RigidBodyComponent, TransformComponent>();
			for ( auto entity : view )
			{
				auto& rb = view.get<RigidBodyComponent>( entity );
				auto& tc = view.get<TransformComponent>( entity );

				JPH::Vec3 position = bodyInterface.GetPosition( JPH::BodyID{ rb.BodyID } );
				JPH::Quat rotation = bodyInterface.GetRotation( JPH::BodyID{ rb.BodyID } );
				tc.Position = { position.GetX(), position.GetY(), position.GetZ() };
				tc.Rotation.SetFromQuaternion( { rotation.GetW(), rotation.GetX(), rotation.GetY(), rotation.GetZ() } );
			}
		}

		// Update scriptable objects
		for ( const auto& storage : m_Registry.storage() )
		{
			// Nothing to iterate through, Continue
			if ( storage.second.size() == 0 )
				continue;

			// We know that the data will always be inherited from Component
			void* data = storage.second.value( storage.second[ 0 ] );
			Component* component = (Component*)data;

			// If the type is not inherited from ScriptableComponent, continue 
			if ( dynamic_cast<ScriptableComponent*>( component ) == nullptr )
				continue;

			for ( auto go : storage.second )
				reinterpret_cast<ScriptableComponent*>( storage.second.value( go ) )->OnUpdate();
		}
	}

	void Scene::OnEnd()
	{
		// Shutdown Physics Scene
		{
			m_PhysicsScene->Shutdown();
		}
	}

	GameObject Scene::InstantiateGameObject( const std::string& a_Name )
	{
		auto go = GameObject( m_Registry.create() );
		AddComponentToGameObject<GUIDComponent>( go );
		AddComponentToGameObject<TagComponent>( go, a_Name );
		AddComponentToGameObject<TransformComponent>( go );
		return go;
	}

	GameObject Scene::InstantiateGameObject( GUID a_GUID, const std::string& a_Name )
	{
		auto go = GameObject( m_Registry.create() );
		AddComponentToGameObject<GUIDComponent>( go, a_GUID );
		AddComponentToGameObject<TagComponent>( go, a_Name );
		AddComponentToGameObject<TransformComponent>( go );
		return go;
	}

	GameObject Scene::InstantiateGameObjectFrom( GameObject a_Source )
	{
		GameObject dst( m_Registry.create() );
		AddComponentToGameObject<GUIDComponent>( dst );
		AddComponentToGameObject<TagComponent>( dst, a_Source.GetTag() );
		TransformComponent& tc = AddComponentToGameObject<TransformComponent>( dst );
		tc.Position = a_Source.GetTransform().Position;
		tc.Rotation = a_Source.GetTransform().Rotation;
		tc.Scale = a_Source.GetTransform().Scale;

		for ( auto [id, storage] : m_Registry.storage() )
		{
			if ( !storage.contains( a_Source ) )
				continue;

			if ( storage.type() == Refl::MetaRegistry::ResolveMetaType<GUIDComponent>().info() )
				continue;

			if ( storage.type() == Refl::MetaRegistry::ResolveMetaType<TagComponent>().info() )
				continue;

			if ( storage.type() == Refl::MetaRegistry::ResolveMetaType<TransformComponent>().info() )
				continue;

			if ( storage.contains(dst) )
				storage.erase( dst );

			storage.push( dst, storage.value( a_Source ) );
		}

		return dst;
	}

	void Scene::CopyGameObject( GameObject a_Destination, GameObject a_Source )
	{

	}

	void Scene::Clear()
	{
		m_Registry.clear();
	}

	CameraComponent* Scene::GetMainCamera()
	{
		// If the scene doesn't have a main camera,
		// get the first camera in the scene
		if ( !m_Registry.valid( m_MainCamera ) )
		{
			auto cameras = m_Registry.view<CameraComponent>();
			// Return nullptr as there are no cameras in the scene
			if ( cameras.empty() )
				return nullptr;

			m_MainCamera = cameras.front();
		}
		return m_Registry.try_get<CameraComponent>( m_MainCamera );
	}

}