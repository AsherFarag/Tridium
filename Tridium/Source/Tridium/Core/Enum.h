#pragma once

#define DEFINE_BITMASK_OPS( Enum ) \
    constexpr Enum operator&(Enum _Left, Enum _Right) noexcept { \
        using _IntTy = std::underlying_type_t<Enum>; \
        return static_cast<Enum>(static_cast<_IntTy>(_Left) & static_cast<_IntTy>(_Right)); \
    } \
    \
    constexpr Enum operator|(Enum _Left, Enum _Right) noexcept { \
        using _IntTy = std::underlying_type_t<Enum>; \
        return static_cast<Enum>(static_cast<_IntTy>(_Left) | static_cast<_IntTy>(_Right)); \
    } \
    \
    constexpr Enum operator^(Enum _Left, Enum _Right) noexcept { \
        using _IntTy = std::underlying_type_t<Enum>; \
        return static_cast<Enum>(static_cast<_IntTy>(_Left) ^ static_cast<_IntTy>(_Right)); \
    } \
    \
    constexpr Enum& operator&=(Enum& _Left, Enum _Right) noexcept { \
        return _Left = _Left & _Right; \
    } \
    \
    constexpr Enum& operator|=(Enum& _Left, Enum _Right) noexcept { \
        return _Left = _Left | _Right; \
    } \
    \
    constexpr Enum& operator^=(Enum& _Left, Enum _Right) noexcept { \
        return _Left = _Left ^ _Right; \
    } \
    \
    constexpr Enum operator~(Enum _Left) noexcept { \
        using _IntTy = std::underlying_type_t<Enum>; \
        return static_cast<Enum>(~static_cast<_IntTy>(_Left)); \
    }