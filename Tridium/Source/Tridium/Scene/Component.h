#pragma once
#include <Tridium/Core/UUID.h>
#include <Tridium/ECS/Registry.h>
#include <Tridium/Math/Math.h>
#include <Tridium/Reflection/Meta.h>
#include <Tridium/Utils/TypeTraits.h>
#include <Tridium/Utils/Concepts.h>

namespace Tridium {

	//=================================================================================================
	// Scene Tick Groups:
	//=================================================================================================
	enum class ESceneTickGroup : uint8_t
	{
		// Will not be ticked.
		None = 0,

		// Ticking occurs before physics simulation.
		PrePhysics,

		// Ticking occurs after physics simulation.
		PostPhysics,

		// Ticking occurs before rendering work has been submitted.
		PreRender,

		// Ticking occurs during the rendering phase.
		// NOTE: Rendering is not done in this phase, this is for submitting rendering work only.
		Render,

		// Total number of tick groups.
		COUNT,

		Default = PrePhysics
	};

	namespace Concepts::Component {

		//=================================================================================================
		// Does the component have an OnBeginPlay function?
		template<typename T>
		concept HasOnBeginPlay = requires( T a_Component )
		{
			{ a_Component.OnBeginPlay() } -> SameAs<void>;
		};

		//=================================================================================================
		// Does the component have an OnUpdate function?
		template<typename T>
		concept HasOnUpdate = requires( T a_Component, float a_DeltaTime )
		{
			{ a_Component.OnUpdate( a_DeltaTime ) } -> SameAs<void>;
		};

		//=================================================================================================
		// Does the component have an OnEndPlay function?
		template<typename T>
		concept HasOnEndPlay = requires( T a_Component )
		{
			{ a_Component.OnEndPlay() } -> SameAs<void>;
		};

		//=================================================================================================
		// Does the component use lifecycle events?
		template<typename T>
		concept HasLifecycleEvents = HasOnBeginPlay<T> || HasOnUpdate<T> || HasOnEndPlay<T>;

	} // namespace Concepts::Component

	//=================================================================================================
	// Component Traits: Used to define metadata for components.
	// Specialize this struct for your component type to customize its behavior.
	//=================================================================================================
	template<typename T>
	struct ComponentTraits
	{
		static constexpr auto TickGroup = ESceneTickGroup::Default;
		static constexpr bool CreateComponentSystem = Concepts::Component::HasLifecycleEvents<T>;
	};

	//=================================================================================================
	// Base Component: All components should derive from this struct.
	// Components by default do not use virtual polymorphism.
	//=================================================================================================
	struct Component
	{
	};

	//=================================================================================================
	// UUID Component: Provides a unique identifier for a game object.
	//=================================================================================================
	struct UUIDComponent : Component
	{
		UUID ID = UUID::Generate();
	};

	//=================================================================================================
	// Name Component: Used to assign a name or tag to a game object.
	//=================================================================================================
	struct NameComponent : Component
	{
		String Name = "GameObject";
	};

	//=================================================================================================
	// Icon Component: Used to assign an icon to a game object in the editor.
	//=================================================================================================
	struct IconComponent : Component
	{
		String Icon;
	};

	//=================================================================================================
	// Transform Component: Stores the local position, rotation, and scale data for a game object.
	// Also handles hierarchy-based transformations.
	//=================================================================================================
	struct TransformComponent : Component
	{
	protected:

		//=============================================================================================
		Vector3 m_LocalPosition{};
		Quaternion m_LocalRotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		Vector3 m_LocalEulerAngles{ 0.0f };
		Vector3 m_LocalScale{ 1.0f };

		//=============================================================================================
		Entity m_Parent = NullEntity;
		Entity m_FirstChild = NullEntity;
		Entity m_PrevSibling = NullEntity;
		Entity m_NextSibling = NullEntity;

	public:

		//=============================================================================================
		TransformComponent() = default;
		TransformComponent( const TransformComponent& ) = default;
		TransformComponent( const Vector3& a_Position ) : m_LocalPosition( a_Position ) {}
		TransformComponent( const Matrix4& a_Transform )
		{
			Math::DecomposeTransform( a_Transform, m_LocalPosition, m_LocalRotation, m_LocalScale );
			m_LocalEulerAngles = Math::EulerAngles( m_LocalRotation );
		}

		//=============================================================================================
		const Vector3& LocalPosition() const { return m_LocalPosition; }
		void SetLocalPosition( const Vector3& a_Position ) { m_LocalPosition = a_Position; }

		//=============================================================================================
		const Vector3& LocalRotationEuler() const { return m_LocalEulerAngles; }
		void SetLocalRotationEuler( const Vector3& a_EulerAngles )
		{ 
			m_LocalEulerAngles = a_EulerAngles; 
			m_LocalRotation = Quaternion( a_EulerAngles );
		}

