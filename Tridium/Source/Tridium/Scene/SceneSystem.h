#pragma once
#include "SceneEvent.h"

namespace Tridium {

	class ISceneSystem
	{
	public:
		virtual ~ISceneSystem() = default;

		// Called after a scene is loaded just before BeginPlay is called.
		virtual void Init() {}

		// Called after a scene is unloaded just before the scene and all GameObjects are destroyed.
		virtual void Shutdown() {}

		virtual void OnSceneEvent( const SceneEventPayload& a_Event ) {}

	protected:
		class Scene* m_Scene = nullptr;
		friend class Scene;
	};

}