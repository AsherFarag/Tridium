#pragma once
#include <Tridium/Core/Types.h>
#include <span>


namespace Tridium {

	template<typename T, size_t _Extent = std::dynamic_extent>
	using Span = std::span<T, _Extent>;

    template <class T, size_t _Extent>
    [[nodiscard]] auto AsBytes( Span<T, _Extent> a_Span ) noexcept
    {
        using _ReturnType = Span<const byte_t, _Extent == std::dynamic_extent ? std::dynamic_extent : sizeof( T ) * _Extent>;
        return _ReturnType{ reinterpret_cast<const byte_t*>( a_Span.data() ), a_Span.size_bytes() };
    }

    template <class _Ty, size_t _Extent> requires ( !std::is_const_v<_Ty> )
    [[nodiscard]] auto AsWritableBytes( Span<_Ty, _Extent> a_Span ) noexcept
    {
        using _ReturnType = Span<byte_t, _Extent == std::dynamic_extent ? std::dynamic_extent : sizeof( _Ty ) * _Extent>;
        return _ReturnType{ reinterpret_cast<byte_t*>( a_Span.data() ), a_Span.size_bytes() };
    }

} // namespace Tridium