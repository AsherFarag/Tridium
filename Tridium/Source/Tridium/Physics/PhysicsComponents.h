#pragma once
#include <Tridium/Scene/Component.h>
#include <Tridium/Physics/MotionType.h>
#include <Tridium/Physics/PhysicsScene.h>

namespace Tridium {

	//=================================================================================================
	// Rigid Body Component: Component that adds a rigid body to an entity for physics simulation.
	// For a game object to be affected by physics, it must have a RigidBodyComponent.
	//=================================================================================================
	struct RigidBodyComponent : Component
	{
	private:

		//=============================================================================================
		friend class PhysicsSceneSystem;

		//=============================================================================================
		IPhysicsScene* m_PhysicsScene = nullptr;
		PhysicsBodyID m_BodyID = NullPhysicsBodyID;
		EPhysicsLayer m_PhysicsLayer = EPhysicsLayer::Dynamic;
		EMotionType m_MotionType = EMotionType::Dynamic;
		float m_MassScale = 1.0f;
		float m_GravityScale = 1.0f;
		float m_Restitution = 0.5f;
		LinearMotionConstraint m_LinearMotionConstraint{};
		AngularMotionConstraint m_AngularMotionConstraint{};

	public:

		//=============================================================================================
		IPhysicsScene* PhysicsScene() const { return m_PhysicsScene; }
		PhysicsBodyID BodyID() const { return m_BodyID; }
		bool Valid() const { return m_BodyID != NullPhysicsBodyID && m_PhysicsScene != nullptr; }
		bool IsSleeping() const { return Valid() ? m_PhysicsScene->IsPhysicsBodySleeping( m_BodyID ) : false; }

		//=============================================================================================
		EPhysicsLayer PhysicsLayer() const { return m_PhysicsLayer; }
		EMotionType MotionType() const { return m_MotionType; }
		Vector3 LinearVelocity() const { return Valid() ? m_PhysicsScene->GetPhysicsBodyLinearVelocity( m_BodyID ) : Vector3::Zero(); }
		Vector3 AngularVelocity() const { return Valid() ? m_PhysicsScene->GetPhysicsBodyAngularVelocity( m_BodyID ) : Vector3::Zero(); }
		float GravityScale() const { return m_GravityScale; }
		float Restitution() const { return m_Restitution; }
		float MassScale() const { return m_MassScale; }
		const LinearMotionConstraint& LinearMotionConstraint() const { return m_LinearMotionConstraint; }
		const AngularMotionConstraint& AngularMotionConstraint() const { return m_AngularMotionConstraint; }

		//=============================================================================================
		void SetPhysicsLayer( EPhysicsLayer a_PhysicsLayer ) { m_PhysicsLayer = a_PhysicsLayer; }
		void SetMotionType( EMotionType a_MotionType ) { m_MotionType = a_MotionType; }
		void SetMassScale( float a_MassScale ) { m_MassScale = a_MassScale; }
		void SetGravityScale( float a_GravityScale ) { m_GravityScale = a_GravityScale; }
		void SetRestitution( float a_Restitution ) { m_Restitution = a_Restitution; }
		void SetFriction( float a_Friction ) { ASSERT( Valid() ); m_PhysicsScene->SetPhysicsBodyFriction( m_BodyID, a_Friction ); }
		void SetLinearVelocity( const Vector3& a_LinearVelocity ) { ASSERT( Valid() ); m_PhysicsScene->SetPhysicsBodyLinearVelocity( m_BodyID, a_LinearVelocity ); }
		void SetAngularVelocity( const Vector3& a_AngularVelocity ) { ASSERT( Valid() ); m_PhysicsScene->SetPhysicsBodyAngularVelocity( m_BodyID, a_AngularVelocity ); }
		void AddImpulse( const Vector3& a_Impulse ) { ASSERT( Valid() ); m_PhysicsScene->AddImpulseToPhysicsBody( m_BodyID, a_Impulse ); }
	};

	//=================================================================================================
	// Sphere Collider Component: Adds a sphere collider to an entity for physics simulation.
	// Performance Cost: Low
	//=================================================================================================
	struct SphereColliderComponent : Component
	{
	private:

		//=============================================================================================
		Vector3 m_Center = { 0.0f, 0.0f, 0.0f };
		Rotator m_Rotation = { 0.0f, 0.0f, 0.0f };
		float m_Radius = 0.5f;

