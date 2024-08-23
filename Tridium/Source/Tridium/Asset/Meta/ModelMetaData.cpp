#include "tripch.h"
#include "ModelMetaData.h"
#include "assimp/postprocess.h"
#include <Tridium/IO/SerializationUtil.h>

#define DEFAULT_POST_PROCESS_FLAGS aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GlobalScale

namespace Tridium {
	ModelMetaData::ModelMetaData()
		: AssetMetaData( EAssetType::Mesh )
	{
	}

	void ModelMetaData::OnSerialize( YAML::Emitter& a_Out )
	{
		a_Out << YAML::Key << "Import Settings" << YAML::BeginMap;

		a_Out << YAML::Key << "PostProcessFlags" << YAML::Value << ImportSettings.PostProcessFlags;
		a_Out << YAML::Key << "Scale" << YAML::Value << ImportSettings.Scale;
		a_Out << YAML::Key << "DiscardLocalData" << YAML::Value << ImportSettings.DiscardLocalData;

		a_Out << YAML::EndMap;
	}

	bool ModelMetaData::OnDeserialize( YAML::Node& a_Data )
	{
		auto importSettingsNode = a_Data["Import Settings"];
		if ( !importSettingsNode )
			return true;

		if ( auto node = importSettingsNode["PostProcessFlags"] )
			ImportSettings.PostProcessFlags = node.as<unsigned int>();

		if ( auto node = importSettingsNode["Scale"] )
			ImportSettings.Scale = node.as<float>();

		if ( auto node = importSettingsNode["DiscardLocalData"] )
			ImportSettings.Scale = node.as<bool>();

		return true;
	}

	ModelImportSettings::ModelImportSettings()
		: PostProcessFlags( DEFAULT_POST_PROCESS_FLAGS )
	{
	}

}