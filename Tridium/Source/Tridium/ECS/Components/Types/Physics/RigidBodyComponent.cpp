#include "tripch.h"
#include "RigidBodyComponent.h"

namespace Tridium {
	RigidBodyComponent::RigidBodyComponent( const RigidBodyComponent& a_Other )
	{
		m_MotionType = a_Other.m_MotionType;
		m_Mass = a_Other.m_Mass;
		m_GravityScale = a_Other.m_GravityScale;
	}
}