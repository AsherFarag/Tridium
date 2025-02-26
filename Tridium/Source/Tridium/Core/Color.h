#pragma once
#include <Tridium/Math/Math.h>
#include <glm/gtx/string_cast.hpp>

namespace Tridium {

	template<typename T> requires std::is_arithmetic_v<T>
	struct BasicColor
	{
		T r, g, b, a;

		constexpr BasicColor() : r( T(0) ), g( T(0) ), b( T(0) ), a( T(1) ) {}
		constexpr BasicColor( T a_Scalar, T a_Alpha = T(1) ) : r( a_Scalar ), g( a_Scalar ), b( a_Scalar ), a( a_Alpha ) {}
		constexpr BasicColor( T a_Red, T a_Green, T a_Blue, T a_Alpha = T(1) ) : r( a_Red ), g( a_Green ), b( a_Blue ), a( a_Alpha ) {}
		constexpr BasicColor( const Vector4& a_Vec ) : r( a_Vec.r ), g( a_Vec.g ), b( a_Vec.b ), a( a_Vec.a ) {}
		constexpr BasicColor( const Vector3& a_Vec ) : r( a_Vec.r ), g( a_Vec.g ), b( a_Vec.b ), a( T(1) ) {}
		constexpr BasicColor( const BasicColor& a_Other ) = default;
		constexpr BasicColor& operator=( const BasicColor& a_Other ) = default;

		constexpr operator Vector4() const { return Vector4( r, g, b, a ); }
		constexpr operator Vector3() const { return Vector3( r, g, b ); }

		constexpr const T& operator[]( size_t a_Index ) const { return reinterpret_cast<const T*>( this )[a_Index]; }
		constexpr T& operator[]( size_t a_Index )
		{
			static_assert( sizeof( BasicColor ) == 4 * sizeof( T ), "BasicColor layout must be tightly packed." );
			return reinterpret_cast<T*>( this )[a_Index];
		}


		constexpr BasicColor& operator+=( const BasicColor& a_Other ) { r += a_Other.r; g += a_Other.g; b += a_Other.b; a += a_Other.a; return *this; }
		constexpr BasicColor& operator-=( const BasicColor& a_Other ) { r -= a_Other.r; g -= a_Other.g; b -= a_Other.b; a -= a_Other.a; return *this; }
		constexpr BasicColor& operator*=( const BasicColor& a_Other ) { r *= a_Other.r; g *= a_Other.g; b *= a_Other.b; a *= a_Other.a; return *this; }
		constexpr BasicColor& operator/=( const BasicColor& a_Other ) { r /= a_Other.r; g /= a_Other.g; b /= a_Other.b; a /= a_Other.a; return *this; }

		constexpr BasicColor& operator+=( T a_Scalar ) { r += a_Scalar; g += a_Scalar; b += a_Scalar; a += a_Scalar; return *this; }
		constexpr BasicColor& operator-=( T a_Scalar ) { r -= a_Scalar; g -= a_Scalar; b -= a_Scalar; a -= a_Scalar; return *this; }
		constexpr BasicColor& operator*=( T a_Scalar ) { r *= a_Scalar; g *= a_Scalar; b *= a_Scalar; a *= a_Scalar; return *this; }
		constexpr BasicColor& operator/=( T a_Scalar ) { r /= a_Scalar; g /= a_Scalar; b /= a_Scalar; a /= a_Scalar; return *this; }

		constexpr BasicColor operator+( const BasicColor& a_Other ) const { return BasicColor( r + a_Other.r, g + a_Other.g, b + a_Other.b, a + a_Other.a ); }
		constexpr BasicColor operator-( const BasicColor& a_Other ) const { return BasicColor( r - a_Other.r, g - a_Other.g, b - a_Other.b, a - a_Other.a ); }
		constexpr BasicColor operator*( const BasicColor& a_Other ) const { return BasicColor( r * a_Other.r, g * a_Other.g, b * a_Other.b, a * a_Other.a ); }
		constexpr BasicColor operator/( const BasicColor& a_Other ) const { return BasicColor( r / a_Other.r, g / a_Other.g, b / a_Other.b, a / a_Other.a ); }

		template<typename U> constexpr BasicColor operator+( U a_Scalar ) const { return BasicColor( r + a_Scalar, g + a_Scalar, b + a_Scalar, a + a_Scalar ); }
		template<typename U> constexpr BasicColor operator-( U a_Scalar ) const { return BasicColor( r - a_Scalar, g - a_Scalar, b - a_Scalar, a - a_Scalar ); }
		template<typename U> constexpr BasicColor operator*( U a_Scalar ) const { return BasicColor( r * a_Scalar, g * a_Scalar, b * a_Scalar, a * a_Scalar ); }
		template<typename U> constexpr BasicColor operator/( U a_Scalar ) const { return BasicColor( r / a_Scalar, g / a_Scalar, b / a_Scalar, a / a_Scalar ); }

		constexpr bool operator==( const BasicColor& other ) const { return r == other.r && g == other.g && b == other.b && a == other.a; }
		constexpr bool operator!=( const BasicColor& other ) const { return !( *this == other ); }

		// HSV
		static BasicColor FromHSV( T a_Hue, T a_Saturation, T a_Value, T a_Alpha = T( 1 ) )
		{
			T c = a_Value * a_Saturation;
			T x = c * ( 1 - std::abs( std::fmod( a_Hue / 60, 2 ) - 1 ) );
			T m = a_Value - c;
			T r, g, b;
			if ( a_Hue < 60 ) { r = c; g = x; b = 0; }
			else if ( a_Hue < 120 ) { r = x; g = c; b = 0; }
			else if ( a_Hue < 180 ) { r = 0; g = c; b = x; }
			else if ( a_Hue < 240 ) { r = 0; g = x; b = c; }
			else if ( a_Hue < 300 ) { r = x; g = 0; b = c; }
			else { r = c; g = 0; b = x; }
			return { r + m, g + m, b + m, a_Alpha };
		}

		void ToHSV( T& o_Hue, T& o_Saturation, T& o_Value ) const
		{
			T max = std::max( r, std::max( g, b ) );
			T min = std::min( r, std::min( g, b ) );
			T delta = max - min;
			o_Value = max;
			if ( delta == T( 0 ) )
			{
				o_Hue = T( 0 );
				o_Saturation = T( 0 );
				return;
			}
			if ( max == r ) o_Hue = 60 * std::fmod( ( g - b ) / delta, 6 );
			else if ( max == g ) o_Hue = 60 * ( ( b - r ) / delta + 2 );
			else o_Hue = 60 * ( ( r - g ) / delta + 4 );
			o_Saturation = delta / max;
		}

		// Predefined colors
		static constexpr BasicColor White()  { return { T(1), T(1), T(1), T(1) }; }
		static constexpr BasicColor Black()  { return { T(0), T(0), T(0), T(1) }; }
		static constexpr BasicColor Red()    { return { T(1), T(0), T(0), T(1) }; }
		static constexpr BasicColor Green()  { return { T(0), T(1), T(0), T(1) }; }
		static constexpr BasicColor Blue()   { return { T(0), T(0), T(1), T(1) }; }
	};

	using Color = BasicColor<float>;

}

// Hot fix for glm::to_string not being defined in the glm namespace
namespace glm {
	template<>
	GLM_FUNC_QUALIFIER std::string to_string( Tridium::Color const& x )
	{
		return glm::to_string( static_cast<glm::vec4>( x ) );
	}
}