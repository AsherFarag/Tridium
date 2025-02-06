#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Utils/Singleton.h>
#include "EngineModule.h"
#include "EngineSubsystem.h"
#include <Tridium/Scene/SceneManager.h>

namespace Tridium {


	//================================================================
	// Engine
	//  
	class Engine final : public ISingleton<Engine>
	{
	public:

		// ===== Subsystems =====
		EngineSubsystemManager& GetSubsystemManager() { return m_SubsystemManager; }

		template<typename T>
		bool HasSubsystem() const { return m_SubsystemManager.HasSubsystem<T>(); }

		template<typename T>
		T* GetSubsystem() const { return m_SubsystemManager.GetSubsystem<T>(); }
		// ======================

	private:
		bool Initialize();
		void Shutdown();

		// ===== Subsystems =====
		void InitSubsystems();
		void ShutdownSubsystems();
		void RegisterSubsystems();
		// ======================

	private:
		EngineSubsystemManager m_SubsystemManager;

	private:
		friend class Application;
	};

}