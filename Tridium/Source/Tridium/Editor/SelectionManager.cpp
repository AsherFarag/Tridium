#include "SelectionManager.h"

#if WITH_EDITOR

#include <Tridium/Editor/Editor.h>

namespace Tridium {

	SelectionManager& SelectionManager::Get()
	{
		ASSERT( Editor::Get(), "Editor instance is not available!" );
		return Editor::Get()->GetSelectionManager();
	}

	void SelectionManager::Select( ESelectionContext a_Context, const Selectable& a_Selectable )
	{
		auto& selections = m_ContextSelections[(size_t)a_Context];

		if ( IsSelected( a_Context, a_Selectable ) )
			return; // Already selected

		selections.PushBack( a_Selectable );
		Editor::Events::OnSelectionChanged.Broadcast( a_Context, a_Selectable, true );
	}

	bool SelectionManager::IsSelected( ESelectionContext a_Context, const Selectable& a_Selectable ) const
	{
		const auto& selections = m_ContextSelections[(size_t)a_Context];
		return std::find( selections.Begin(), selections.End(), a_Selectable ) != selections.End();
	}

	bool SelectionManager::IsSelected( const Selectable& a_Selectable ) const
	{
		for ( size_t i = 0; i < (size_t)ESelectionContext::COUNT; ++i )
		{
			if ( IsSelected( ( ESelectionContext )i, a_Selectable ) )
				return true;
		}

		return false;
	}

	void SelectionManager::Deselect( ESelectionContext a_Context, const Selectable& a_Selectable )
	{
		auto& selections = m_ContextSelections[(size_t)a_Context];
		auto it = std::find( selections.Begin(), selections.End(), a_Selectable );

		if ( it != selections.End() )
		{
			selections.Erase( it );
			Editor::Events::OnSelectionChanged.Broadcast( a_Context, a_Selectable, false );
		}
	}

	void SelectionManager::DeselectAll()
	{
		for ( size_t i = 0; i < (size_t)ESelectionContext::COUNT; ++i )
		{
			DeselectAll( ( ESelectionContext )i );
		}
	}

	void SelectionManager::DeselectAll( ESelectionContext a_Context )
	{
		auto& selections = m_ContextSelections[(size_t)a_Context];

		for ( const auto& selectable : selections )
		{
			Editor::Events::OnSelectionChanged.Broadcast( a_Context, selectable, false );
		}

		selections.Clear();
	}

} // namespace Tridium

#endif // WITH_EDITOR