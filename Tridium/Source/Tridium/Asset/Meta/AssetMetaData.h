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
		uint32_t FileFormatVersion = FILE_FORMAT_VERSION;
		AssetHandle Handle;
		EAssetType AssetType;

		AssetMetaData() = default;
		AssetMetaData( EAssetType a_AssetType ) : Handle(AssetHandle::Create()), AssetType( a_AssetType ) { }

		void Serialize( const IO::FilePath& a_Path );
		static AssetMetaData* Deserialize( const IO::FilePath& a_Path );

	protected:
		virtual void OnSerialize( YAML::Emitter& a_Out ) {}
		virtual bool OnDeserialize( YAML::Node& a_Data ) { return true; }
	};

}