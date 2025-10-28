#pragma once
#include <Tridium/Editor/Config.h>

#if WITH_EDITOR

#include <Tridium/Containers/Variant.h>
#include <Tridium/Containers/Array.h>
#include <Tridium/Core/UUID.h>
#include <Tridium/Scene/Scene.h>

namespace Tridium {

	//=================================================================================================
	// Selectable: A variant type for any object in the editor that can be selected.
	//=================================================================================================
	using Selectable = Variant<UUID, GameObject>;

	//=================================================================================================
	// Selection Context: The context in which selection is happening (Global, Scene, Asset Browser)
	//=================================================================================================
	enum class ESelectionContext { Global = 0, Scene, AssetBrowser, COUNT };

	//=================================================================================================
	// Selection Manager:
	//=================================================================================================
	class SelectionManager
	{
	public:

		//=============================================================================================
		static SelectionManager& Get();

		//=============================================================================================
		const auto& Selections( ESelectionContext a_Context ) const { return m_ContextSelections[(size_t)a_Context]; }
		size_t NumSelected( ESelectionContext a_Context ) const { return Selections( a_Context ).Size(); }

		//=============================================================================================
		void Select( ESelectionContext a_Context, const Selectable& a_Selectable );
		bool IsSelected( ESelectionContext a_Context, const Selectable& a_Selectable ) const;
		bool IsSelected( const Selectable& a_Selectable ) const;
		void Deselect( ESelectionContext a_Context, const Selectable& a_Selectable );
		void DeselectAll();
		void DeselectAll( ESelectionContext a_Context );

	private:

		//=============================================================================================
		// Selection arrays for each context.
		Array<Selectable> m_ContextSelections[(size_t)ESelectionContext::COUNT];

	};

} // namespace Tridium

#endif // WITH_EDITOR