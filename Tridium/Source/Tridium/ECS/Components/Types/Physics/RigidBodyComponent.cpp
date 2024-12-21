#include "tripch.h"
#include "RigidBodyComponent.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {
	BEGIN_REFLECT_COMPONENT( RigidBodyComponent, Scriptable )
		BASE( Component )
		PROPERTY( m_BodyProxy )
		PROPERTY( m_PhysicsLayer, EditAnywhere | Serialize )
		PROPERTY( m_MotionType, EditAnywhere | Serialize )
		PROPERTY( m_Mass, EditAnywhere | Serialize )
		PROPERTY( m_GravityScale, EditAnywhere | Serialize )
		PROPERTY( m_Restitution, EditAnywhere | Serialize )
		PROPERTY( m_LinearMotionConstraint, EditAnywhere | Serialize )
		PROPERTY( m_AngularMotionConstraint, EditAnywhere | Serialize )
	END_REFLECT( RigidBodyComponent )

	RigidBodyComponent::RigidBodyComponent()
	{
	}

	RigidBodyComponent::RigidBodyComponent( const RigidBodyComponent& a_Other )
		: m_PhysicsLayer( a_Other.m_PhysicsLayer )
		, m_MotionType( a_Other.m_MotionType )
		, m_Mass( a_Other.m_Mass )
		, m_GravityScale( a_Other.m_GravityScale )
		, m_Restitution( a_Other.m_Restitution )
		, m_LinearMotionConstraint( a_Other.m_LinearMotionConstraint )
		, m_AngularMotionConstraint( a_Other.m_AngularMotionConstraint )
	{
	}

	RigidBodyComponent::RigidBodyComponent( RigidBodyComponent&& a_Other ) noexcept
		: m_BodyProxy( std::move( a_Other.m_BodyProxy ) )
		, m_PhysicsLayer( a_Other.m_PhysicsLayer )
		, m_MotionType( a_Other.m_MotionType )
		, m_Mass( a_Other.m_Mass )
		, m_GravityScale( a_Other.m_GravityScale )
		, m_Restitution( a_Other.m_Restitution )
		, m_LinearMotionConstraint( std::move( a_Other.m_LinearMotionConstraint ) )
		, m_AngularMotionConstraint( std::move( a_Other.m_AngularMotionConstraint ) )
	{
	}

	RigidBodyComponent& RigidBodyComponent::operator=( const RigidBodyComponent& a_Other )
	{
		m_BodyProxy = a_Other.m_BodyProxy;
		m_PhysicsLayer = a_Other.m_PhysicsLayer;
		m_MotionType = a_Other.m_MotionType;
		m_Mass = a_Other.m_Mass;
		m_GravityScale = a_Other.m_GravityScale;
		m_Restitution = a_Other.m_Restitution;
		m_LinearMotionConstraint = a_Other.m_LinearMotionConstraint;
		m_AngularMotionConstraint = a_Other.m_AngularMotionConstraint;

		return *this;
	}
}