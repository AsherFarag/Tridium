#pragma once
#ifdef IS_EDITOR
#include <Tridium/Utils/Singleton.h>
#include <Tridium/Math/Math.h>
#include <Tridium/Asset/AssetType.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Core/Containers/Containers.h>

namespace Tridium {

	class Texture;

	// Struct containing Editor specific information about an asset type.
	// Typically information that is used to display the asset in the Editor.
	struct AssetTypeInfo
	{
		EAssetType Type;
		String DisplayName;
		Vector4 Color;
		SharedPtr<Texture> Icon;
	};

	// Class containing information about all asset types.
	class AssetTypeManager final : public ISingleton<AssetTypeManager, /* _ExplicitSetup */ false, /* _IsOwning */ true, /* _IsThreadSafe */ true>
	{
	public:
		static const AssetTypeInfo& GetAssetTypeInfo( EAssetType type );

	private:
		virtual void OnSingletonConstructed() override { Init(); }
		void Init();

		UnorderedMap<EAssetType, AssetTypeInfo> m_AssetTypeInfoMap;
	};

}
#endif // IS_EDITOR