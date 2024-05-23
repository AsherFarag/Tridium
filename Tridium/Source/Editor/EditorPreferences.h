#pragma once
#ifdef IS_EDITOR

#include <Tridium/Core/Layer.h>

namespace Tridium::Editor {

    class EditorPreferences : public Layer
    {
	public:
		EditorPreferences( const std::string& name = "Editor Preferences" );
		virtual ~EditorPreferences() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnImGuiDraw() override;
		virtual void OnEvent( Event& e ) override;
    };

}

#endif // IS_EDITOR