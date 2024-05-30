#pragma once

#ifdef IS_EDITOR

#include "Panel.h"

#include <Tridium/Core/Project.h>
#include <Tridium/Rendering/Texture.h>

namespace Tridium::Editor {

	enum class ContentType : BYTE
	{
		None = 0, Folder, Lua, Texture
	};

	class ContentBrowser final : public Panel
	{
	public:
		ContentBrowser();
		virtual ~ContentBrowser() = default;

		virtual void OnImGuiDraw() override;

	private:
		static ContentType GetContentType( const fs::path& a_FilePath );
		bool ContentItemOnImGuiDraw( const ContentType a_Type, const fs::path& a_FilePath, const ImVec2& a_Size );
		void ContentOnClicked( const ContentType a_Type, const fs::path& a_FilePath );

	private:
		fs::path m_CurrentDirectory;

		Ref<Texture2D> m_DefaultIcon;
		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_LuaIcon;
		Ref<Texture2D> m_TextureIcon;
	};
}

#endif // IS_EDITOR