	public:

		//=============================================================================================
		const Vector3& Center() const { return m_Center; }
		const Rotator& Rotation() const { return m_Rotation; }
		float Radius() const { return m_Radius; }

		//=============================================================================================
		void SetCenter( const Vector3& a_Center ) { m_Center = a_Center; }
		void SetRotation( const Rotator& a_Rotation ) { m_Rotation = a_Rotation; }
		void SetRadius( float a_Radius ) { m_Radius = a_Radius; }

	};

	//=================================================================================================
	// Box Collider Component: Adds a box collider to an entity for physics simulation.
	// Performance Cost: Low
	//=================================================================================================
	struct BoxColliderComponent : Component
	{
	private:

		//=============================================================================================
		Vector3 m_Center = { 0.0f, 0.0f, 0.0f };
		Rotator m_Rotation = { 0.0f, 0.0f, 0.0f };
		Vector3 m_HalfExtents = { 0.5f, 0.5f, 0.5f };

	public:

		//=============================================================================================
		const Vector3& Center() const { return m_Center; }
		const Rotator& Rotation() const { return m_Rotation; }
		const Vector3& HalfExtents() const { return m_HalfExtents; }

		//=============================================================================================
		void SetCenter( const Vector3& a_Center ) { m_Center = a_Center; }
		void SetRotation( const Rotator& a_Rotation ) { m_Rotation = a_Rotation; }
		void SetHalfExtents( const Vector3& a_HalfExtents ) { m_HalfExtents = a_HalfExtents; }

	};

	//=================================================================================================
	// Capsule Collider Component: Adds a capsule collider to an entity for physics simulation.
	// Performance Cost: Medium
	//=================================================================================================
	struct CapsuleColliderComponent : Component
	{
	private:

		//=============================================================================================
		Vector3 m_Center = { 0.0f, 0.0f, 0.0f };
		Rotator m_Rotation = { 0.0f, 0.0f, 0.0f };
		float m_Radius = 0.5f;
		float m_HalfHeight = 0.5f;

	public:

		//=============================================================================================
		const Vector3& Center() const { return m_Center; }
		const Rotator& Rotation() const { return m_Rotation; }
		float Radius() const { return m_Radius; }
		float HalfHeight() const { return m_HalfHeight; }

		//=============================================================================================
		void SetCenter( const Vector3& a_Center ) { m_Center = a_Center; }
		void SetRotation( const Rotator& a_Rotation ) { m_Rotation = a_Rotation; }
		void SetRadius( float a_Radius ) { m_Radius = a_Radius; }
		void SetHalfHeight( float a_HalfHeight ) { m_HalfHeight = a_HalfHeight; }

	};

	//=================================================================================================
	// Cylinder Collider Component: Adds a cylinder collider to an entity for physics simulation.
	// Performance Cost: Medium-High
	//=================================================================================================
	struct CylinderColliderComponent : Component
	{
	private:

		//=============================================================================================
		Vector3 m_Center = { 0.0f, 0.0f, 0.0f };
		Rotator m_Rotation = { 0.0f, 0.0f, 0.0f };
		float m_Radius = 0.5f;
		float m_HalfHeight = 0.5f;

	public:

		//=============================================================================================
		const Vector3& Center() const { return m_Center; }
		const Rotator& Rotation() const { return m_Rotation; }
		float Radius() const { return m_Radius; }
		float HalfHeight() const { return m_HalfHeight; }

		//=============================================================================================
		void SetCenter( const Vector3& a_Center ) { m_Center = a_Center; }
		void SetRotation( const Rotator& a_Rotation ) { m_Rotation = a_Rotation; }
		void SetRadius( float a_Radius ) { m_Radius = a_Radius; }
		void SetHalfHeight( float a_HalfHeight ) { m_HalfHeight = a_HalfHeight; }

	};

	//=================================================================================================
	// Mesh Collider Component: Adds a mesh collider to an entity for physics simulation.
	// Performance Cost: High
	//=================================================================================================
	struct MeshColliderComponent : Component
	{
	};

} // namespace Tridium

namespace Tridium::Meta {

	template<>
	struct Reflector<EPhysicsLayer>
	{
		using Type = Type<EPhysicsLayer>;

