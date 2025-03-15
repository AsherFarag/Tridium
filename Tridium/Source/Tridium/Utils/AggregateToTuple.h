#pragma once
#include "Concepts.h"
#include "MapMacro.h"
#include <Tridium/Containers/Tuple.h>
#include <Tridium/Core/Types.h>

namespace Tridium {

	// Converts a struct to a tuple.
	// Returns a tuple with the values of the struct.
	// The struct must be aggregate initializable.
	// Note: This does copy the values of the struct into the tuple.
	// Example:
	// struct Aggregate
	// {
	// 	float a;
	// 	int b;
	// };
	//
	// Aggregate a{ 1.0f, 2 };
	// std::tuple<float, int> t = ToTuple( a );
	template<Concepts::IsStruct T>
	constexpr auto ToTuple( T const& a_Struct );

	template<Concepts::IsStruct T>
	constexpr auto ToPtrTuple( T const& a_Struct );

	// Calls a_Func on each field of a_Struct.
	// Note: This uses ToTuple which creates a single copy of the struct.
	// _Func can be either
	//  - void( auto& ) where the field value is passed by reference ( can be const )
	//  - void( size_t, auto& ) where size_t is the field index and the field value is passed by reference ( can be const )
	//
	// Example:
	// struct Aggregate
	// {
	// 	float a;
	// 	int b;
	// };
	//
	// Aggregate a{ 1.0f, 2 };
	// ForEachField( a, []( auto& field )
	// {
	// 	std::cout << field << std::endl;
	// } );
	//
	// ForEachField( a, []( size_t index, auto& field )
	// {
	// 	std::cout << index << ": " << field << std::endl;
	// } );
	template<Concepts::IsStruct T, typename _Func>
	constexpr void ForEachField( T const& a_Struct, _Func&& a_Func );



	///////////////////////////////////////////////////////////////////////////////////////////



	namespace Detail {

		struct AnyType 
		{
			template<class T>
			constexpr operator T() const;
		};

		template<size_t I>
		using IndexedAnyType = AnyType;

		template<Concepts::Aggregate T, typename Indices>
		struct AggregateInitializableFromIndices;

		template<Concepts::Aggregate T, std::size_t... Is>
		struct AggregateInitializableFromIndices<T, std::index_sequence<Is...>> : std::bool_constant<Concepts::IsAggregateInitializable<T, IndexedAnyType<Is>...>>
		{
		};

		template<typename T, size_t N>
		concept AggregateInitializibleWithNArgs = Concepts::Aggregate<T> && AggregateInitializableFromIndices<T, std::make_index_sequence<N>>::value;

		template<Concepts::Aggregate T, size_t N, bool _CanInit>
		struct FieldCount : FieldCount<T, N + 1, AggregateInitializibleWithNArgs<T, N + 1>>
		{
		};

		template<Concepts::Aggregate T, size_t N>
		struct FieldCount<T, N, false> : std::integral_constant<size_t, N - 1>
		{
		};

		template<Concepts::Aggregate T>
		constexpr size_t FieldCountV = FieldCount<T, 0, AggregateInitializibleWithNArgs<T, 0>>::value;


#define CONCAT(a, b) a##b
#define GENERATE_VAR(n) CONCAT(t, n)
#define STRUCT_BIND_START auto [
#define STRUCT_BIND_PTR_START auto& [
#define STRUCT_BIND_END ] = Struct;

#define STRUCT_BIND_1  t1
#define STRUCT_BIND_2  t1, t2
#define STRUCT_BIND_3  t1, t2, t3
#define STRUCT_BIND_4  t1, t2, t3, t4
#define STRUCT_BIND_5  t1, t2, t3, t4, t5
#define STRUCT_BIND_6  t1, t2, t3, t4, t5, t6
#define STRUCT_BIND_7  t1, t2, t3, t4, t5, t6, t7
#define STRUCT_BIND_8  t1, t2, t3, t4, t5, t6, t7, t8
#define STRUCT_BIND_9  t1, t2, t3, t4, t5, t6, t7, t8, t9
#define STRUCT_BIND_10 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10
#define STRUCT_BIND_11 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11
#define STRUCT_BIND_12 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12
#define STRUCT_BIND_13 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13
#define STRUCT_BIND_14 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14
#define STRUCT_BIND_15 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15
#define STRUCT_BIND_16 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16
#define STRUCT_BIND_17 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17
#define STRUCT_BIND_18 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18
#define STRUCT_BIND_19 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19
#define STRUCT_BIND_20 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19, t20

#define GET_BIND_MACRO(n) CONCAT(STRUCT_BIND_, n)

#define ADDR_OF(x) &x
#define LIST_TO_ADDR(...) MAP(ADDR_OF, __VA_ARGS__)
#define GET_BIND_MACRO_PTR(n) LIST_TO_ADDR( CONCAT(STRUCT_BIND_, n) )

#define STRUCT_BIND(N)                                                                                                                                         \
  STRUCT_BIND_START GET_BIND_MACRO(N)                                                                                                                          \
  STRUCT_BIND_END                                                                                                                                              \
  return std::make_tuple(GET_BIND_MACRO(N));

#define GET_NTH_ELEMENT(I, N)                                                                                                                                  \
  STRUCT_BIND_START GET_BIND_MACRO(N)                                                                                                                          \
  STRUCT_BIND_END                                                                                                                                              \
  return std::get<I>(std::make_tuple(GET_BIND_MACRO(N)));

#define STRUCT_BIND_PTR(N)                                                                                                                                     \
  STRUCT_BIND_PTR_START GET_BIND_MACRO(N)                                                                                                                      \
  STRUCT_BIND_END                                                                                                                                              \
  return std::make_tuple

#define STRUCT_UNPACKER(N)                                                                                                                                     \
  template<typename T>                                                                                                                                         \
  struct StructUnpacker<T, N>                                                                                                                                  \
  {                                                                                                                                                            \
    static constexpr auto ToTuple(T Struct)                                                                                                                    \
	{                                                                                                                                                          \
	  STRUCT_BIND(N);                                                                                                                                          \
	}                                                                                                                                                          \
	static constexpr auto ToPtrTuple(T Struct)                                                                                                                 \
	{                                                                                                                                                          \
	}                                                                                                                                                          \
    template<int I>                                                                                                                                            \
    static constexpr auto GetNthElement(T Struct)                                                                                                              \
    {                                                                                                                                                          \
      GET_NTH_ELEMENT(I, N)                                                                                                                                    \
    }                                                                                                                                                          \
  };

