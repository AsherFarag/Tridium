#pragma once

#ifdef IS_EDITOR

#include "Panel.h"
#include <Tridium/Rendering/Texture.h>

namespace Tridium::Editor {

	enum class ContentType
	{
		None = 0, Folder, Lua, Texture, Tridium_Project
	};

	class ContentBrowserPanel final : public Panel
	{
	public:
		ContentBrowserPanel();
		virtual ~ContentBrowserPanel() = default;
		
		virtual void OnImGuiDraw() override;

	private:
		static ContentType GetContentType( const fs::path& a_FilePath );
		bool ContentItemOnImGuiDraw( const ContentType a_Type, const fs::path& a_FilePath, const ImVec2& a_Size );
		void ContentOnOpened( const ContentType a_Type, const fs::path& a_FilePath );

	private:
		fs::path m_CurrentDirectory;

		Ref<Texture> m_DefaultIcon;
		Ref<Texture> m_FolderIcon;
		Ref<Texture> m_LuaIcon;
		Ref<Texture> m_ImageMediaIcon;
		Ref<Texture> m_TridiumProjectIcon;
	};
}

#endif // IS_EDITOR