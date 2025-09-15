#pragma once
#include <Tridium/Core/Assert.h>
#include <Tridium/Core/Types.h>
#include <Tridium/Utils/Concepts.h>

// Container Implementations
#include <Tridium/Containers/Span.h>
#include <Tridium/Containers/Array.h>
#include <Tridium/Containers/String.h>
#include <bit> // for std::endian

namespace Tridium {

	inline constexpr uint16_t SwapEndian16( const uint16_t v )
	{
		return ( v >> 8 ) | ( v << 8 );
	}

	inline uint32_t SwapEndian32( const uint32_t a_Value )
	{
	#if defined(_MSC_VER) // MSVC
		return _byteswap_ulong( a_Value );
	#else // GCC/Clang
		return __builtin_bswap32( a_Value );
	#endif
	}

	inline uint64_t SwapEndian64( const uint64_t a_Value )
	{
	#if defined(_MSC_VER) // MSVC
		return _byteswap_uint64( a_Value );
	#else // GCC/Clang
		return __builtin_bswap64( a_Value );
	#endif
	}

	template<typename T>
	constexpr T SwapEndian( T a_Value )
	{
		if constexpr ( sizeof( T ) == 1 ) return a_Value;
		else if constexpr ( sizeof( T ) == 2 ) return ReinterpretCast<T>( SwapEndian16( ReinterpretCast<uint16_t>( a_Value ) ) );
		else if constexpr ( sizeof( T ) == 4 ) return ReinterpretCast<T>( SwapEndian32( ReinterpretCast<uint32_t>( a_Value ) ) );
		else if constexpr ( sizeof( T ) == 8 ) return ReinterpretCast<T>( SwapEndian64( ReinterpretCast<uint64_t>( a_Value ) ) );
		else static_assert( std::_Always_false<T>, "swapEndian only supports types of size 1, 2, 4, or 8 bytes." );
		return a_Value;
	}

	template<typename T>
	constexpr T CpuToLE( T a_Value )
	{
		if constexpr ( std::endian::native == std::endian::little ) return a_Value;
		else return SwapEndian( a_Value );
	}

	template<typename T>
	constexpr T CpuToBE( T a_Value )
	{
		if constexpr ( std::endian::native == std::endian::big ) return a_Value;
		else return SwapEndian( a_Value );
	}

	//=================================================================================================
	// Stream Reader: Template struct for reading specific types from an input stream.
	// Specialize this struct for each type you want to support.
	//=================================================================================================
	template<typename T>
	struct StreamReader
	{
		static bool Read( class IInputStream&, T& )
		{
			static_assert( std::_Always_false<T>, "No StreamReader defined for this type." );
			return false;
		}
	};

	//=================================================================================================
	// Input Stream Interface: Interface for reading data from a stream.
	// A stream can be a file, memory buffer, network socket, etc.
	//=================================================================================================
	class IInputStream
	{
	public:

		//=================================================================================================
		virtual ~IInputStream() = default;

		//=================================================================================================
		// Returns true if the stream is valid and can be read from.
		virtual bool Valid() const = 0;

		//=================================================================================================
		// Returns the current position in the stream that will be read from (in bytes from the start).
		virtual size_t StreamPosition() = 0;

		//=================================================================================================
		// Seeks to the specified position in the stream. Returns true on success.
		virtual bool Seek( size_t a_Position ) = 0;

		//=================================================================================================
		// Reads raw data from the stream and writes it into 'a_Destination'. Returns true on success.
		virtual bool ReadRaw( byte_t* a_Destination, size_t a_Size ) = 0;

		template<typename T>
		bool Read( T& a_Value )
		{
			return StreamReader<T>().Read( *this, a_Value );
		}

		template<typename T>
		auto& operator>>( T& a_Value )
		{
			ASSERT( Read( a_Value ) );
			return *this;
		}
	};

	//=================================================================================================
	// Stream Writer: Template struct for writing specific types to an output stream.
	// Specialize this struct for each type you want to support.
	//=================================================================================================
	template<typename T>
	struct StreamWriter
	{
		static bool Write( class IOutputStream&, const T& )
		{
			static_assert( std::_Always_false<T>, "No StreamWriter defined for this type." );
			return false;
		}
	};

