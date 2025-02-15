#pragma once
#include <Tridium/Utils/Concepts.h>
#include <Tridium/Containers/Variant.h>
#include <Tridium/Containers/Stack.h>

namespace Tridium {

	namespace Concepts {

		template<typename T>
		concept IsCommand = requires( T a_Command )
		{
			{ a_Command.Undo() } -> SameAs<void>;
			{ a_Command.Redo() } -> SameAs<void>;
		};

	} // namespace Concepts


	template<typename... _Cmds> requires ( Concepts::IsCommand<_Cmds> && ... )
	using VariantCommand = Variant<_Cmds...>;

	template<typename... _Cmds> requires ( Concepts::IsCommand<_Cmds> && ... )
	class AbstractCommandManager
	{
	public:
		using Command = VariantCommand<_Cmds...>;
		using CommandStack = Stack<Command>;

		void Execute( Command a_Command )
		{
			m_RedoStack = {};
			m_UndoStack.push( a_Command );
		}

		template<Concepts::IsCommand _Cmd, typename... _Args>
		void Execute( _Args&&... a_Args )
		{
			Execute( Command( _Cmd( std::forward<_Args>( a_Args )... ) ) );
		}

		void Undo()
		{
			if ( m_UndoStack.empty() )
				return;

			Command command = m_UndoStack.top(); m_UndoStack.pop();
			UndoCommand( command );
			m_RedoStack.push( std::move( command ) );
		}

		void Redo()
		{
			if ( m_RedoStack.empty() )
				return;
			Command command = m_RedoStack.top(); m_RedoStack.pop();
			RedoCommand( command );
			m_UndoStack.push( std::move( command ) );
		}

		void Clear()
		{
			m_UndoStack = {};
			m_RedoStack = {};
		}

	private:
		void RedoCommand( Command& a_Command )
		{
			std::visit( [this]( auto& command )
				{
					command.Redo();
				}, a_Command );
		}

		void UndoCommand( Command& a_Command )
		{
			std::visit( [this]( auto& command )
				{
					command.Undo();
				}, a_Command );
		}

	private:
		CommandStack m_UndoStack;
		CommandStack m_RedoStack;
	};

} // namespace Tridium