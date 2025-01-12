#pragma once
#include <string>
#include <optional>
#include <unordered_map>

namespace Tridium {

	enum class ESubsystemStatusType
	{
		Success, // Default status, no error
		Failure, // General failure, the engine will continue to run
		FatalError, // Fatal error, the engine will shut down
	};

	struct SubsystemStatus
	{
		ESubsystemStatusType Status;
		std::string Message;

		SubsystemStatus( ESubsystemStatusType a_Status = ESubsystemStatusType::Success, const std::string& a_Message = "" )
			: Status( a_Status ), Message( a_Message ) {}
	};

	class IEngineSubsystem
	{
	public:
		using Dependencies = std::unordered_set<size_t>;

		virtual SubsystemStatus Initialize() { return {}; }
		virtual SubsystemStatus Shutdown() { return {}; }
		virtual const char* GetName() const = 0;
		virtual const Dependencies& GetDependencies() const { static Dependencies s_Dependencies{}; return s_Dependencies; }
	};

	class EngineSubsystemManager
	{
	public:
		using SubsystemStorage = std::unordered_map<size_t, UniquePtr<IEngineSubsystem>>;

		const SubsystemStorage& GetSubsystems() const { return m_Subsystems; }

		template<typename T>
		bool HasSubsystem() const;

		template<typename T>
		T* GetSubsystem() const;

		// Will attempt to construct the subsystem if it doesn't exist and add it to the storage.
		// If the subsystem already exists, it will return the existing subsystem.
		template<typename T, typename... Args>
		T* RegisterSubsystem( Args&&... args );

	private:
		SubsystemStorage  m_Subsystems;
	};

	template<typename T>
	bool EngineSubsystemManager::HasSubsystem() const
	{
		static_assert( std::is_base_of_v<IEngineSubsystem, T>, "T must derive from IEngineSubsystem" );

		static size_t s_Hash = typeid( T ).hash_code();
		return m_Subsystems.find( s_Hash ) != m_Subsystems.end();
	}

	template<typename T>
	T* EngineSubsystemManager::GetSubsystem() const
	{
		static_assert( std::is_base_of_v<IEngineSubsystem, T>, "T must derive from IEngineSubsystem" );

		static size_t s_Hash = typeid( T ).hash_code();
		auto it = m_Subsystems.find( s_Hash );
		if ( it != m_Subsystems.end() )
		{
			return static_cast<T*>( it->second.get() );
		}
		return nullptr;
	}

	template<typename T, typename... Args>
	T* EngineSubsystemManager::RegisterSubsystem( Args&&... args )
	{
		static_assert( std::is_base_of<IEngineSubsystem, T>::value, "T must derive from IEngineSubsystem" );

		static size_t s_Hash = typeid( T ).hash_code();

		// Check if the subsystem is already registered
		if ( auto it = m_Subsystems.find( s_Hash ); it != m_Subsystems.end() )
		{
			return static_cast<T*>( it->second.get() );
		}

		// Create the subsystem and add it to the storage
		UniquePtr<T> subsystem = MakeUnique<T>( std::forward<Args>( args )... );
		T* ptr = subsystem.get();
		m_Subsystems[s_Hash] = std::move( subsystem );
		return ptr;
	}

}