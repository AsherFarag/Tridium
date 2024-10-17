#pragma once

#ifdef IS_EDITOR

#include "Panel.h"
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Asset/AssetType.h>

namespace Tridium::Editor {

	enum class EFileType
	{
		Folder,
		Lua,
		Image,
		Tridium_Scene,
		Tridium_Project,
	};

	class ContentBrowserPanel final : public Panel
	{
	public:
		ContentBrowserPanel();
		virtual ~ContentBrowserPanel() = default;
		
		virtual void OnImGuiDraw() override;

	private:
		bool ContentItemOnImGuiDraw( const EFileType a_Type, const IO::FilePath& a_FilePath, const ImVec2& a_Size );
		void ContentOnOpened( const EFileType a_Type, const IO::FilePath& a_FilePath );

	private:
		IO::FilePath m_CurrentDirectory;

		SharedPtr<Texture> m_DefaultIcon;
		SharedPtr<Texture> m_FolderIcon;
		SharedPtr<Texture> m_LuaIcon;
		SharedPtr<Texture> m_ImageMediaIcon;
		SharedPtr<Texture> m_TridiumProjectIcon;
	};
}

#endif // IS_EDITOR