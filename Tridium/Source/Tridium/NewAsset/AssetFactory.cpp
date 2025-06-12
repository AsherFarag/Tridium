#include "tripch.h"
#include "AssetFactory.h"

namespace Tridium::T {

	Array<CreateAssetLoader> AssetFactory::s_AssetLoaderInitializers;

	class TextureLoader : public IAssetLoader
	{
		// Inherited via IAssetLoader
		EAssetType GetAssetType() const override
		{
			return EAssetType();
		}
		SharedPtr<IAsset> CreateAsset( AssetID a_ID, EAssetFlags a_Flags ) override
		{
			return SharedPtr<IAsset>();
		}
		Expected<void, String> Save( const SharedPtr<IAsset>& a_Asset, const AssetMetadata& a_Metadata ) override
		{
			return Expected<void, String>();
		}
		Expected<void, String> Load( SharedPtr<IAsset> a_Asset, const AssetMetadata& a_Metadata ) override
		{
			return Expected<void, String>();
		}
	};

	REGISTER_ASSET_LOADER( TextureLoader );
}