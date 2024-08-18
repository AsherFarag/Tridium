#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Core/Core.h>

namespace YAML {
	class Node;
	class Emitter;
}

namespace Tridium {

	constexpr unsigned char FILE_FORMAT_VERSION = 0u;

	struct AssetMetaData
	{
		unsigned char FileFormatVersion = FILE_FORMAT_VERSION;
		AssetHandle Handle;
		EAssetType AssetType;

		void Serialize( const fs::path& a_Path );
		static AssetMetaData* Deserialize( const fs::path& a_Path );

	protected:
		virtual void OnSerialize( YAML::Emitter& a_Out ) {}
		virtual bool OnDeserialize( YAML::Node& a_Data ) { return true; }
	};

}