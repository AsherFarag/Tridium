#pragma once
#include "AbstractCommandManager.h"
#include "Commands.h"

namespace Tridium {

	struct NoCommand
	{
		void Redo() {}
		void Undo() {}
	};

	using CommandManager = AbstractCommandManager<
		NoCommand
	>;

} // namespace Tridium