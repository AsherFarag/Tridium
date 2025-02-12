#pragma once
#include <Tridium/Core/Config.h>
#include <Tridium/Containers/String.h>
#include <Tridium/IO/FilePath.h>

namespace Tridium {

	struct ProjectConfig
	{
		String Name = "Untitled";
		SceneHandle StartScene;

	#if WITH_EDITOR || 1 // TEMP
		struct Editor
		{
			String ProjectName;
			FilePath ProjectDirectory{};
			FilePath AssetDirectory{ "Content" }; // Relative to ProjectDirectory
		} Editor;
	#endif // WITH_EDITOR
	};


}