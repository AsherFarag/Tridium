#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Core/Core.h>
#include "ImportSettings.h"

namespace YAML {
	class Node;
	class Emitter;
}

namespace Tridium {

	typedef Byte FileFormat;
	constexpr FileFormat FILE_FORMAT_VERSION = 0u;

	struct AssetMetaData
	{
		FileFormat FileFormatVersion = FILE_FORMAT_VERSION;
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

	struct ShaderMetaData : public AssetMetaData
	{
		ShaderMetaData() : AssetMetaData( EAssetType::Shader ) {}
	};

	struct SceneMetaData : public AssetMetaData
	{
		SceneMetaData() : AssetMetaData( EAssetType::Scene ) {}
	};

	struct TextureMetaData : public AssetMetaData
	{
		TextureMetaData() : AssetMetaData( EAssetType::Texture ) {}
	};

	struct MaterialMetaData : public AssetMetaData
	{
		MaterialMetaData() : AssetMetaData( EAssetType::Material ) {}
	};

	struct ModelMetaData : public AssetMetaData
	{
		ModelMetaData();
		ModelImportSettings ImportSettings;

	protected:
		virtual void OnSerialize( YAML::Emitter& a_Out );
		virtual bool OnDeserialize( YAML::Node& a_Data );
	};

}