		//=============================================================================================
		Vector3 LocalRotationEulerDeg() const { return Math::Degrees( m_LocalEulerAngles ); }
		void SetLocalRotationEulerDeg( const Vector3& a_EulerAnglesDeg ) { SetLocalRotationEuler( Math::Radians( a_EulerAnglesDeg ) ); }

		//=============================================================================================
		Vector3 LocalForward() const { return ( m_LocalRotation * Vector3::Forward() ).Normalized(); }

		//=============================================================================================
		const Quaternion& LocalRotation() const { return m_LocalRotation; }
		void SetLocalRotation( const Quaternion& a_Rotation ) 
		{ 
			m_LocalRotation = a_Rotation; 
			m_LocalEulerAngles = Math::EulerAngles( a_Rotation );
		}

		//=============================================================================================
		const Vector3& LocalScale() const { return m_LocalScale; }
		void SetLocalScale( const Vector3& a_Scale ) { m_LocalScale = a_Scale; }

		//=============================================================================================
		Matrix4 LocalTransform() const
		{
			return Math::Translate( Matrix4( 1.0f ), m_LocalPosition )
				 * Math::ToMat4( m_LocalRotation )
				 * Math::Scale( Matrix4( 1.0f ), m_LocalScale );
		}

		//=============================================================================================
		Matrix4 WorldTransform( const EntityComponentRegistry& a_Registry, Entity a_Entity ) const
		{
			Matrix4 transform = LocalTransform();

			Entity parentEntity = m_Parent;
			const TransformComponent* parentTransform = nullptr;
			while ( parentEntity != NullEntity )
			{
				parentTransform = a_Registry.TryGet<TransformComponent>( parentEntity );
				ASSERT( parentTransform, "Transform has a Parent but the Parent does not have a Transform!" );
				if ( !parentTransform )
					break;

				transform = parentTransform->LocalTransform() * transform;
				parentEntity = parentTransform->Parent();
			}

			return transform;
		}

		//=============================================================================================
		Vector3 WorldPosition( const EntityComponentRegistry& a_Registry, Entity a_Entity ) const
		{
			Matrix4 worldTransform = WorldTransform( a_Registry, a_Entity );
			return Vector3( worldTransform[3].x, worldTransform[3].y, worldTransform[3].z );
		}

		//=============================================================================================
		Quaternion WorldRotation( const EntityComponentRegistry& a_Registry, Entity a_Entity ) const
		{
			Matrix4 worldTransform = WorldTransform( a_Registry, a_Entity );
			Vector3 scale;
			Vector3 position;
			Quaternion rotation;
			Math::DecomposeTransform( worldTransform, position, rotation, scale );
			return rotation;
		}

		//=============================================================================================
		Vector3 WorldForward( const EntityComponentRegistry& a_Registry, Entity a_Entity ) const
		{
			Quaternion worldRot = WorldRotation( a_Registry, a_Entity );
			return worldRot * Vector3::Forward();
		}

		//=============================================================================================
		Entity Parent() const { return m_Parent; }

		//=============================================================================================
		bool HasChildren() const { return m_FirstChild != NullEntity; }

		//=============================================================================================
		// Adds a child to this entity's hierarchy.
		void AddChild( EntityComponentRegistry& a_Registry, Entity a_Self, Entity a_Child )
		{
			ASSERT( a_Registry.AllOf<TransformComponent>( a_Self ), "Parent entity must have a HierarchyComponent." );

			TransformComponent& childHierarchy = a_Registry.GetOrEmplace<TransformComponent>( a_Child );

			// Detach from old parent if needed
			if ( childHierarchy.Parent() != NullEntity )
				DetachFromParent( a_Registry, a_Child );

			childHierarchy.m_Parent = a_Self;
			childHierarchy.m_PrevSibling = NullEntity;
			childHierarchy.m_NextSibling = NullEntity;

			if ( m_FirstChild == NullEntity )
			{
				// No children yet
				m_FirstChild = a_Child;
			}
			else
			{
				// Push to the front of the sibling list
				Entity sibling = m_FirstChild;
				auto& siblingHierarchy = a_Registry.Get<TransformComponent>( sibling );
				siblingHierarchy.m_PrevSibling = a_Child;
				childHierarchy.m_NextSibling = sibling;
				m_FirstChild = a_Child;
			}
		}

