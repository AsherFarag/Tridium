#pragma once

#ifdef IS_EDITOR

#include "Panel.h"
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Asset/AssetType.h>

namespace Tridium::Editor {

	enum class EFileType : uint8_t
	{
		None = 0,
		Scene,
		Material,
		MeshSource,
		StaticMesh,
		Shader,
		Texture,
		CubeMap,
		Lua,
		Folder,
	};

	class ContentBrowserPanel;

	struct ContentItem
	{
		static std::unordered_map<EFileType, SharedPtr<Texture>> s_Icons;

		ContentBrowserPanel& Owner;
		EFileType Type;
		std::string Name;

		ContentItem( ContentBrowserPanel& a_Owner, const EFileType a_Type, const IO::FilePath& a_Name )
			: Owner(a_Owner), Type( a_Type ), Name( std::move( a_Name.ToString() ) ) {}

		// Returns true if the item was opened.
		bool OnImGuiDraw( const ImVec2& a_Size ) const;

		bool operator ==( const ContentItem & a_Other ) const { return Name == a_Other.Name; }
	};

	class ContentBrowserPanel final : public Panel
	{
	public:
		using Directory = IO::FilePath;
		using FolderHeirarchy = std::unordered_map<Directory, std::vector<ContentItem>>;

		ContentBrowserPanel();
		virtual ~ContentBrowserPanel() = default;
		
		virtual void OnImGuiDraw() override;
		void OpenFolder( const IO::FilePath& a_Path );
		const IO::FilePath& GetDirectory() const { return m_CurrentDirectory; }

	protected:
		void ReconstructDirectoryStack();
		void ReconstructFolderHierarchy();
		void RecurseFolderHeirarchy( const IO::FilePath& a_Directory );

		void DrawFolderHierarchy();
		void RecurseDrawFolderHierarchy( const IO::FilePath& a_Directory );
		void DrawContentBrowserHeader();
		void DrawContentItems();
		bool DrawContentItemContextMenu( const ContentItem& a_Item ); // Returns true if the ContentItems Draw Loop should break.
		void DrawContentBrowserSettings();

		// Returns true if the ContentItems Draw Loop should break.
		bool OnContentItemOpened( const ContentItem& a_Item );

	private:
		IO::FilePath m_CurrentDirectory;
		std::string m_DirectoryInputBuffer;
		std::string m_ContentSearchFilter;
		std::list<std::string> m_DirectoryStack;
		FolderHeirarchy m_FolderHeirarchy;

		float m_ContentThumbnailSize = 128.0f;
	};
}

namespace std {
	template<>
	struct hash<Tridium::Editor::ContentItem>
	{
		size_t operator()( const Tridium::Editor::ContentItem& a_ContentItem ) const noexcept
		{
			return std::hash<std::string>{}( a_ContentItem.Name );
		}
	};
}

#endif // IS_EDITOR