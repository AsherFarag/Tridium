#pragma once

#ifdef IS_EDITOR
#include "imgui.h"
#include <Tridium/Events/Eventsfwd.h>

namespace Tridium::Editor {

	class PanelStack;

	class Panel
	{
		friend PanelStack;
	public:
		Panel( const std::string& a_Name ) : m_Name( a_Name ) {}
		virtual ~Panel() = default;

		virtual void OnEvent( Event& e ) {}
		virtual void OnImGuiDraw() = 0;

		bool IsFocused() const { return m_IsFocused; }

	protected:
		bool ImGuiBegin(ImGuiWindowFlags a_WindowFlags = 0);
		void ImGuiEnd(); // If used, it MUST be the last call in OnImGuiDraw()

		void Close();
		// Return false if this Panel should not be closed
		virtual bool OnClose() { return true; }

	protected:
		PanelStack* m_Owner = nullptr;
		bool m_Open = true;
		bool m_IsFocused = false;
		std::string m_Name;
	};

	// Wrapper for a list of panels
	class PanelStack
	{
	public:
		~PanelStack();

		// Removes the panel from the stack and returns it.
		// Returns nullptr if the panel was not in the stack!
		Panel* PullPanel( Panel* panel );

		// Returns true if the panel was successfully removed and deleted
		bool DestroyPanel( Panel* panel );

		template <typename T, typename... Args>
		T* PushPanel( Args&&... args )
		{
			static_assert( std::is_base_of_v<Panel, T>, "T must be derived from Panel!" );
			auto typeHash = typeid( T ).hash_code();

			// If panel already exists
			if ( auto it = m_Panels.find( typeHash ); it != m_Panels.end() )
			{
				return static_cast<T*>( it->second );
			}

			T* newPanel = new T( std::forward<Args>( args )... );
			newPanel->m_Owner = this;
			m_Panels.insert( { typeHash, newPanel } );
			return newPanel;
		}

		// Removes the panel from the stack and returns it.
		// Returns nullptr if the panel was not in the stack!
		template <typename T>
		T* PullPanel()
		{
			StaticAssert< constexpr( std::is_base_of_v( T, Panel ) ) >::Assert();
			auto typeHash = typeid( T ).hash_code();

			// If panel exists
			if ( auto it = m_Panels.find( typeHash ); it != m_Panels.end() )
			{
				return static_cast<T*>( it->second );
			}

			TE_CORE_WARN( "Attempted to pull non-existent panel!" );
			return nullptr;
		}

		// Returns true if the panel was successfully removed and deleted
		template <typename T>
		bool DestroyPanel()
		{
			T* panel = PullPanel<T>();
			if ( panel == nullptr )
			{
				return false;
			}

			delete panel;
			return true;
		}

		template <typename T>
		T* GetPanel()
		{
			static_assert( std::is_base_of_v<Panel, T>, "T must be derived from Panel!" );
			auto typeHash = typeid( T ).hash_code();

			// If panel exists
			if ( auto it = m_Panels.find( typeHash ); it != m_Panels.end() )
			{
				return static_cast<T*>( it->second );
			}

			return nullptr;
		}

		template <typename T>
		bool HasPanel()
		{
			static_assert( std::is_base_of_v<Panel, T>, "T must be derived from Panel!" );
			auto typeHash = typeid( T ).hash_code();

			// If panel exists
			if ( auto it = m_Panels.find( typeHash ); it != m_Panels.end() )
			{
				return true;
			}

			return false;
		}

		inline auto begin() { return m_Panels.begin(); }
		inline auto end() { return m_Panels.end(); }

		inline auto rbegin() { return m_Panels.rbegin(); }
		inline auto rend() { return m_Panels.rend(); }

		inline auto size() { return m_Panels.size(); }

	private:
		std::map< size_t, Panel* > m_Panels;
	};

}

#endif // IS_EDITOR