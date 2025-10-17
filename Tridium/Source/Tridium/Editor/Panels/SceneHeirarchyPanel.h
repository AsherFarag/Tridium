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

		OldGameObject GetSelectedGameObject() const { return m_SelectedGameObject; }
		void SetSelectedGameObject( OldGameObject gameObject );

	private:
		virtual bool OnKeyPressed( const KeyPressedEvent& e ) override;

		void DrawSceneHeirarchy();
		void OpenAddPopUp();
		void DrawAddPopUp( OldGameObject go = {} );
		void DrawSceneNode( OldGameObject go );

	private:
		OldGameObject m_SelectedGameObject;
		std::string m_SearchBuffer;

		DelegateHandle m_OnGameObjectSelectedHandle;
	};
}

#endif // IS_EDITOR
