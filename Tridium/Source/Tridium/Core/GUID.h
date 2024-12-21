#pragma once
#include <Tridium/Core/Types.h>

namespace Tridium {

	template<typename T>
	T GenerateRandomID() { static_assert( false, "GenerateRandomID not implemented for this type" ); }

	template<>
	int16_t GenerateRandomID();

	template<>
	int32_t GenerateRandomID();

	template<>
	int64_t GenerateRandomID();

	template<>
	uint16_t GenerateRandomID();

	template<>
	uint32_t GenerateRandomID();

	template<>
	uint64_t GenerateRandomID();

	template<typename T>
	class UID
	{
	public:
		using Type = T;
		static constexpr T InvalidID = 0;

		UID() : m_ID( InvalidID ) {}
		UID( T a_ID ) : m_ID( a_ID ) {}

		static UID Create()
		{
			T id = GenerateRandomID<T>();
			return id == InvalidID ? Create() : UID( id );
		}

		T ID() const { return m_ID; }
		bool IsValid() const { return m_ID != InvalidID; }

		constexpr operator T() const { return m_ID; }
		constexpr UID& operator=( const T& a_ID ) { m_ID = a_ID; return *this; }
		constexpr UID& operator=( const UID& a_ID ) { m_ID = a_ID.m_ID; return *this; }
		constexpr bool operator==( const T& a_ID ) const { return m_ID == a_ID; }
		constexpr bool operator==( const UID& a_ID ) const { return m_ID == a_ID.m_ID; }
		constexpr bool operator!=( const T& a_ID ) const { return m_ID != a_ID; }
		constexpr bool operator!=( const UID& a_ID ) const { return m_ID != a_ID.m_ID; }
		constexpr bool operator<( const UID& a_ID ) const { return m_ID < a_ID.m_ID; }
		constexpr bool operator>( const UID& a_ID ) const { return m_ID > a_ID.m_ID; }
		constexpr bool operator<=( const UID& a_ID ) const { return m_ID <= a_ID.m_ID; }
		constexpr bool operator>=( const UID& a_ID ) const { return m_ID >= a_ID.m_ID; }

	private:
		T m_ID;
	};

	// Globally-Unique Identifier
	using GUID = UID<uint64_t>;
}

namespace std {
	template<typename T>
	struct hash<Tridium::UID<T>>
	{
		std::size_t operator()( const Tridium::UID<T>& a_ID ) const
		{
			return std::hash<T>()( a_ID.ID() );
		}
	};
}