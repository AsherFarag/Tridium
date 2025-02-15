#pragma once
#include "AbstractCommandManager.h"
#include "Commands.h"

namespace Tridium {

	using CommandManager = AbstractCommandManager<
		Commands::GameObjectCreated,
		Commands::GameObjectDestroyed
	>;

} // namespace Tridium