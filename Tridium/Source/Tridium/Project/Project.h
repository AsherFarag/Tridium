#pragma once
#include <Tridium/IO/FilePath.h>
#include <Tridium/Project/ProjectConfig.h>
#include <Tridium/Asset/AssetType.h>

namespace Tridium {

	struct Project
	{
		ProjectConfig Config;

		FilePath GetAssetDirectory() const { return Config.Editor.ProjectDirectory / Config.Editor.AssetDirectory; }

		Project() = default;
		Project( const String& a_Name, const FilePath& a_AssetDirectory, SceneHandle a_StartScene );
		~Project();
	};

} // namespace Tridium