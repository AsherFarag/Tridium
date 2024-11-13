#include "tripch.h"
#include "RigidBodyComponent.h"

namespace Tridium {
	RigidBodyComponent::~RigidBodyComponent()
	{
		Application::GetScene()->GetPhysicsScene().RemovePhysicsBody( BodyID );
	}
}