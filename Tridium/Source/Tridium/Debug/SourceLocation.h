#pragma once
#include <Tridium/Utils/Macro.h>
#include <source_location>

namespace Tridium {

	//===================================
	// Source Location
	//  Contains information about the source location of the code
	struct SourceLocation
	{
	private:
		const char* m_FileName = nullptr;
		const char* m_Function = nullptr;
		uint32_t m_Line = 0u;
		uint32_t m_Column = 0u;

	public:
		constexpr SourceLocation() noexcept = default;
		[[nodiscard]] constexpr const char* FileName() const noexcept { return m_FileName; }
		[[nodiscard]] constexpr const char* Function() const noexcept { return m_Function; }
		[[nodiscard]] constexpr uint32_t Line() const noexcept { return m_Line; }
		[[nodiscard]] constexpr uint32_t Column() const noexcept { return m_Column; }

        [[nodiscard]] static consteval SourceLocation Current( 
            const uint32_t a_Line = __builtin_LINE(),
            const uint32_t a_Column = __builtin_COLUMN(),
            const char* const a_FileName = __builtin_FILE(),
        #if defined(__clang__) || defined(__EDG__)
            const char* const a_Function = __builtin_FUNCTION()
        #else
            const char* const a_Function = __builtin_FUNCSIG()
        #endif
        ) noexcept
        {
            SourceLocation location;
			location.m_FileName = a_FileName;
            location.m_Function = a_Function;
			location.m_Line = a_Line;
			location.m_Column = a_Column;
			return location;
        }
	};

} // namespace Tridium