	//=================================================================================================
	// Output Stream Interface: Interface for writing data to a stream.
	// A stream can be a file, memory buffer, network socket, etc.
	//=================================================================================================
	class IOutputStream
	{
	public:

		//=================================================================================================
		virtual ~IOutputStream() = default;

		//=================================================================================================
		// Returns true if the stream is valid and can be written to.
		virtual bool Valid() const = 0;

		//=================================================================================================
		// Returns the current position in the stream that will be written to (in bytes from the start).
		virtual size_t StreamPosition() = 0;

		//=================================================================================================
		// Seeks to the specified position in the stream. Returns true on success.
		virtual bool Seek( size_t a_Position ) = 0;

		//=================================================================================================
		// Writes raw data to the stream. Returns true on success.
		virtual bool WriteRaw( const byte_t* a_Data, size_t a_Size ) = 0;

		template<typename T>
		bool Write( const T& a_Value )
		{
			return StreamWriter<T>().Write( *this, a_Value );
		}

		template<typename T>
		auto& operator<<( const T& a_Value )
		{
			ASSERT( Write( a_Value ) );
			return *this;
		}

	};

	//=================================================================================================
	// Default Stream Reader Implementations
	//=================================================================================================

	template<Concepts::Integral T>
	struct StreamReader<T>
	{
		static bool Read( IInputStream& a_Stream, T& a_Value )
		{
			T leValue = 0;

			if ( !a_Stream.ReadRaw( ReinterpretCast<byte_t*>( &leValue ), sizeof( leValue ) ) )
			{
				return false;
			}

			a_Value = CpuToLE( leValue );

			return true;
		}
	};

	template<Concepts::FloatingPoint T>
	struct StreamReader<T>
	{
		static bool Read( IInputStream& a_Stream, T& a_Value )
		{
			// Assumes IEEE 754 float/double. Endianness matters for cross-platform.
			if constexpr ( std::endian::native == std::endian::little )
			{
				return a_Stream.ReadRaw( ReinterpretCast<byte_t*>( &a_Value ), sizeof( T ) );
			}
			else
			{
				// swap bytes for big-endian hosts
				T swapped;
				std::byte* src = ReinterpretCast< std::byte* >( &swapped );
				std::byte* dst = ReinterpretCast< std::byte* >( &a_Value );
				if ( !a_Stream.ReadRaw( ReinterpretCast<byte_t*>( &swapped ), sizeof( T ) ) )
				{
					return false;
				}
				for ( size_t i = 0; i < sizeof( T ); i++ )
				{
					dst[ i ] = src[ sizeof( T ) - 1 - i ];
				}
				return true;
			}
		}
	};

	template<typename T>
	struct StreamReader<BasicString<T>>
	{
		static bool Read( IInputStream& a_Stream, BasicString<T>& a_Value )
		{
			size_t size = 0;

			if ( !a_Stream.Read( size ) )
			{
				return false;
			}

			a_Value.resize( size );

			if ( size > 0 )
			{
				if ( !a_Stream.ReadRaw( ReinterpretCast<byte_t*>( a_Value.data() ), size * sizeof( T ) ) )
				{
					return false;
				}
			}

			return true;
		}
	};

	template<typename T>
	struct StreamReader<Array<T>>
	{
		static bool Read( IInputStream& a_Stream, Array<T>& a_Value )
		{
			size_t size = 0;

			if ( !a_Stream.Read( size ) )
			{
				return false;
			}

			a_Value.resize( size );

			for ( T& element : a_Value )
			{
				if ( !a_Stream.Read( element ) )
				{
					return false;
				}
			}

			return true;
		}
	};

	//=================================================================================================
	// Default Stream Writer Implementations
	//=================================================================================================

	template<Concepts::Integral T>
	struct StreamWriter<T>
	{
		static bool Write( IOutputStream& a_Stream, const T& value )
		{
			const T leValue = CpuToLE( value );
			return a_Stream.WriteRaw( ReinterpretCast<const byte_t* >( &leValue ), sizeof( leValue ) );
		}
	};

