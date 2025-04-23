#pragma once
#if IS_EDITOR

#include "Panel.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/Core/Delegate.h>

namespace Tridium { class Scene; }

namespace Tridium {

	class SceneHeirarchyPanel final : public Panel
	{
	public:
		SceneHeirarchyPanel();
		virtual ~SceneHeirarchyPanel();

		virtual void OnImGuiDraw() override;

		GameObject GetSelectedGameObject() const { return m_SelectedGameObject; }
		void SetSelectedGameObject( GameObject gameObject );

	private:
		virtual bool OnKeyPressed( const KeyPressedEvent& e ) override;

		void DrawSceneHeirarchy();
		void OpenAddPopUp();
		void DrawAddPopUp( GameObject go = {} );
		void DrawSceneNode( GameObject go );

	private:
		GameObject m_SelectedGameObject;
		std::string m_SearchBuffer;

		DelegateHandle m_OnGameObjectSelectedHandle;
	};
}

#endif // IS_EDITOR
