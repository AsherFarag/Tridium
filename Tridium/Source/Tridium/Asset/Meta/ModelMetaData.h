#pragma once
#include "AssetMetaData.h"

namespace Tridium {

	struct ModelImportSettings
	{
		ModelImportSettings();

		unsigned int PostProcessFlags;
		float Scale = 1.f;
		bool DiscardLocalData = false; /* If true, once the mesh has been loaded onto the GPU, the local Vertex Data will be deleted. */
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