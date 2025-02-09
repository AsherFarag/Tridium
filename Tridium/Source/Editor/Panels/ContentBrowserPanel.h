#pragma once
#if IS_EDITOR

#include "Panel.h"
#include <Tridium/Graphics/Rendering/Texture.h>
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
		LuaScript,
		Folder,
	};

	const char* FileTypeToString( EFileType a_Type );

	class ContentBrowserPanel;

	class ContentItemIcons final
	{
	public:
		static std::unordered_map<EFileType, SharedPtr<Texture>> s_FileTypeIcons;
		static SharedPtr<Texture> s_UnimportedAssetIcon;
	};

	struct ContentItem
	{
		ContentBrowserPanel& Owner;
		EFileType Type;
		std::string Name;
		AssetHandle Handle{AssetHandle::InvalidID };
		bool IsImported = false;

		// Returns true if the item was opened.
		bool OnImGuiDraw( float a_Size ) const;
		bool IsAsset() const { return Type != EFileType::Folder && Type != EFileType::None; }
		bool operator ==( const ContentItem & a_Other ) const { return Name == a_Other.Name; }
	};

	class ContentBrowserPanel final : public Panel
	{
	public:
		using Directory = FilePath;
		using FolderHeirarchy = std::unordered_map<Directory, std::vector<ContentItem>>;

		ContentBrowserPanel();
		virtual ~ContentBrowserPanel() = default;
		
		virtual void OnImGuiDraw() override;
		void OpenFolder( const FilePath& a_Path );
		const FilePath& GetDirectory() const { return m_CurrentDirectory; }

	protected:
		void ReconstructDirectoryStack();
		void ReconstructFolderHierarchy();
		void RecurseFolderHeirarchy( const FilePath& a_Directory );

		void DrawFolderHierarchy();
		void RecurseDrawFolderHierarchy( const FilePath& a_Directory );
		void DrawContentBrowserHeader();
		void DrawContentItems();
		bool DrawContentItemContextMenu( const ContentItem& a_Item ); // Returns true if the ContentItems Draw Loop should break.
		void DrawContentBrowserSettings();

		// Returns true if the ContentItems Draw Loop should break.
		bool OnContentItemOpened( const ContentItem& a_Item );

	private:
		FilePath m_CurrentDirectory;
		std::string m_DirectoryInputBuffer;
		std::string m_ContentSearchFilter;
		std::list<std::string> m_DirectoryStack;
		FolderHeirarchy m_FolderHeirarchy;

		float m_ContentThumbnailSize = 1.0f;
		float m_RefreshTimer = 0.0f;
		const float m_RefreshTime = 1.0f;

		//std::mutex m_Mutex;
		//std::atomic<bool> m_IsReconstructingFolderHeirarchy = false;
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