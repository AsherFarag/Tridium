#pragma once
#include <Tridium/Scene/Scene.h>

// Old
#include <Tridium/Core/Memory.h>
#include <Tridium/Math/Math.h>
#include <Tridium/Scene/SceneSystem.h>
#include <Tridium/Reflection/ReflectionFwd.h>

#include "RayCast.h"
#include "PhysicsBody.h"
#include "PhysicsLayer.h"
#include "PhysicsFilter.h"

#ifdef TE_SHIPPING
	#define USE_DEBUG_RENDERER 0
#else
	#define USE_DEBUG_RENDERER 1
#endif

namespace Tridium {

	// Forward Declarations
	class OldScene;
	class RigidBodyComponent;
	class OldTransformComponent;
	class OldGameObject;
	struct RayCastResult;
	// --------------------

	enum class ESixDOFConstraintMotion : uint8_t
	{
		Locked,
		Limited,
		Free
	};

	struct LinearMotionConstraint
	{
		REFLECT( LinearMotionConstraint );

		ESixDOFConstraintMotion XMotion = ESixDOFConstraintMotion::Free;
		ESixDOFConstraintMotion YMotion = ESixDOFConstraintMotion::Free;
		ESixDOFConstraintMotion ZMotion = ESixDOFConstraintMotion::Free;

		float XLimit = 0.0f;
		float YLimit = 0.0f;
		float ZLimit = 0.0f;
	};

	struct AngularMotionConstraint
	{
		REFLECT( AngularMotionConstraint );

		ESixDOFConstraintMotion Swing1Motion = ESixDOFConstraintMotion::Free;
		ESixDOFConstraintMotion Swing2Motion = ESixDOFConstraintMotion::Free;
		ESixDOFConstraintMotion TwistMotion = ESixDOFConstraintMotion::Free;

		float Swing1Limit = 0.0f;
		float Swing2Limit = 0.0f;
		float TwistLimit = 0.0f;
	};

	//=================================================================================================
	// Physics Scene Interface: Base class for physics engine implementations.
	// Works as a bridge between the Scene and the physics engine.
	//=================================================================================================
	class IPhysicsScene
	{
	public:

		//=============================================================================================
		static UniquePtr<IPhysicsScene> Create();

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void Tick( float a_TimeStep ) = 0;

		virtual OldGameObject GetGameObjectFromPhysicsBody( PhysicsBodyID a_BodyID ) const = 0;
		virtual PhysicsBodyID GetPhysicsBodyFromGameObject( OldGameObject a_GameObject ) const = 0;

		virtual RayCastResult CastRay( const Vector3& a_Start, const Vector3& a_End, ERayCastChannel a_Channel = ERayCastChannel::Visibility, const PhysicsBodyFilter& a_BodyFilter = {} ) = 0;

		virtual void RemovePhysicsBody( PhysicsBodyID a_PhysicsBodyID ) = 0;
		virtual void RemovePhysicsBody( RigidBodyComponent& a_RigidBody ) = 0;
		virtual bool AddPhysicsBody( const OldGameObject& a_GameObject, RigidBodyComponent& a_RigidBody, OldTransformComponent& a_TransformComponent ) = 0;
		virtual bool UpdatePhysicsBody( const OldGameObject& a_GameObject, RigidBodyComponent& a_RigidBody, OldTransformComponent& a_TransformComponent ) = 0;

		virtual void UpdatePhysicsBodyTransform( const RigidBodyComponent& a_RigidBody, const OldTransformComponent& a_TransformComponent ) = 0;

		virtual Vector3 GetPhysicsBodyPosition( PhysicsBodyID a_BodyID ) const = 0;
		virtual Quaternion GetPhysicsBodyRotation( PhysicsBodyID a_BodyID ) const = 0;
		virtual Vector3 GetPhysicsBodyLinearVelocity( PhysicsBodyID a_BodyID ) const = 0;
		virtual Vector3 GetPhysicsBodyAngularVelocity( PhysicsBodyID a_BodyID ) const = 0;
		virtual bool IsPhysicsBodySleeping( PhysicsBodyID a_BodyID ) const = 0;

		virtual void SetPhysicsBodyPosition( PhysicsBodyID a_BodyID, const Vector3& a_Position ) = 0;
		virtual void SetPhysicsBodyRotation( PhysicsBodyID a_BodyID, const Quaternion& a_Rotation ) = 0;
		virtual void SetPhysicsBodyFriction( PhysicsBodyID a_BodyID, float a_Friciton ) = 0;
		virtual void SetPhysicsBodyLinearVelocity( PhysicsBodyID a_BodyID, const Vector3& a_LinearVelocity ) = 0;
		virtual void SetPhysicsBodyAngularVelocity( PhysicsBodyID a_BodyID, const Vector3& a_AngularVelocity ) = 0;
		virtual void AddImpulseToPhysicsBody( PhysicsBodyID a_BodyID, const Vector3& a_Impulse ) = 0;
		virtual void AddImpulseToPhysicsBody( PhysicsBodyID a_BodyID, const Vector3& a_Impulse, const Vector3& a_Position ) = 0;

	#if USE_DEBUG_RENDERER
		virtual void RenderDebug( const Matrix4& a_ViewProjection ) = 0;
	#endif

	protected:

		//=============================================================================================
		friend class PhysicsSceneSystem;
		class Scene* m_OwningScene = nullptr;

		//=============================================================================================
		friend OldScene;
		OldScene* m_Scene;

	};

	class OldPhysicsSceneSystem : public OldISceneSystem
	{
	public:
		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void OnSceneEvent( const SceneEventPayload& a_Event ) override;

	private:
		void OnComponentCreated( const OnComponentCreatedEvent& a_Event );

	private:
		IPhysicsScene* m_PhysicsScene;
	};

	//=================================================================================================
	// PhysicsSceneSystem: Scene system that manages the physics scene.
	//=================================================================================================
	class PhysicsSceneSystem : public ISceneSystem
	{
	public:

		//=============================================================================================
		IPhysicsScene* PhysicsScene() const { return m_PhysicsScene.get(); }

		//=============================================================================================
		RayCastResult CastRay( const Vector3& a_Start, const Vector3& a_End, const RayCastParams& a_Params ) const;

	protected:

		//=============================================================================================
		void Init() override;
		void Shutdown() override;

		//=============================================================================================
		void OnRigidBodyComponentCreated( EntityComponentRegistry& a_Registry, EntityID a_Entity );
		void OnRigidBodyComponentDestroyed( EntityComponentRegistry& a_Registry, EntityID a_Entity );

	protected:

		//=============================================================================================
		UniquePtr<IPhysicsScene> m_PhysicsScene;

		//=============================================================================================
		EntityEventHandle m_OnRigidBodyComponentCreatedHandle;
		EntityEventHandle m_OnRigidBodyComponentDestroyedHandle;

	};

} // namespace Tridium