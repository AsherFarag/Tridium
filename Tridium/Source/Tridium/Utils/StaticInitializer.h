#pragma once
#include <Tridium/Core/Hash.h>
#include <Tridium/Containers/Array.h>
#include <functional>

namespace Tridium {

	class Initializer
	{
	public:
		struct Node
		{
			HashedString Name;
			Initializer* Instance = nullptr;
			std::function<void()> InitFunction;
			std::function<void()> ShutdownFunction;
			SmallArray<Node*, 8> Dependencies;
			SmallArray<Node*, 8> Dependents;
		};
	};

}