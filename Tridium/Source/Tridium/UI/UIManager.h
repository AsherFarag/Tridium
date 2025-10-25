#pragma once
#include <Tridium/UI/Config.h>

#if CONFIG_ENABLE_TOOL_UI

#include <Tridium/Application/Application.h>
#include <Tridium/Containers/String.h>
#include <Tridium/Containers/UnorderedMap.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/UI/UIPanel.h>

namespace Tridium {

	DEFINE_TICK_GROUP( BeginUIDraw );
	DEFINE_TICK_GROUP( DrawUI );
	DEFINE_TICK_GROUP( EndUIDraw );

	//=================================================================================================
	// UI Panel Data: Stores a UI Panel and its associated metadata. 
	//=================================================================================================
	struct UIPanelData
	{
		String Name;
		bool IsOpen = true;
		SharedPtr<IUIPanel> Panel;
	};

	//=================================================================================================
	// UI Manager: Manages the overall UI system, including panels and layout.
	//=================================================================================================
	class UIManager
	{
	public:

		//=============================================================================================
		UIManager() = default;
		~UIManager() = default;

		//=============================================================================================
		// Propagates an event to all open UI panels.
		void OnEvent( Event& a_Event );

		//=============================================================================================
		// Update all registered UI panels.
		void UpdateUI( float a_DeltaTime );

		//=============================================================================================
		// Iterates through and draws all registered UI panels.
		void DrawUI();

		//=============================================================================================
		// Get all registered panels.
		auto& PanelStorage() { return m_Panels; }
		const auto& PanelStorage() const { return m_Panels; }

		//=============================================================================================
		// Get the number of registered panels.
		size_t NumPanels() const { return m_Panels.size(); }

		//=============================================================================================
		// Gets the panel info for a given panel name.
		// Returns nullptr if not found.
		UIPanelData* GetPanelData( StringView a_Name )
		{
			if ( auto it = m_Panels.find( a_Name ); it != m_Panels.end() )
			{
				return &it->second; // Found
			}

			return nullptr; // Not found
		}

		//=============================================================================================
		// Try and get a panel by name.
		SharedPtr<IUIPanel> GetPanel( StringView a_Name )
		{
			if ( auto it = m_Panels.find( a_Name ); it != m_Panels.end() )
			{
				return it->second.Panel; // Found
			}

			return nullptr; // Not found
		}

		//=============================================================================================
		// Try and get a panel by name and type.
		template<Concepts::Derived<IUIPanel> T>
		SharedPtr<T> GetPanel( StringView a_Name )
		{
			if ( auto it = m_Panels.find( a_Name ); it != m_Panels.end() )
			{
				if ( DynamicCast<T*>( it->second.Panel.get() ) == nullptr )
					return nullptr; // Wrong type

				return SharedPtrCast<T>( it->second.Panel ); // Found
			}

			return nullptr; // Not found
		}

		//=============================================================================================
		// Create and register a new panel.
		template<Concepts::Derived<IUIPanel> T, typename... _Args>
		SharedPtr<T> CreatePanel( String a_Name, bool a_Open, _Args&&... a_Args )
		{
			SharedPtr<T> panel = MakeShared<T>( std::forward<_Args>( a_Args )... );

			UIPanelData panelData;
			panelData.Name = std::move( a_Name );
			panelData.IsOpen = a_Open;
			panelData.Panel = panel;
			m_Panels[panelData.Name] = std::move( panelData );

			return panel;
		}

	protected:

		//=============================================================================================
		UnorderedMap<StringView, UIPanelData> m_Panels;

	};

} // namespace Tridium

#endif // CONFIG_ENABLE_TOOL_UI