	template<typename T, size_t N>
	struct StructUnpacker
	{
		static_assert( N != 0, "Can't have 0 elements in a struct" );
	};

	STRUCT_UNPACKER( 1 )
	STRUCT_UNPACKER( 2 )
	STRUCT_UNPACKER( 3 )
	STRUCT_UNPACKER( 4 )
	STRUCT_UNPACKER( 5 )
	STRUCT_UNPACKER( 6 )
	STRUCT_UNPACKER( 7 )
	STRUCT_UNPACKER( 8 )
	STRUCT_UNPACKER( 9 )
	STRUCT_UNPACKER( 10 )
	STRUCT_UNPACKER( 11 )
	STRUCT_UNPACKER( 12 )
	STRUCT_UNPACKER( 13 )
	STRUCT_UNPACKER( 14 )
	STRUCT_UNPACKER( 15 )
	STRUCT_UNPACKER( 16 )
	STRUCT_UNPACKER( 17 )
	STRUCT_UNPACKER( 18 )
	STRUCT_UNPACKER( 19 )
	STRUCT_UNPACKER( 20 )


		template<typename... _Args, size_t... Is>
		constexpr auto StructToTupleImpl( std::tuple<_Args...> const& a_Tuple, std::index_sequence<Is...> const& )
		{
			#define GET_TYPE(x) std::remove_cvref_t<decltype(x)>
			#define CURR_ELEM std::get<Is>(a_Tuple)
			#define CURR_ELEM_TYPE GET_TYPE(CURR_ELEM)

			return std::make_tuple(
				[&]()
				{
					if constexpr ( Concepts::IsStruct<CURR_ELEM_TYPE> )
					{
						return StructToTupleImpl( StructUnpacker<CURR_ELEM_TYPE, FieldCountV<CURR_ELEM_TYPE>>::ToTuple( CURR_ELEM ),
							std::make_index_sequence<FieldCountV<CURR_ELEM_TYPE>> {} );
					}
					//else if constexpr ( Detail::IsSpecialisation<CURR_ELEM_TYPE, std::vector> )
					//{
					//	#define VECTOR_TYPE typename CURR_ELEM_TYPE::value_type
					//	if constexpr ( Concepts::IsStruct<VECTOR_TYPE> )
					//	{
					//		return StructToTupleVectorImpl < VECTOR_TYPE,
					//			decltype( StructToTupleImpl( StructUnpacker<VECTOR_TYPE, FieldCountV<VECTOR_TYPE>> {}( std::declval<VECTOR_TYPE>() ),
					//				std::make_index_sequence<FieldCountV<VECTOR_TYPE>> {} ) ) > ( CURR_ELEM );
					//	}
					//	else
					//	{
					//		return CURR_ELEM;
					//	}
					//	#undef VECTOR_TYPE
					//}
					else
					{
						return CURR_ELEM;
					}
				}()...);

			#undef CURR_ELEM
			#undef CURR_ELEM_TYPE
		}

		// Helper to detect if _Func accepts an index
		template <typename _Func, typename T>
		constexpr bool AcceptsIndex() {
			if constexpr ( std::is_invocable_v<_Func, size_t, T> ) {
				return true;  // _Func(size_t, auto) form
			}
			else {
				return false; // _Func(auto) form
			}
		}

	} // namespace Detail

	template<Concepts::IsStruct T>
	constexpr auto ToTuple( T const& a_Struct )
	{
		constexpr size_t fieldCount = Detail::FieldCountV<T>;
		return Detail::StructToTupleImpl( Detail::StructUnpacker<T, fieldCount>::ToTuple( a_Struct ), std::make_index_sequence<fieldCount> {} );
	}

	template<Concepts::IsStruct T>
	constexpr auto ToPtrTuple( T const& a_Struct )
	{
		constexpr size_t fieldCount = Detail::FieldCountV<T>;
		return Detail::StructToTupleImpl( Detail::StructUnpacker<T, fieldCount>::ToTuple( a_Struct ), std::make_index_sequence<fieldCount> {} );
	}

	template<Concepts::IsStruct T, typename _Func>
	constexpr void ForEachField( T const& a_Struct, _Func&& a_Func )
	{
		auto fields = ToTuple( a_Struct );

		std::apply( [&]( auto&&... args ) 
		{
			size_t index = 0;
			if constexpr ( ( Detail::AcceptsIndex<_Func, decltype( args )>() && ... ) )
			{
				// If function expects (index, value)
				( ( a_Func( index++, args ) ), ... );
			}
			else
			{
				// If function expects just (value)
				( a_Func( args ), ... );
			}
		}, fields );
	}

}