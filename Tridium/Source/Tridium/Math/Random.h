#pragma once
#include <Tridium/Math/Math.h>
#include <random>

namespace Tridium::Math::Random {

	template<typename T = float>
	T Value()
	{
		static std::random_device rd;  // Obtain a random number from hardware
		static std::mt19937 eng( rd() ); // Seed the generator

		if constexpr (std::is_integral_v<T>) 
		{
			std::uniform_int_distribution<T> distr( std::numeric_limits<T>::min(), std::numeric_limits<T>::max() ); // Define the range
			return distr( eng );
		} 
		else if constexpr (std::is_floating_point_v<T>) 
		{
			std::uniform_real_distribution<T> distr( static_cast<T>(0), static_cast<T>(1) ); // Define the range
			return distr( eng );
		} 
		else 
		{
			static_assert( std::is_arithmetic_v<T>, "T must be an arithmetic type" );
		}

		return T{};
	}

	template<typename T = float>
	T Range( T a_Min, T a_Max )
	{
		static std::random_device rd;  // Obtain a random number from hardware
		static std::mt19937 eng( rd() ); // Seed the generator
		if constexpr (std::is_integral_v<T>) 
		{
			std::uniform_int_distribution<T> distr( a_Min, a_Max ); // Define the range
			return distr( eng );
		} 
		else if constexpr (std::is_floating_point_v<T>) 
		{
			std::uniform_real_distribution<T> distr( a_Min, a_Max ); // Define the range
			return distr( eng );
		} 
		else 
		{
			static_assert( std::is_arithmetic_v<T>, "T must be an arithmetic type" );
		}
		return T{};
	}

}