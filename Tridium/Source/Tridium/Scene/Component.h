#pragma once
#include <Tridium/Core/UUID.h>
#include <Tridium/ECS/ECSFwd.h>
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

		// Ticking occurs after rendering work has been submitted.
		PostRender,

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
		Quaternion m_LocalRotation{};
		Vector3 m_LocalEulerAngles{};
		Vector3 m_LocalScale{ 1.0f };

	public:

		//=============================================================================================
		TransformComponent() = default;
		TransformComponent( const TransformComponent& ) = default;
		TransformComponent( const Vector3& a_Position ) : m_LocalPosition( a_Position ) {}

		//=============================================================================================
		const Vector3& LocalPosition() const { return m_LocalPosition; }
		void SetLocalPosition( const Vector3& a_Position ) { m_LocalPosition = a_Position; }

		//=============================================================================================
		const Vector3& LocalRotationEuler() const { return m_LocalEulerAngles; }
		void SetLocalRotationEuler( const Vector3& a_EulerAngles )
		{ 
			printf( std::format( "\n Setting Local Rotation Euler Angles to: X={} Y={} Z={}", a_EulerAngles.X, a_EulerAngles.Y, a_EulerAngles.Z ).c_str() );
			m_LocalEulerAngles = a_EulerAngles; 
			m_LocalRotation = Quaternion( a_EulerAngles );
		}

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

		Property<&TransformComponent::LocalRotationEuler, &TransformComponent::SetLocalRotationEuler, Editable, Serializable, DisplayName<"Rotation">>
		LocalRotationEuler;

		Property<&TransformComponent::LocalScale, &TransformComponent::SetLocalScale, Editable, Serializable, DisplayName<"Scale">>
		LocalScale;

		// Functions

		Function<&TransformComponent::LocalTransform> 
		LocalTransform;
	};

} // namespace Tridium::Meta