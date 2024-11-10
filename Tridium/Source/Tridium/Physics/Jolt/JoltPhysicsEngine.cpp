#include "tripch.h"
#include "JoltPhysicsEngine.h"
#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

namespace Tridium {

	void JoltPhysicsEngine::Init()
	{
		// Register allocation hook.
		JPH::RegisterDefaultAllocator();

		// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
		JPH::Factory::sInstance = new JPH::Factory();

		JPH::RegisterTypes();
	}

	void JoltPhysicsEngine::Shutdown()
	{	
		// Unregisters all types with the factory and cleans up the default material
		JPH::UnregisterTypes();

		// Delete the factory
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}

} // namespace Tridium