		Constant<EPhysicsLayer::Static, Scriptable,
		Tooltip<"Static physics layer. Rigid bodies in this layer should have their MotionType set to Static for better performance.">>
		Static;

		Constant<EPhysicsLayer::Dynamic, Scriptable,
		Tooltip<"Dynamic physics layer. Rigid bodies in this layer can move and interact with other dynamic bodies.">>
		Dynamic;

		Constant<EPhysicsLayer::Player, Scriptable,
		Tooltip<"Player physics layer. Rigid bodies in this layer are typically used for player-controlled entities.">>
		Player;
	};

	template<>
	struct Reflector<EMotionType>
	{
		using Type = Type<EMotionType>;

		Constant<EMotionType::Static, Scriptable,
		Tooltip<"Static motion type. Rigid bodies with this motion type do not move and are not affected by physics forces.">>
		Static;

		Constant<EMotionType::Kinematic, Scriptable,
		Tooltip<"Kinematic motion type. Rigid bodies with this motion type are moved via code and are not affected by physics forces.">>
		Kinematic;

		Constant<EMotionType::Dynamic, Scriptable,
		Tooltip<"Dynamic motion type. Rigid bodies with this motion type are affected by physics forces and can move freely.">>
		Dynamic;
	};

	template<>
	struct Reflector<RigidBodyComponent>
	{
		using Type = Type<RigidBodyComponent, RequireComponents<TransformComponent>, Icon<TE_ICON_SQUARE_ARROW_UP_RIGHT>>;

		using RB = RigidBodyComponent;

		Property<&RB::PhysicsLayer, &RB::SetPhysicsLayer, Scriptable, Editable, Serializable,
		Tooltip<"Defines what physics layer this rigid body belongs to. If a rigid body will never move, consider setting it to 'Static' for better performance.">>
		PhysicsLayer;

		Property<&RB::MotionType, &RB::SetMotionType, Scriptable, Editable, Serializable,
		Tooltip<"Defines how this rigid body moves. Static bodies do not move, Kinematic bodies are moved via code, Dynamic bodies are affected by physics.">>
		MotionType;

		Property<&RB::MassScale, &RB::SetMassScale, Scriptable, Editable, Serializable,
		Tooltip<"Scales the mass of the rigid body. A higher mass makes the body harder to move when impacted by forces.">>
		MassScale;

		Property<&RB::GravityScale, &RB::SetGravityScale, Scriptable, Editable, Serializable,
		Tooltip<"Scales the effect of gravity on this rigid body. A value of 0 means no gravity, 1 means normal gravity.">>
		GravityScale;

		Property<&RB::Restitution, &RB::SetRestitution, Scriptable, Editable, Serializable, Range<0.0f, 1.0f>,
		Tooltip<"Defines how bouncy this rigid body is. A value of 0 means no bounce, 1 means full bounce.">>
		Restitution;

		//Property<&RB::LinearMotionConstraint, nullptr, Scriptable, Editable, Serializable,
		//Tooltip<"Defines constraints on the linear motion of this rigid body along the X, Y, and Z axes.">>
		//LinearMotionConstraint;
		//
		//Property<&RB::AngularMotionConstraint, nullptr, Scriptable, Editable, Serializable,
		//Tooltip<"Defines constraints on the angular motion of this rigid body around the X, Y, and Z axes.">>
		//AngularMotionConstraint;

		Property<&RB::IsSleeping, nullptr, Scriptable, Visible,
		Tooltip<"Indicates whether the rigid body is currently sleeping (not being simulated).">>
		IsSleeping;
	};

	template<>
	struct Reflector<SphereColliderComponent>
	{
		using Type = Type<SphereColliderComponent, Icon<TE_ICON_CIRCLE>>;

		using SC = SphereColliderComponent;

		Property<&SC::Center, &SC::SetCenter, Scriptable, Editable, Serializable,
		Tooltip<"The local position of the sphere collider's center relative to the entity's transform.">>
		Center;

		//Property<&SC::Rotation, &SC::SetRotation, Scriptable, Editable, Serializable,
		//Tooltip<"The local rotation of the sphere collider relative to the entity's transform.">>
		//Rotation;

		Property<&SC::Radius, &SC::SetRadius, Scriptable, Editable, Serializable, Min<0.0f>,
		Tooltip<"The radius of the sphere collider.">>
		Radius;
	};

} // namespace Tridium::Meta