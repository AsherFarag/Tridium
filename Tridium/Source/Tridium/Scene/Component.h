#pragma once
#include <Tridium/Core/UUID.h>
#include <Tridium/ECS/ECS.h>
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
	// Hierarchy Component: Manages parent-child relationships between game objects.
	//=================================================================================================
	struct HierarchyComponent : Component
	{
		EntityID Parent = NullEntity;
		EntityID FirstChild = NullEntity;
		EntityID PrevSibling = NullEntity;
		EntityID NextSibling = NullEntity;

		//=============================================================================================
		// Adds a child to this entity's hierarchy.
		void AddChild( EntityComponentRegistry& a_Registry, EntityID a_Self, EntityID a_Child )
		{
			ASSERT( a_Registry.AllOf<HierarchyComponent>( a_Self ), "Parent entity must have a HierarchyComponent." );

			HierarchyComponent& childHierarchy = a_Registry.GetOrEmplace<HierarchyComponent>( a_Child );

			// Detach from old parent if needed
			if ( childHierarchy.Parent != NullEntity )
				DetachFromParent( a_Registry, a_Child );

			childHierarchy.Parent = a_Self;
			childHierarchy.PrevSibling = NullEntity;
			childHierarchy.NextSibling = NullEntity;

			if ( FirstChild == NullEntity )
			{
				// No children yet
				FirstChild = a_Child;
			}
			else
			{
				// Push to the front of the sibling list
				EntityID sibling = FirstChild;
				HierarchyComponent& siblingHierarchy = a_Registry.Get<HierarchyComponent>( sibling );
				siblingHierarchy.PrevSibling = a_Child;
				childHierarchy.NextSibling = sibling;
				FirstChild = a_Child;
			}
		}

		//=============================================================================================
		// Removes this entity from its parent's hierarchy (but does NOT destroy the entity).
		static void DetachFromParent( EntityComponentRegistry& a_Registry, EntityID a_Entity )
		{
			HierarchyComponent& self = a_Registry.Get<HierarchyComponent>( a_Entity );
			if ( self.Parent == NullEntity )
				return;

			HierarchyComponent& parent = a_Registry.Get<HierarchyComponent>( self.Parent );

			// If we are the first child
			if ( parent.FirstChild == a_Entity )
				parent.FirstChild = self.NextSibling;

			// Fix sibling links
			if ( self.PrevSibling != NullEntity )
				a_Registry.Get<HierarchyComponent>( self.PrevSibling ).NextSibling = self.NextSibling;

			if ( self.NextSibling != NullEntity )
				a_Registry.Get<HierarchyComponent>( self.NextSibling ).PrevSibling = self.PrevSibling;

			self.Parent = NullEntity;
			self.PrevSibling = NullEntity;
			self.NextSibling = NullEntity;
		}

		//=============================================================================================
		// Recursively destroys all children of this entity.
		static void DestroyChildren( EntityComponentRegistry& a_Registry, EntityID a_Entity )
		{
			HierarchyComponent& hierarchy = a_Registry.Get<HierarchyComponent>( a_Entity );
			EntityID child = hierarchy.FirstChild;

			while ( child != NullEntity )
			{
				EntityID next = a_Registry.Get<HierarchyComponent>( child ).NextSibling;
				DestroyChildren( a_Registry, child ); // Recursive destroy
				a_Registry.Destroy( child );
				child = next;
			}

			hierarchy.FirstChild = NullEntity;
		}

		//=============================================================================================
		// Moves this entity under a new parent (detaches from old).
		void Reparent( EntityComponentRegistry& a_Registry, EntityID a_Self, EntityID a_NewParent )
		{
			if ( Parent == a_NewParent )
				return;

			DetachFromParent( a_Registry, a_Self );
			if ( a_NewParent != NullEntity )
				a_Registry.GetOrEmplace<HierarchyComponent>( a_NewParent ).AddChild( a_Registry, a_NewParent, a_Self );
		}

		//=============================================================================================
		// Iterates through all direct children.
		template<typename Func>
		void ForEachChild( const EntityComponentRegistry& a_Registry, Func&& a_Func ) const
		{
			EntityID child = FirstChild;
			while ( child != NullEntity )
			{
				a_Func( child );
				const HierarchyComponent& childHierarchy = a_Registry.Get<HierarchyComponent>( child );
				child = childHierarchy.NextSibling;
			}
		}

		//=============================================================================================
		// Recursively iterates all descendants.
		template<typename Func>
		void ForEachDescendant( const EntityComponentRegistry& a_Registry, Func&& a_Func ) const
		{
			ForEachChild( a_Registry, [&]( EntityID child )
			{
				a_Func( child );
				a_Registry.Get<HierarchyComponent>( child ).ForEachDescendant( a_Registry, a_Func );
			} );
		}
	};

	//=================================================================================================
	// Transform Component: Stores the local position, rotation, and scale data for a game object.
	// This component does not handle parent-child relationships.
	//=================================================================================================
	struct TransformComponent : Component
	{
	private:

		//=============================================================================================
		Vector3 m_LocalPosition{};
		Quaternion m_LocalRotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		Vector3 m_LocalEulerAngles{};
		Vector3 m_LocalScale{ 1.0f };

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
		Matrix4 WorldTransform( const EntityComponentRegistry& a_Registry, EntityID a_Entity ) const
		{
			Matrix4 transform = LocalTransform();

			if ( const HierarchyComponent* hierarchy = a_Registry.TryGet<HierarchyComponent>( a_Entity ) )
			{
				EntityID parentEntity = hierarchy ? hierarchy->Parent : NullEntity;

				while ( parentEntity != NullEntity )
				{
					if ( const TransformComponent* parentTransform = a_Registry.TryGet<TransformComponent>( parentEntity ) )
					{
						transform = parentTransform->LocalTransform() * transform;
					}

					hierarchy = a_Registry.TryGet<HierarchyComponent>( parentEntity );
					parentEntity = hierarchy ? hierarchy->Parent : NullEntity;
				}
			}

			return transform;
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
	// HierarchyComponent
	//=================================================================================================
	template<>
	struct Reflector<HierarchyComponent>
	{
		using Type = Type<HierarchyComponent, HideInInspector>;

		Field<&HierarchyComponent::Parent, Serializable> 
		Parent;

		Field<&HierarchyComponent::FirstChild, Serializable> 
		FirstChild;

		Field<&HierarchyComponent::PrevSibling, Serializable> 
		PrevSibling;

		Field<&HierarchyComponent::NextSibling, Serializable> 
		NextSibling;
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