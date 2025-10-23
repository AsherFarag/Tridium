#pragma once
#if IS_EDITOR

#include "Panel.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/Common/Function.h>

namespace Tridium { class OldScene; }

namespace Tridium {

	class SceneHeirarchyPanel final : public Panel
	{
	public:
		SceneHeirarchyPanel();
		virtual ~SceneHeirarchyPanel();

		virtual void OnImGuiDraw() override;

	private:
		virtual bool OnKeyPressed( const KeyPressedEvent& e ) override { return false; }

		void DrawSceneHeirarchy() {};
		void OpenAddPopUp() {};

	};
}

#endif // IS_EDITOR
