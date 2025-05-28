#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include <Tridium/Utils/Concepts.h>

namespace Tridium {

	template<size_t _Count, Concepts::Arithmetic T> struct Vector;

	template<Concepts::Arithmetic T>
	struct Tridium::Vector<2, T>
	{
		static constexpr size_t Count = 2;
		using Type = Vector<Count, T>;
		using ValueType = T;
		using GLMType = glm::vec<Count, T>;

		T X;
		T Y;

		// Constructors

		constexpr Vector() noexcept : X( 0 ), Y( 0 ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( U a_Scalar ) noexcept : X( a_Scalar ), Y( a_Scalar ) {}
		template<Concepts::Arithmetic U1, Concepts::Arithmetic U2>
		constexpr Vector( U1 a_X, U2 a_Y ) noexcept : X( a_X ), Y( a_Y ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const Vector<2, U>& a_Other ) noexcept : X( a_Other.X ), Y( a_Other.Y ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const Vector<3, U>& a_Other ) noexcept : X( a_Other.X ), Y( a_Other.Y ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const Vector<4, U>& a_Other ) noexcept : X( a_Other.X ), Y( a_Other.Y ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const Vector<2, U>& a_Other ) noexcept { X = a_Other.X; Y = a_Other.Y; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const Vector<3, U>& a_Other ) noexcept { X = a_Other.X; Y = a_Other.Y; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const Vector<4, U>& a_Other ) noexcept { X = a_Other.X; Y = a_Other.Y; return *this; }

		// Conversions for glm types

		template<Concepts::Arithmetic U>
		constexpr Vector( const glm::vec<2, U>& a_Other ) noexcept : X( a_Other.x ), Y( a_Other.y ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const glm::vec<3, U>& a_Other ) noexcept : X( a_Other.x ), Y( a_Other.y ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const glm::vec<4, U>& a_Other ) noexcept : X( a_Other.x ), Y( a_Other.y ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const glm::vec<2, U>& a_Other ) noexcept { X = a_Other.x; Y = a_Other.y; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const glm::vec<3, U>& a_Other ) noexcept { X = a_Other.x; Y = a_Other.y; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const glm::vec<4, U>& a_Other ) noexcept { X = a_Other.x; Y = a_Other.y; return *this; }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr operator glm::vec<2, U>() const noexcept { return glm::vec<2, U>( X, Y ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr operator glm::vec<3, U>() const noexcept { return glm::vec<3, U>( X, Y, 0 ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr operator glm::vec<4, U>() const noexcept { return glm::vec<4, U>( X, Y, 0, 0 ); }
		[[nodiscard]] operator GLMType& () noexcept { return *ReinterpretCast<GLMType*>( this ); }
		[[nodiscard]] operator const GLMType& () const noexcept { return *ReinterpretCast<const GLMType*>( this ); }

		// Static Constructors

		[[nodiscard]] static constexpr Vector Zero() noexcept { return Vector( 0, 0 ); }
		[[nodiscard]] static constexpr Vector One() noexcept { return Vector( 1, 1 ); }
		[[nodiscard]] static constexpr Vector Up() noexcept { return Vector( 0, 1 ); }
		[[nodiscard]] static constexpr Vector Down() noexcept { return Vector( 0, -1 ); }
		[[nodiscard]] static constexpr Vector Left() noexcept { return Vector( -1, 0 ); }
		[[nodiscard]] static constexpr Vector Right() noexcept { return Vector( 1, 0 ); }

		// Functions

		[[nodiscard]] constexpr T Length() const noexcept { return glm::length( Cast<const GLMType&>( *this ) ); }
		[[nodiscard]] constexpr T LengthSqr() const noexcept requires (Concepts::FloatingPoint<T>) { return glm::length2( Cast<const GLMType&>( *this ) ); }
		[[nodiscard]] constexpr Vector Normalized() const noexcept { return glm::normalize( Cast<const GLMType&>( *this ) ); }
		[[nodiscard]] constexpr void Normalize() noexcept { *this = Normalized(); }

		// Swizzle

		[[nodiscard]] constexpr Vector YX() const noexcept { return Vector( Y, X ); }

		// Increment and Decrement Operators

		constexpr Vector& operator++() noexcept { ++X; ++Y; return *this; }
		constexpr Vector operator++( int ) noexcept { Vector temp = *this; ++(*this); return temp; }
		constexpr Vector& operator--() noexcept { --X; --Y; return *this; }
		constexpr Vector operator--( int ) noexcept { Vector temp = *this; --(*this); return temp; }

		// Arithmetic Operators

		[[nodiscard]] constexpr Vector operator-() const noexcept { return Vector( -X, -Y ); }

		// Access Operators

		[[nodiscard]] constexpr T* Data() noexcept { return &X; }
		[[nodiscard]] constexpr const T* Data() const noexcept { return &X; }
		[[nodiscard]] constexpr T& operator[]( size_t a_Index ) noexcept
		{
			ASSERT( a_Index < Count, "Index out of bounds in Vector<2, T>::operator[]" );
			switch ( a_Index )
			{
			default:
			case 0: return X;
			case 1: return Y;
			}
		}
		[[nodiscard]] constexpr const T& operator[]( size_t a_Index ) const noexcept
		{
			ASSERT( a_Index < Count, "Index out of bounds in Vector<2, T>::operator[]" );
			switch ( a_Index )
			{
			default:
			case 0: return X;
			case 1: return Y;
			}
		}
	};

	template<Concepts::Arithmetic T>
	struct Tridium::Vector<3, T>
	{
		static constexpr size_t Count = 3;
		using Type = Vector<Count, T>;
		using ValueType = T;
		using GLMType = glm::vec<Count, T>;

		T X;
		T Y;
		T Z;

		// Constructors

		constexpr Vector() noexcept : X( 0 ), Y( 0 ), Z( 0 ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( U a_Scalar ) noexcept : X( a_Scalar ), Y( a_Scalar ), Z( a_Scalar ) {}
		template<Concepts::Arithmetic U1, Concepts::Arithmetic U2, Concepts::Arithmetic U3 = T>
		constexpr Vector( U1 a_X, U2 a_Y, U3 a_Z = 0 ) noexcept : X( a_X ), Y( a_Y ), Z( a_Z ) {}
		template<Concepts::Arithmetic U1, Concepts::Arithmetic U2>
		constexpr Vector( const Vector<2, U1>& a_XY, U2 a_Z = 0 ) noexcept : X( a_XY.X ), Y( a_XY.Y ), Z( a_Z ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const Vector<3, U>& a_Other ) noexcept : X( a_Other.X ), Y( a_Other.Y ), Z( a_Other.Z ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const Vector<4, U>& a_Other ) noexcept : X( a_Other.X ), Y( a_Other.Y ), Z( a_Other.Z ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const Vector<2, U>& a_Other ) noexcept { X = a_Other.X; Y = a_Other.Y; Z = 0; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const Vector<3, U>& a_Other ) noexcept { X = a_Other.X; Y = a_Other.Y; Z = a_Other.Z; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const Vector<4, U>& a_Other ) noexcept { X = a_Other.X; Y = a_Other.Y; Z = a_Other.Z; return *this; }

		// Conversions for glm types

		template<Concepts::Arithmetic U>
		constexpr Vector( const glm::vec<2, U>& a_Other ) noexcept : X( a_Other.x ), Y( a_Other.y ), Z( 0 ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const glm::vec<3, U>& a_Other ) noexcept : X( a_Other.x ), Y( a_Other.y ), Z( a_Other.z ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const glm::vec<4, U>& a_Other ) noexcept : X( a_Other.x ), Y( a_Other.y ), Z( a_Other.z ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const glm::vec<2, U>& a_Other ) noexcept { X = a_Other.x; Y = a_Other.y; Z = 0; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const glm::vec<3, U>& a_Other ) noexcept { X = a_Other.x; Y = a_Other.y; Z = a_Other.z; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const glm::vec<4, U>& a_Other ) noexcept { X = a_Other.x; Y = a_Other.y; Z = a_Other.z; return *this; }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr operator glm::vec<2, U>() const noexcept { return glm::vec<2, U>( X, Y ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr operator glm::vec<3, U>() const noexcept { return glm::vec<3, U>( X, Y, Z ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr operator glm::vec<4, U>() const noexcept { return glm::vec<4, U>( X, Y, Z, 0 ); } 
		[[nodiscard]] operator GLMType& () noexcept { return *ReinterpretCast<GLMType*>( this ); }
		[[nodiscard]] operator const GLMType& () const noexcept { return *ReinterpretCast<const GLMType*>( this ); }

		// Static Constructors

		[[nodiscard]] static constexpr Vector Zero() noexcept { return Vector( 0, 0, 0 ); }
		[[nodiscard]] static constexpr Vector One() noexcept { return Vector( 1, 1, 1 ); }
		[[nodiscard]] static constexpr Vector Up() noexcept { return Vector( 0, 1, 0 ); }
		[[nodiscard]] static constexpr Vector Down() noexcept { return Vector( 0, -1, 0 ); }
		[[nodiscard]] static constexpr Vector Left() noexcept { return Vector( -1, 0, 0 ); }
		[[nodiscard]] static constexpr Vector Right() noexcept { return Vector( 1, 0, 0 ); }
		[[nodiscard]] static constexpr Vector Forward() noexcept { return Vector( 0, 0, 1 ); }
		[[nodiscard]] static constexpr Vector Backward() noexcept { return Vector( 0, 0, -1 ); }

		// Functions

		[[nodiscard]] constexpr T Length() const noexcept { return glm::length( Cast<const GLMType&>( *this ) ); }
		[[nodiscard]] constexpr T LengthSqr() const noexcept { return glm::dot( Cast<const GLMType&>( *this ), Cast<const GLMType&>( *this ) ); }
		[[nodiscard]] constexpr Vector Normalized() const noexcept { return glm::normalize( Cast<const GLMType&>( *this ) ); }
		[[nodiscard]] constexpr void Normalize() noexcept { *this = Normalized(); }

		// Swizzle

		[[nodiscard]] constexpr Vector<2, T> XY() const noexcept { return Vector<2, T>( X, Y ); }
		[[nodiscard]] constexpr Vector<2, T> XZ() const noexcept { return Vector<2, T>( X, Z ); }
		[[nodiscard]] constexpr Vector<2, T> YX() const noexcept { return Vector<2, T>( Y, X ); }
		[[nodiscard]] constexpr Vector<2, T> YZ() const noexcept { return Vector<2, T>( Y, Z ); }
		[[nodiscard]] constexpr Vector<2, T> ZX() const noexcept { return Vector<2, T>( Z, X ); }
		[[nodiscard]] constexpr Vector<2, T> ZY() const noexcept { return Vector<2, T>( Z, Y ); }
		[[nodiscard]] constexpr Vector<3, T> XYZ() const noexcept { return Vector<3, T>( X, Y, Z ); }
		[[nodiscard]] constexpr Vector<3, T> XZY() const noexcept { return Vector<3, T>( X, Z, Y ); }
		[[nodiscard]] constexpr Vector<3, T> YXZ() const noexcept { return Vector<3, T>( Y, X, Z ); }
		[[nodiscard]] constexpr Vector<3, T> YZX() const noexcept { return Vector<3, T>( Y, Z, X ); }
		[[nodiscard]] constexpr Vector<3, T> ZXY() const noexcept { return Vector<3, T>( Z, X, Y ); }
		[[nodiscard]] constexpr Vector<3, T> ZYX() const noexcept { return Vector<3, T>( Z, Y, X ); }

		// Increment and Decrement

		constexpr Vector& operator++() noexcept { ++X; ++Y; ++Z; return *this; }
		constexpr Vector operator++( int ) noexcept { Vector temp = *this; ++(*this); return temp; }
		constexpr Vector& operator--() noexcept { --X; --Y; --Z; return *this; }
		constexpr Vector operator--( int ) noexcept { Vector temp = *this; --(*this); return temp; }

		// Arithmetic Operators

		[[nodiscard]] constexpr Vector operator-() const noexcept { return Vector( -X, -Y, -Z ); }

#if 0

		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr Vector operator+( const Vector<3, U>& a_Other ) const noexcept { return Vector( X + a_Other.X, Y + a_Other.Y, Z + a_Other.Z ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr Vector operator-( const Vector<3, U>& a_Other ) const noexcept { return Vector( X - a_Other.X, Y - a_Other.Y, Z - a_Other.Z ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr Vector operator*( const Vector<3, U>& a_Other ) const noexcept { return Vector( X * a_Other.X, Y * a_Other.Y, Z * a_Other.Z ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr Vector operator/( const Vector<3, U>& a_Other ) const noexcept { return Vector( X / a_Other.X, Y / a_Other.Y, Z / a_Other.Z ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr Vector operator+( const U& a_Other ) const noexcept { return Vector( X + a_Other, Y + a_Other, Z + a_Other ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr Vector operator-( const U& a_Other ) const noexcept { return Vector( X - a_Other, Y - a_Other, Z - a_Other ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr Vector operator*( const U& a_Other ) const noexcept { return Vector( X * a_Other, Y * a_Other, Z * a_Other ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr Vector operator/( const U& a_Other ) const noexcept { return Vector( X / a_Other, Y / a_Other, Z / a_Other ); }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator+=( const Vector<3, U>& a_Other ) noexcept { X += a_Other.X; Y += a_Other.Y; Z += a_Other.Z; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator-=( const Vector<3, U>& a_Other ) noexcept { X -= a_Other.X; Y -= a_Other.Y; Z -= a_Other.Z; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator*=( const Vector<3, U>& a_Other ) noexcept { X *= a_Other.X; Y *= a_Other.Y; Z *= a_Other.Z; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator/=( const Vector<3, U>& a_Other ) noexcept { X /= a_Other.X; Y /= a_Other.Y; Z /= a_Other.Z; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator+=( const U& a_Other ) noexcept { X += a_Other; Y += a_Other; Z += a_Other; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator-=( const U& a_Other ) noexcept { X -= a_Other; Y -= a_Other; Z -= a_Other; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator*=( const U& a_Other ) noexcept { X *= a_Other; Y *= a_Other; Z *= a_Other; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator/=( const U& a_Other ) noexcept { X /= a_Other; Y /= a_Other; Z /= a_Other; return *this; }

#endif

		// Comparison Operators

		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator==( const Vector<3, U>& a_Other ) const noexcept { return X == a_Other.X && Y == a_Other.Y && Z == a_Other.Z; }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator!=( const Vector<3, U>& a_Other ) const noexcept { return !(*this == a_Other); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator<( const Vector<3, U>& a_Other ) const noexcept { return LengthSqr() < a_Other.LengthSqr(); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator>( const Vector<3, U>& a_Other ) const noexcept { return LengthSqr() > a_Other.LengthSqr(); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator<=( const Vector<3, U>& a_Other ) const noexcept { return LengthSqr() <= a_Other.LengthSqr(); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator>=( const Vector<3, U>& a_Other ) const noexcept { return LengthSqr() >= a_Other.LengthSqr(); }

		// Access Operators

		[[nodiscard]] constexpr T* Data() noexcept { return &X; }
		[[nodiscard]] constexpr const T* Data() const noexcept { return &X; }
		[[nodiscard]] constexpr T& operator[]( size_t a_Index ) noexcept
		{
			ASSERT( a_Index < Count, "Index out of bounds in Vector<3, T>::operator[]" );
			switch ( a_Index )
			{
			default:
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			}
		}

		[[nodiscard]] constexpr const T& operator[]( size_t a_Index ) const noexcept
		{
			ASSERT( a_Index < Count, "Index out of bounds in Vector<3, T>::operator[]" );
			switch ( a_Index )
			{
			default:
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			}
		}
	};

	template<Concepts::Arithmetic T>
	struct Tridium::Vector<4, T>
	{
		static constexpr size_t Count = 4;
		using Type = Vector<Count, T>;
		using ValueType = T;
		using GLMType = glm::vec<Count, T>;

		T X;
		T Y;
		T Z;
		T W;

		// Constructors

		constexpr Vector() noexcept : X( 0 ), Y( 0 ), Z( 0 ), W( 0 ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( U a_Scalar ) noexcept : X( a_Scalar ), Y( a_Scalar ), Z( a_Scalar ), W( a_Scalar ) {}
		template<Concepts::Arithmetic U1, Concepts::Arithmetic U2, Concepts::Arithmetic U3 = T, Concepts::Arithmetic U4 = T>
		constexpr Vector( U1 a_X, U2 a_Y, U3 a_Z = 0, U4 a_W = 0 ) noexcept : X( a_X ), Y( a_Y ), Z( a_Z ), W( a_W ) {}
		template<Concepts::Arithmetic U1, Concepts::Arithmetic U2 = T, Concepts::Arithmetic U3 = T>
		constexpr Vector( const Vector<2, U1>& a_XY, U2 a_Z = 0, U3 a_W = 0 ) noexcept : X( a_XY.X ), Y( a_XY.Y ), Z( a_Z ), W( a_W ) {}
		template<Concepts::Arithmetic U1, Concepts::Arithmetic U2 = T>
		constexpr Vector( const Vector<3, U1>& a_XYZ, U2 a_W = 0 ) noexcept : X( a_XYZ.X ), Y( a_XYZ.Y ), Z( a_XYZ.Z ), W( a_W ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const Vector<4, U>& a_Other ) noexcept : X( a_Other.X ), Y( a_Other.Y ), Z( a_Other.Z ), W( a_Other.W ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const Vector<2, U>& a_Other ) noexcept { X = a_Other.X; Y = a_Other.Y; Z = 0; W = 0; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const Vector<3, U>& a_Other ) noexcept { X = a_Other.X; Y = a_Other.Y; Z = a_Other.Z; W = 0; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const Vector<4, U>& a_Other ) noexcept { X = a_Other.X; Y = a_Other.Y; Z = a_Other.Z; W = a_Other.W; return *this; }

		// Conversions for glm types

		template<Concepts::Arithmetic U>
		constexpr Vector( const glm::vec<2, U>& a_Other ) noexcept : X( a_Other.x ), Y( a_Other.y ), Z( 0 ), W( 0 ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const glm::vec<3, U>& a_Other ) noexcept : X( a_Other.x ), Y( a_Other.y ), Z( a_Other.z ), W( 0 ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector( const glm::vec<4, U>& a_Other ) noexcept : X( a_Other.x ), Y( a_Other.y ), Z( a_Other.z ), W( a_Other.w ) {}
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const glm::vec<2, U>& a_Other ) noexcept { X = a_Other.x; Y = a_Other.y; Z = 0; W = 0; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const glm::vec<3, U>& a_Other ) noexcept { X = a_Other.x; Y = a_Other.y; Z = a_Other.z; W = 0; return *this; }
		template<Concepts::Arithmetic U>
		constexpr Vector& operator=( const glm::vec<4, U>& a_Other ) noexcept { X = a_Other.x; Y = a_Other.y; Z = a_Other.z; W = a_Other.w; return *this; }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr operator glm::vec<2, U>() const noexcept { return glm::vec<2, U>( X, Y ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr operator glm::vec<3, U>() const noexcept { return glm::vec<3, U>( X, Y, Z ); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr operator glm::vec<4, U>() const noexcept { return glm::vec<4, U>( X, Y, Z, W ); }
		[[nodiscard]] operator GLMType& () noexcept { return *ReinterpretCast<GLMType*>( this ); }
		[[nodiscard]] operator const GLMType& () const noexcept { return *ReinterpretCast<const GLMType*>( this ); }

		// Static Constructors

		[[nodiscard]] static constexpr Vector Zero() noexcept { return Vector( 0, 0, 0, 0 ); }
		[[nodiscard]] static constexpr Vector One() noexcept { return Vector( 1, 1, 1, 1 ); }
		[[nodiscard]] static constexpr Vector Up() noexcept { return Vector( 0, 1, 0, 0 ); }
		[[nodiscard]] static constexpr Vector Down() noexcept { return Vector( 0, -1, 0, 0 ); }
		[[nodiscard]] static constexpr Vector Left() noexcept { return Vector( -1, 0, 0, 0 ); }
		[[nodiscard]] static constexpr Vector Right() noexcept { return Vector( 1, 0, 0, 0 ); }
		[[nodiscard]] static constexpr Vector Forward() noexcept { return Vector( 0, 0, 1, 0 ); }
		[[nodiscard]] static constexpr Vector Backward() noexcept { return Vector( 0, 0, -1, 0 ); }

		// Functions

		[[nodiscard]] constexpr T Length() const noexcept { return glm::length( Cast<const GLMType&>( *this ) ); }
		[[nodiscard]] constexpr T LengthSqr() const noexcept requires (Concepts::FloatingPoint<T>) { return glm::length2( Cast<const GLMType&>( *this ) ); }
		[[nodiscard]] constexpr Vector Normalized() const noexcept { return glm::normalize( Cast<const GLMType&>( *this ) ); }
		[[nodiscard]] constexpr void Normalize() noexcept { *this = Normalized(); }

		// Swizzle 

		[[nodiscard]] constexpr Vector<2, T> XY() const noexcept { return Vector<2, T>( X, Y ); }
		[[nodiscard]] constexpr Vector<2, T> XZ() const noexcept { return Vector<2, T>( X, Z ); }
		[[nodiscard]] constexpr Vector<2, T> XW() const noexcept { return Vector<2, T>( X, W ); }
		[[nodiscard]] constexpr Vector<2, T> YX() const noexcept { return Vector<2, T>( Y, X ); }
		[[nodiscard]] constexpr Vector<2, T> YZ() const noexcept { return Vector<2, T>( Y, Z ); }
		[[nodiscard]] constexpr Vector<2, T> YW() const noexcept { return Vector<2, T>( Y, W ); }
		[[nodiscard]] constexpr Vector<2, T> ZX() const noexcept { return Vector<2, T>( Z, X ); }
		[[nodiscard]] constexpr Vector<2, T> ZY() const noexcept { return Vector<2, T>( Z, Y ); }
		[[nodiscard]] constexpr Vector<2, T> ZW() const noexcept { return Vector<2, T>( Z, W ); }
		[[nodiscard]] constexpr Vector<2, T> WX() const noexcept { return Vector<2, T>( W, X ); }
		[[nodiscard]] constexpr Vector<2, T> WY() const noexcept { return Vector<2, T>( W, Y ); }
		[[nodiscard]] constexpr Vector<2, T> WZ() const noexcept { return Vector<2, T>( W, Z ); }
		[[nodiscard]] constexpr Vector<3, T> XYZ() const noexcept { return Vector<3, T>( X, Y, Z ); }
		[[nodiscard]] constexpr Vector<3, T> XZY() const noexcept { return Vector<3, T>( X, Z, Y ); }
		[[nodiscard]] constexpr Vector<3, T> XWZ() const noexcept { return Vector<3, T>( X, W, Z ); }
		[[nodiscard]] constexpr Vector<3, T> XWY() const noexcept { return Vector<3, T>( X, W, Y ); }
		[[nodiscard]] constexpr Vector<3, T> YXZ() const noexcept { return Vector<3, T>( Y, X, Z ); }
		[[nodiscard]] constexpr Vector<3, T> YZX() const noexcept { return Vector<3, T>( Y, Z, X ); }
		[[nodiscard]] constexpr Vector<3, T> YWZ() const noexcept { return Vector<3, T>( Y, W, Z ); }
		[[nodiscard]] constexpr Vector<3, T> YWX() const noexcept { return Vector<3, T>( Y, W, X ); }
		[[nodiscard]] constexpr Vector<3, T> ZXY() const noexcept { return Vector<3, T>( Z, X, Y ); }
		[[nodiscard]] constexpr Vector<3, T> ZYX() const noexcept { return Vector<3, T>( Z, Y, X ); }
		[[nodiscard]] constexpr Vector<3, T> ZWY() const noexcept { return Vector<3, T>( Z, W, Y ); }
		[[nodiscard]] constexpr Vector<3, T> ZWX() const noexcept { return Vector<3, T>( Z, W, X ); }
		[[nodiscard]] constexpr Vector<3, T> WXY() const noexcept { return Vector<3, T>( W, X, Y ); }
		[[nodiscard]] constexpr Vector<3, T> WZY() const noexcept { return Vector<3, T>( W, Z, Y ); }
		[[nodiscard]] constexpr Vector<3, T> WZX() const noexcept { return Vector<3, T>( W, Z, X ); }
		[[nodiscard]] constexpr Vector<4, T> XYZW() const noexcept { return Vector<4, T>( X, Y, Z, W ); }
		[[nodiscard]] constexpr Vector<4, T> XWYZ() const noexcept { return Vector<4, T>( X, W, Y, Z ); }
		[[nodiscard]] constexpr Vector<4, T> YXZW() const noexcept { return Vector<4, T>( Y, X, Z, W ); }
		[[nodiscard]] constexpr Vector<4, T> YWZX() const noexcept { return Vector<4, T>( Y, W, Z, X ); }
		[[nodiscard]] constexpr Vector<4, T> ZXYW() const noexcept { return Vector<4, T>( Z, X, Y, W ); }
		[[nodiscard]] constexpr Vector<4, T> ZWXY() const noexcept { return Vector<4, T>( Z, W, X, Y ); }
		[[nodiscard]] constexpr Vector<4, T> WXZY() const noexcept { return Vector<4, T>( W, X, Z, Y ); }
		[[nodiscard]] constexpr Vector<4, T> WYZX() const noexcept { return Vector<4, T>( W, Y, Z, X ); }
		[[nodiscard]] constexpr Vector<4, T> WXYZ() const noexcept { return Vector<4, T>( W, X, Y, Z ); }
		[[nodiscard]] constexpr Vector<4, T> WZYX() const noexcept { return Vector<4, T>( W, Z, Y, X ); }
		[[nodiscard]] constexpr Vector<4, T> WZXY() const noexcept { return Vector<4, T>( W, Z, X, Y ); }

		// Increment and Decrement

		constexpr Vector& operator++() noexcept { ++X; ++Y; ++Z; ++W; return *this; }
		constexpr Vector operator++( int ) noexcept { Vector temp = *this; ++(*this); return temp; }
		constexpr Vector& operator--() noexcept { --X; --Y; --Z; --W; return *this; }
		constexpr Vector operator--( int ) noexcept { Vector temp = *this; --(*this); return temp; }

		// Arithmetic Operators

		[[nodiscard]] constexpr Vector operator-() const noexcept { return Vector( -X, -Y, -Z, -W ); }

		// Comparison Operators

		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator==( const Vector<4, U>& a_Other ) const noexcept { return X == a_Other.X && Y == a_Other.Y && Z == a_Other.Z && W == a_Other.W; }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator!=( const Vector<4, U>& a_Other ) const noexcept { return !(*this == a_Other); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator<( const Vector<4, U>& a_Other ) const noexcept { return LengthSqr() < a_Other.LengthSqr(); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator>( const Vector<4, U>& a_Other ) const noexcept { return LengthSqr() > a_Other.LengthSqr(); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator<=( const Vector<4, U>& a_Other ) const noexcept { return LengthSqr() <= a_Other.LengthSqr(); }
		template<Concepts::Arithmetic U>
		[[nodiscard]] constexpr bool operator>=( const Vector<4, U>& a_Other ) const noexcept { return LengthSqr() >= a_Other.LengthSqr(); }

		// Access Operators

		[[nodiscard]] constexpr T* Data() noexcept { return &X; }
		[[nodiscard]] constexpr const T* Data() const noexcept { return &X; }
		[[nodiscard]] constexpr T& operator[]( size_t a_Index ) noexcept
		{
			ASSERT( a_Index < Count, "Index out of bounds in Vector<4, T>::operator[]" );
			switch ( a_Index )
			{
			default:
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			case 3: return W;
			}
		}
		[[nodiscard]] constexpr const T& operator[]( size_t a_Index ) const noexcept
		{
			ASSERT( a_Index < Count, "Index out of bounds in Vector<4, T>::operator[]" );
			switch ( a_Index )
			{
			default:
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			case 3: return W;
			}
		}
	};

	// Arithmetic operator overloads for Vector types

	// Vector & Vector

	template<size_t _Count, Concepts::Arithmetic T>
	[[nodiscard]] constexpr Vector<_Count, T> operator+( const Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		Vector<_Count, T> result{ a_Left };
		for ( size_t i = 0; i < _Count; ++i ) result[i] += a_Right[i];
		return result;
	}

	template<size_t _Count, Concepts::Arithmetic T>
	[[nodiscard]] constexpr Vector<_Count, T> operator-( const Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		Vector<_Count, T> result{ a_Left };
		for ( size_t i = 0; i < _Count; ++i ) result[i] -= a_Right[i];
		return result;
	}

	template<size_t _Count, Concepts::Arithmetic T>
	[[nodiscard]] constexpr Vector<_Count, T> operator*( const Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		Vector<_Count, T> result{ a_Left };
		for ( size_t i = 0; i < _Count; ++i ) result[i] *= a_Right[i];
		return result;
	}

	template<size_t _Count, Concepts::Arithmetic T>
	[[nodiscard]] constexpr Vector<_Count, T> operator/( const Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		Vector<_Count, T> result{ a_Left };
		for ( size_t i = 0; i < _Count; ++i ) result[i] /= a_Right[i];
		return result;
	}

	template<size_t _Count, Concepts::Arithmetic T>
	[[nodiscard]] constexpr Vector<_Count, T>& operator+=( Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		for ( size_t i = 0; i < _Count; ++i ) a_Left[i] += a_Right[i];
		return a_Left;
	}

	template<size_t _Count, Concepts::Arithmetic T>
	[[nodiscard]] constexpr Vector<_Count, T>& operator-=( Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		for ( size_t i = 0; i < _Count; ++i ) a_Left[i] -= a_Right[i];
		return a_Left;
	}


	template<size_t _Count, Concepts::Arithmetic T>
	[[nodiscard]] constexpr Vector<_Count, T>& operator*=( Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		for ( size_t i = 0; i < _Count; ++i ) a_Left[i] *= a_Right[i];
		return a_Left;
	}

	template<size_t _Count, Concepts::Arithmetic T>
	[[nodiscard]] constexpr Vector<_Count, T>& operator/=( Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		for ( size_t i = 0; i < _Count; ++i ) a_Left[i] /= a_Right[i];
		return a_Left;
	}

	// Vector & Scalar

	template<size_t _Count, Concepts::Arithmetic T1, Concepts::Arithmetic T2>
	[[nodiscard]] constexpr Vector<_Count, T1> operator+( const Vector<_Count, T1>& a_Left, const T2& a_Right ) noexcept
	{
		Vector<_Count, T1> result{ a_Left };
		for ( size_t i = 0; i < _Count; ++i ) result[i] += Cast<T1>( a_Right );
		return result;
	}

	template<size_t _Count, Concepts::Arithmetic T1, Concepts::Arithmetic T2>
	[[nodiscard]] constexpr Vector<_Count, T1> operator-( const Vector<_Count, T1>& a_Left, const T2& a_Right ) noexcept
	{
		Vector<_Count, T1> result{ a_Left };
		for ( size_t i = 0; i < _Count; ++i ) result[i] -= Cast<T1>( a_Right );
		return result;
	}

	template<size_t _Count, Concepts::Arithmetic T1, Concepts::Arithmetic T2>
	[[nodiscard]] constexpr Vector<_Count, T1> operator*( const Vector<_Count, T1>& a_Left, const T2& a_Right ) noexcept
	{
		Vector<_Count, T1> result{ a_Left };
		for ( size_t i = 0; i < _Count; ++i ) result[i] *= Cast<T1>( a_Right );
		return result;
	}

	template<size_t _Count, Concepts::Arithmetic T1, Concepts::Arithmetic T2>
	[[nodiscard]] constexpr Vector<_Count, T1> operator/( const Vector<_Count, T1>& a_Left, const T2& a_Right ) noexcept
	{
		Vector<_Count, T1> result{ a_Left };
		for ( size_t i = 0; i < _Count; ++i ) result[i] /= Cast<T1>( a_Right );
		return result;
	}

	template<size_t _Count, Concepts::Arithmetic T1, Concepts::Arithmetic T2>
	[[nodiscard]] constexpr Vector<_Count, T1>& operator+=( Vector<_Count, T1>& a_Left, const T2& a_Right ) noexcept
	{
		for ( size_t i = 0; i < _Count; ++i ) a_Left[i] += Cast<T1>( a_Right );
		return a_Left;
	}

	template<size_t _Count, Concepts::Arithmetic T1, Concepts::Arithmetic T2>
	[[nodiscard]] constexpr Vector<_Count, T1>& operator-=( Vector<_Count, T1>& a_Left, const T2& a_Right ) noexcept
	{
		for ( size_t i = 0; i < _Count; ++i ) a_Left[i] -= Cast<T1>( a_Right );
		return a_Left;
	}

	template<size_t _Count, Concepts::Arithmetic T1, Concepts::Arithmetic T2>
	[[nodiscard]] constexpr Vector<_Count, T1>& operator*=( Vector<_Count, T1>& a_Left, const T2& a_Right ) noexcept
	{
		for ( size_t i = 0; i < _Count; ++i ) a_Left[i] *= Cast<T1>( a_Right );
		return a_Left;
	}



	// Scalar & Vector

	template<size_t _Count, Concepts::Arithmetic T1, Concepts::Arithmetic T2>
	[[nodiscard]] constexpr Vector<_Count, T1> operator+( const T2& a_Left, const Vector<_Count, T1>& a_Right ) noexcept
	{
		Vector<_Count, T1> result{ a_Right };
		for ( size_t i = 0; i < _Count; ++i ) result[i] += Cast<T1>( a_Left );
		return result;
	}

	template<size_t _Count, Concepts::Arithmetic T1, Concepts::Arithmetic T2>
	[[nodiscard]] constexpr Vector<_Count, T1> operator-( const T2& a_Left, const Vector<_Count, T1>& a_Right ) noexcept
	{
		Vector<_Count, T1> result{ -a_Right };
		for ( size_t i = 0; i < _Count; ++i ) result[i] += Cast<T1>( a_Left );
		return result;
	}

	template<size_t _Count, Concepts::Arithmetic T1, Concepts::Arithmetic T2>
	[[nodiscard]] constexpr Vector<_Count, T1> operator*( const T2& a_Left, const Vector<_Count, T1>& a_Right ) noexcept
	{
		Vector<_Count, T1> result{ a_Right };
		for ( size_t i = 0; i < _Count; ++i ) result[i] *= Cast<T1>( a_Left );
		return result;
	}

	template<size_t _Count, Concepts::Arithmetic T1, Concepts::Arithmetic T2>
	[[nodiscard]] constexpr Vector<_Count, T1> operator/( const T2& a_Left, const Vector<_Count, T1>& a_Right ) noexcept
	{
		Vector<_Count, T1> result{ a_Right };
		for ( size_t i = 0; i < _Count; ++i ) result[i] = Cast<T1>( a_Left ) / result[i];
		return result;
	}

	// Comparison Operators

	template<size_t _Count, Concepts::Arithmetic T>
	[[nodiscard]] constexpr bool operator==( const Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		for ( size_t i = 0; i < _Count; ++i )
		{
			if ( a_Left[i] != a_Right[i] ) return false;
		}

		return true;
	}

	template<size_t _Count, Concepts::Arithmetic T>
	[[nodiscard]] constexpr bool operator!=( const Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		return !(a_Left == a_Right);
	}

	template<size_t _Count, Concepts::FloatingPoint T>
	[[nodiscard]] constexpr bool operator<( const Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		return a_Left.LengthSqr() < a_Right.LengthSqr();
	}

	template<size_t _Count, Concepts::FloatingPoint T>
	[[nodiscard]] constexpr bool operator>( const Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		return a_Left.LengthSqr() > a_Right.LengthSqr();
	}

	template<size_t _Count, Concepts::FloatingPoint T>
	[[nodiscard]] constexpr bool operator<=( const Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		return a_Left.LengthSqr() <= a_Right.LengthSqr();
	}

	template<size_t _Count, Concepts::FloatingPoint T>
	[[nodiscard]] constexpr bool operator>=( const Vector<_Count, T>& a_Left, const Vector<_Count, T>& a_Right ) noexcept
	{
		return a_Left.LengthSqr() >= a_Right.LengthSqr();
	}

	template<Concepts::Arithmetic T> using TVector2 = Vector<2, T>;
	template<Concepts::Arithmetic T> using TVector3 = Vector<3, T>;
	template<Concepts::Arithmetic T> using TVector4 = Vector<4, T>;

	using Vector2 = TVector2<float>;
	using Vector3 = TVector3<float>;
	using Vector4 = TVector4<float>;

	using iVector2 = TVector2<int32_t>;
	using iVector3 = TVector3<int32_t>;
	using iVector4 = TVector4<int32_t>;

	using uVector2 = TVector2<uint32_t>;
	using uVector3 = TVector3<uint32_t>;
	using uVector4 = TVector4<uint32_t>;

}


