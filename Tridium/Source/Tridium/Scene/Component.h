#pragma once
#include <Tridium/Utils/TypeTraits.h>
#include <Tridium/Utils/Concepts.h>
#include <Tridium/ECS/ECSFwd.h>

namespace Tridium {

	//=================================================================================================
	// Scene Tick Groups:
	//=================================================================================================
	enum class ESceneTickGroup : uint8_t
	{
		Unknown = 0,

		// Ticking occurs before physics simulation.
		PrePhysics,

		// Ticking occurs after physics simulation.
		PostPhysics,

		// Ticking occurs before rendering work has been submitted.
		PreRender,

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
	// GUID Component: Stores a globally unique identifier for a game object.
	//=================================================================================================
	struct GUIDComponent
	{
		GUID ID;
	};

	//=================================================================================================
	// Tag Component: Used to assign a name or tag to a game object.
	//=================================================================================================
	struct TagComponent
	{
		String Tag;
	};

	//=================================================================================================
	// Hierarchy Component: Manages parent-child relationships between game objects.
	//=================================================================================================
	struct HierarchyComponent
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
	struct TransformComponent
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