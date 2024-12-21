#pragma once

namespace Tridium {

	enum class EModuleInitializationStage
	{
		Default = 0, // Called after all other stages
		PreEngineInit,
		EngineInit,
		PostEngineInit
	};

	class IEngineModule
	{
	public:
		virtual void Initialize() {}
		virtual void Shutdown() {}
		virtual EModuleInitializationStage GetInitializationStage() const = 0;
	};

}