	template<Concepts::FloatingPoint T>
	struct StreamWriter<T>
	{
		static bool Write( IOutputStream& a_Stream, const T& value )
		{
			// Assumes IEEE 754 float/double. Endianness matters for cross-platform.
			if constexpr ( std::endian::native == std::endian::little )
			{
				return a_Stream.WriteRaw( ReinterpretCast<const byte_t*>( &value ), sizeof( T ) );
			}
			else
			{
				// swap bytes for big-endian hosts
				T swapped;
				std::byte* src = ReinterpretCast< std::byte* >( const_cast< T* >( &value ) );
				std::byte* dst = ReinterpretCast< std::byte* >( &swapped );

				for ( size_t i = 0; i < sizeof( T ); i++ )
				{
					dst[ i ] = src[ sizeof( T ) - 1 - i ];
				}

				return a_Stream.WriteRaw( ReinterpretCast< const byte_t* >( &swapped ), sizeof( T ) );
			}
		}
	};

	template<typename T> requires Concepts::IsConvertible<T, StringView>
	struct StreamWriter<T>
	{
		static bool Write( IOutputStream& a_Stream, const T* a_Value )
		{
			StringView view = a_Value;

			const size_t size = view.size();

			if ( !a_Stream.Write( size ) )
			{
				return false;
			}

			if ( size > 0 )
			{
				if ( !a_Stream.WriteRaw( ReinterpretCast<const byte_t*>( view.data() ), size * sizeof( typename T::value_type ) ) )
				{
					return false;
				}
			}

			return true;
		}
	};

	template<typename T> requires Concepts::IsConvertible<T, WStringView>
	struct StreamWriter<T>
	{
		static bool Write( IOutputStream& a_Stream, const T* a_Value )
		{
			WStringView view = a_Value;
			const size_t size = view.size();

			if ( !a_Stream.Write( size ) )
			{
				return false;
			}

			if ( size > 0 )
			{
				if ( !a_Stream.WriteRaw( ReinterpretCast<const byte_t*>( view.data() ), size * sizeof( typename T::value_type ) ) )
				{
					return false;
				}
			}

			return true;
		}
	};

	template<size_t N>
	struct StreamWriter<char[ N ]>
	{
		static bool Write( IOutputStream& a_Stream, const char( &a_Value )[ N ] )
		{
			const size_t size = Cast< size_t >( N - 1 ); // exclude null terminator

			if ( !a_Stream.Write( size ) )
			{
				return false;
			}

			return a_Stream.WriteRaw( ReinterpretCast<const byte_t*>( a_Value ), size );
		}
	};

	template<size_t N>
	struct StreamWriter<const char[ N ]>
	{
		static bool Write( IOutputStream& a_Stream, const char( &a_Value )[ N ] )
		{
			const size_t size = Cast< size_t >( N - 1 ); // exclude null terminator

			if ( !a_Stream.Write( size ) )
			{
				return false;
			}

			return a_Stream.WriteRaw( ReinterpretCast<const byte_t*>( a_Value ), size );
		}
	};

	template<typename T>
	struct StreamWriter<Span<T>>
	{
		static bool Write( IOutputStream& a_Stream, const Span<T>& a_Value )
		{
			if ( !a_Stream.Write( a_Value.size() ) )
			{
				return false;
			}

			for ( const T& element : a_Value )
			{
				if ( !a_Stream.Write( element ) )
				{
					return false;
				}
			}

			return true;
		}
	};

	template<typename T> requires ( Concepts::Pod<T> && !Concepts::IsPointer<T> && !Concepts::Integral<T> && !Concepts::FloatingPoint<T> )
	struct StreamWriter<T>
	{
		static bool Write( IOutputStream& a_Stream, const T& a_Value )
		{
			return a_Stream.WriteRaw( ReinterpretCast<const byte_t*>( &a_Value ), sizeof( T ) );
		}
	};

} // namespace Tridium