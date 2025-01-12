#pragma once
#include <string>

namespace Tridium {

	struct RHIDeviceInfo
	{
		std::string DeviceName;
		std::string DeviceManufacturer;
		uint64_t Memory;
	};

}