#pragma once
#include <Tridium/Scene/Scene.h>

namespace Tridium {

	//=================================================================================================
	// RendererSceneSystem: Scene system that handles rendering the scene.
	//=================================================================================================
	class RendererSceneSystem : public ISceneSystem
	{
	public:

		//=============================================================================================
		// This class is ticked manually by the Scene.
		ESceneTickGroup GetTickGroup() const override { return ESceneTickGroup::Render; }

	protected:

		//=============================================================================================
		void OnUpdate( float a_DeltaTime ) override;

	};

} // namespace Tridium