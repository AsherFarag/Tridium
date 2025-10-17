#pragma once
#include "SceneEvent.h"

namespace Tridium {

	// Forward declarations
	class OldScene;
	// -------------------

	class OldISceneSystem
	{
	public:
		virtual ~OldISceneSystem() = default;

		// Called after a scene is loaded just before BeginPlay is called.
		virtual void Init() {}

		// Called after a scene is unloaded just before the scene and all GameObjects are destroyed.
		virtual void Shutdown() {}

		// Called when a scene event is triggered.
		virtual void OnSceneEvent( const SceneEventPayload& a_Event ) {}

		// Get the scene that owns this system.
		OldScene* GetOwningScene() const { return m_Scene; }

	private:
		OldScene* m_Scene = nullptr;
		friend class OldScene;
	};

}