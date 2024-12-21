#pragma once
#ifdef IS_EDITOR
#include <Tridium/Math/Math.h>
#include <Tridium/Asset/AssetType.h>
#include <Tridium/Core/Memory.h>
#include <unordered_map>

#include <string>

namespace Tridium {

	class Texture;
	
	namespace Editor {

		// Struct containing Editor specific information about an asset type.
		// Typically information that is used to display the asset in the Editor.
		struct AssetTypeInfo
		{
			EAssetType Type;
			std::string DisplayName;
			Vector4 Color;
			SharedPtr<Texture> Icon;
		};

		// Class containing information about all asset types.
		class AssetTypeManager
		{
		public:
			static AssetTypeManager& Get() { return *s_Instance; }
			static const AssetTypeInfo& GetAssetTypeInfo( EAssetType type );

		private:
			void Initialize();

			std::unordered_map<EAssetType, AssetTypeInfo> m_AssetTypeInfoMap;
			static UniquePtr<AssetTypeManager> s_Instance;
			friend class EditorApplication;
		};
	}

}
#endif // IS_EDITOR