#pragma once

#ifdef IS_EDITOR

#include "Panel.h"
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Asset/AssetType.h>

namespace Tridium::Editor {

	class ContentBrowserPanel final : public Panel
	{
	public:
		ContentBrowserPanel();
		virtual ~ContentBrowserPanel() = default;
		
		virtual void OnImGuiDraw() override;

	private:
		bool ContentItemOnImGuiDraw( const EAssetType a_Type, const IO::FilePath& a_FilePath, const ImVec2& a_Size );
		void ContentOnOpened( const EAssetType a_Type, const IO::FilePath& a_FilePath );

	private:
		IO::FilePath m_CurrentDirectory;

		AssetRef<Texture> m_DefaultIcon;
		AssetRef<Texture> m_FolderIcon;
		AssetRef<Texture> m_LuaIcon;
		AssetRef<Texture> m_ImageMediaIcon;
		AssetRef<Texture> m_TridiumProjectIcon;
	};
}

#endif // IS_EDITOR