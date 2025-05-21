#pragma once
#include "Types.h"

namespace Tridium {

	using ErrorCode = uint32_t;

	//===========================
	// Error Codes
	//  A namespace for common error codes that can be returned to describe the result of an operation.
	//  Every error code other than OK is considered a failure and should be handled accordingly.
	namespace ErrorCodes {

		constexpr ErrorCode OK = 0;
		constexpr ErrorCode Failed = 1;
		constexpr ErrorCode OutOfMemory = 2;
		constexpr ErrorCode OutOfBounds = 3;

	} // namespace ErrorCodes

} // namespace Tridium