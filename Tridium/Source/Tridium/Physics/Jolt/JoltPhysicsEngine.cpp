#include "tripch.h"
#include "JoltPhysicsEngine.h"
#include "JoltPhysicsScene.h"
#include "JoltDebugRenderer.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

namespace Tridium {

	void JoltPhysicsEngine::Init()
	{
		// Register allocation hook.
		JPH::RegisterDefaultAllocator();

		// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
		// NOTE: Raw new/delete is intentional here - this follows Jolt Physics library's singleton pattern
		// The instance is owned by the library and must be deleted in Shutdown()
		JPH::Factory::sInstance = new JPH::Factory();

		JPH::RegisterTypes();

	#if USE_DEBUG_RENDERER

		// NOTE: Raw new/delete is intentional here - this follows Jolt Physics library's singleton pattern
		// The instance is owned by the library and must be deleted in Shutdown()
		JPH::DebugRenderer::sInstance = new JoltDebugRenderer();

	#endif // USE_DEBUG_RENDERER

	}

	void JoltPhysicsEngine::Shutdown()
	{	
		// Unregisters all types with the factory and cleans up the default material
		JPH::UnregisterTypes();

		// Delete the factory
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;

	#if USE_DEBUG_RENDERER
		delete JPH::DebugRenderer::sInstance;
		JPH::DebugRenderer::sInstance = nullptr;
	#endif // USE_DEBUG_RENDERER
	}

} // namespace Tridium