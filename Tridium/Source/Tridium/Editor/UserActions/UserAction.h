#pragma once
#include <Tridium/Editor/Config.h>

#if WITH_EDITOR

#include <Tridium/Containers/Array.h>
#include <Tridium/Containers/Deque.h>
#include <Tridium/Core/Memory.h>

namespace Tridium {

	//=================================================================================================
	// User Action Interface: A user action is an editor operation that can be undone/redone.
	// This can be moving a game object, changing a property, adding/removing files, etc.
	//=================================================================================================
	class IUserAction
	{
	public:

		//=============================================================================================
		IUserAction() = default;
		virtual ~IUserAction() = default;

		//=============================================================================================
		// Undo the user action. I.e. revert the changes made by the action via Ctrl+Z.
		virtual void Undo() = 0;

		//=============================================================================================
		// Redo the user action. I.e. reapply the changes made by the action via Ctrl+Y.
		virtual void Redo() = 0;

		//=============================================================================================
		// Get a string representation of the action for display in the UI.
		// e.g., "Modify Transform"
		// So it can be used like "Undo Modify Transform" or "Redo Modify Transform".
		virtual String ToString() const { return {}; }

	};

	//=============================================================================================
	// Entries are stored as arrays to handle batched actions.
	// But kept in a small array for memory efficiency since most actions are singular.
	using UserActionBatch = SmallArray<UniquePtr<IUserAction>, 1>;

	//=================================================================================================
	// User Action Manager: Manages the stack of user actions for undo/redo functionality.
	//=================================================================================================
	class UserActionManager
	{
	public:

		//=============================================================================================
		// Push a single user action onto the stack.
		void Push( UniquePtr<IUserAction> a_Action )
		{
			UserActionBatch batch;
			batch.EmplaceBack( std::move( a_Action ) );
			Push( std::move( batch ) );
		}

		//=============================================================================================
		// Push a batch of user actions onto the stack.
		void Push( UserActionBatch&& a_Actions )
		{
			// First truncate future actions if needed
			if ( m_HeadIndex < m_ActionStack.size() )
			{
				m_ActionStack.resize( m_HeadIndex );
			}

			// Trim history if needed
			if ( m_ActionStack.size() >= m_MaxHistorySize )
			{
				m_ActionStack.pop_front();
			}

			// Push the new batch
			m_ActionStack.emplace_back( std::move( a_Actions ) );
			m_HeadIndex = m_ActionStack.size();

		}

		//=============================================================================================
		// Undo the last batch of user actions.
		void Undo()
		{
			if ( m_HeadIndex <= 0 )
				return; // No actions to undo.

			for ( const UniquePtr<IUserAction>& action : m_ActionStack[--m_HeadIndex] )
			{
				ASSERT( action != nullptr, "Invalid action in undo stack" );
				action->Undo();
			}
		}

		//=============================================================================================
		// Redo the next batch of user actions.
		void Redo()
		{
			if ( m_HeadIndex >= m_ActionStack.size() )
				return; // No actions to redo.

			for ( const UniquePtr<IUserAction>& action : m_ActionStack[m_HeadIndex++] )
			{
				ASSERT( action != nullptr, "Invalid action in redo stack" );
				action->Redo();
			}
		}

		//=============================================================================================
		void Clear()
		{
			m_ActionStack.clear();
			m_HeadIndex = 0;
		}

		//=============================================================================================
		void SetMaxHistorySize( size_t a_Size ) 
		{ 
			m_MaxHistorySize = a_Size;

			// Trim history if needed
			if ( m_ActionStack.size() > m_MaxHistorySize )
			{
				m_ActionStack.erase( m_ActionStack.begin(), m_ActionStack.begin() + ( m_ActionStack.size() - m_MaxHistorySize ) );
				m_HeadIndex = std::min( m_HeadIndex, m_MaxHistorySize );
			}
		}

	private:

		//=============================================================================================
		// Stack of user actions for undo/redo functionality.
		Deque<UserActionBatch> m_ActionStack;

		//=============================================================================================
		// Current index in the action stack for undo/redo tracking.
		size_t m_HeadIndex = 0;

		//=============================================================================================
		// Maximum history size to limit memory usage.
		size_t m_MaxHistorySize = 256;

	};

} // namespace Tridium

#endif // WITH_EDITOR