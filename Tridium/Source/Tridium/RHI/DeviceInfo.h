#pragma once
#include <string>

namespace Tridium::RHI {

	struct DeviceInfo
	{
		std::string DeviceName;
		std::string DeviceManufacturer;
		uint64_t Memory;
	};

}