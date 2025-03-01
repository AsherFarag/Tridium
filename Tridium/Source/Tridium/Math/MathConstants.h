#pragma once
#include <Tridium/Core/Types.h>

// This namespace contains some common mathematical constants.
namespace Tridium::Math {

	using DefaultGenType = float;

	// PI is a circle constant equal to the ratio of a circle's circumference to its diameter.
	// Approximatly equal to 3.141592653589793
	// Note: In Radians
	template<typename _Gen = DefaultGenType>
	consteval _Gen PI()
	{
		return _Gen( 3.141592653589793238462643383279 );
	}

	// Tau is a circle constant equal to 2pi, the ratio of a circle's circumference to its radius.
	// Approximatly equal to 6.283185307179586
	// Note: In Radians
	template<typename _Gen = DefaultGenType>
	consteval _Gen Tau()
	{
		return PI<_Gen>() * _Gen( 2 );
	}

	// Eular's number
	// An irrational number that is the base of the natural logarithm. ( e )
	// Useful for exponential growth and decay, and for perlin noise.
	// Approximately equal to 2.718281828459045
	template<typename _Gen = DefaultGenType>
	consteval _Gen EularNumber()
	{
		return _Gen( 2.718281828459045235360287471352 );
	}

	// Golden Ratio
	// Associated with the Fibonacci sequence, the golden ratio describes the relationship between two proportions.
	// It is an irrational number that is approximately equal to 1.618033988749895.
	template<typename _Gen = DefaultGenType>
	consteval _Gen GoldenRatio()
	{
		return _Gen( 1.618033988749895 );
	}

	// Speed of light in a vacuum in meters per second.
	// Approximately equal to 299,792,458 m/s
	// Note: 1 unit in Tridium is equal to 1 meter.
	template<typename _Gen = DefaultGenType>
	consteval _Gen SpeedOfLight()
	{
		return _Gen( 299792458 );
	}

	// Gravitational constant in meters cubed per kilogram per second squared.
	// Approximately equal to 6.67430 x 10^-11 m^3 kg^-1 s^-2
	// Note: 1 unit in Tridium is equal to 1 meter.
	template<typename _Gen = DefaultGenType>
	consteval _Gen GravitationalConstant()
	{
		return _Gen( 6.67430e-11 );
	}

} // namespace Tridium::Math