		//=============================================================================================
		// Removes this entity from its parent's hierarchy (but does NOT destroy the entity).
		static void DetachFromParent( EntityComponentRegistry& a_Registry, Entity a_Entity )
		{
			auto& self = a_Registry.Get<TransformComponent>( a_Entity );
			if ( self.Parent() == NullEntity )
				return;

			auto& parent = a_Registry.Get<TransformComponent>( self.Parent() );

			// If we are the first child
			if ( parent.m_FirstChild == a_Entity )
				parent.m_FirstChild = self.m_NextSibling;

			// Fix sibling links
			if ( self.m_PrevSibling != NullEntity )
				a_Registry.Get<TransformComponent>( self.m_PrevSibling ).m_NextSibling = self.m_NextSibling;

			if ( self.m_NextSibling != NullEntity )
				a_Registry.Get<TransformComponent>( self.m_NextSibling ).m_PrevSibling = self.m_PrevSibling;

			self.m_Parent = NullEntity;
			self.m_PrevSibling = NullEntity;
			self.m_NextSibling = NullEntity;
		}

		//=============================================================================================
		// Recursively destroys all children of this entity.
		static void DestroyChildren( EntityComponentRegistry& a_Registry, Entity a_Entity )
		{
			auto& hierarchy = a_Registry.Get<TransformComponent>( a_Entity );
			Entity child = hierarchy.m_FirstChild;

			while ( child != NullEntity )
			{
				Entity next = a_Registry.Get<TransformComponent>( child ).m_NextSibling;
				DestroyChildren( a_Registry, child ); // Recursive destroy
				a_Registry.Destroy( child );
				child = next;
			}

			hierarchy.m_FirstChild = NullEntity;
		}

		//=============================================================================================
		// Moves this entity under a new parent (detaches from old).
		void Reparent( EntityComponentRegistry& a_Registry, Entity a_Self, Entity a_NewParent )
		{
			if ( m_Parent == a_NewParent )
				return;

			DetachFromParent( a_Registry, a_Self );
			if ( a_NewParent != NullEntity )
				a_Registry.GetOrEmplace<TransformComponent>( a_NewParent ).AddChild( a_Registry, a_NewParent, a_Self );
		}

		//=============================================================================================
		// Iterates through all direct children.
		template<std::invocable<const EntityComponentRegistry&, Entity> Func>
		void ForEachChild( const EntityComponentRegistry& a_Registry, Func&& a_Func ) const
		{
			Entity child = m_FirstChild;
			while ( child != NullEntity )
			{
				a_Func( a_Registry, child );
				const auto& childHierarchy = a_Registry.Get<TransformComponent>( child );
				child = childHierarchy.m_NextSibling;
			}
		}

		//=============================================================================================
		// Recursively iterates all descendants.
		template<std::invocable<const EntityComponentRegistry&, Entity> Func>
		void ForEachDescendant( const EntityComponentRegistry& a_Registry, Func&& a_Func ) const
		{
			ForEachChild( a_Registry, []( const EntityComponentRegistry& a_Registry, Entity child )
			{
				a_Func( a_Registry, child );
				a_Registry.Get<TransformComponent>( child ).ForEachDescendant( a_Registry, a_Func );
			} );
		}
	};

} // namespace Tridium

namespace Tridium::Meta {

	//=================================================================================================
	// UUIDComponent
	//=================================================================================================
	template<>
	struct Reflector<UUIDComponent>
	{
		using Type = Type<UUIDComponent, HideInInspector>;
		Field<&UUIDComponent::ID, Visible, Serializable>
		ID;
	};

	//=================================================================================================
	// NameComponent
	//=================================================================================================
	template<>
	struct Reflector<NameComponent>
	{
		using Type = Type<NameComponent, HideInInspector>;

		Field<&NameComponent::Name, Editable, Serializable> 
		Name;
	};

	//=================================================================================================
	// IconComponent
	//=================================================================================================
	template<>
	struct Reflector<IconComponent>
	{
		using Type = Type<IconComponent, HideInInspector>;

		Field<&IconComponent::Icon, Serializable> 
		Icon;
	};

	//=================================================================================================
	// TransformComponent
	//=================================================================================================
	template<>
	struct Reflector<TransformComponent>
	{
		using Type = Type<TransformComponent, HideInInspector>;

		Property<&TransformComponent::LocalPosition, &TransformComponent::SetLocalPosition, Editable, Serializable, DisplayName<"Position">>
		LocalPosition;

		Property<&TransformComponent::LocalRotationEulerDeg, &TransformComponent::SetLocalRotationEulerDeg, Editable, Serializable, DisplayName<"Rotation">>
		LocalRotationEuler;

		Property<&TransformComponent::LocalScale, &TransformComponent::SetLocalScale, Editable, Serializable, DisplayName<"Scale">>
		LocalScale;

		// Functions

		Function<&TransformComponent::LocalTransform> 
		LocalTransform;
	};

} // namespace Tridium::Meta