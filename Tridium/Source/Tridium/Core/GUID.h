#pragma once

namespace Tridium {

	// Globally-Unique Identifier
	class GUID
	{
	public:
		using Type = uint64_t;

		static constexpr Type InvalidGUID = 0;

		GUID() : m_ID( InvalidGUID ) {}
		GUID( Type a_ID ) : m_ID( a_ID ) {}

		static GUID Null() { return GUID(); }
		static GUID Create();
		const Type ID() const { return m_ID; }
		const bool Valid() const { return m_ID != InvalidGUID; }

		operator Type() const { return m_ID; }
		operator const Type() const { return m_ID; }
		GUID& operator=( const Type& id ) { m_ID = id; return *this; }
		GUID& operator=( const GUID& id ) { m_ID = id.m_ID; return *this; }
		bool operator==( const Type& id ) const { return m_ID == id; }
		bool operator==( const GUID& guid ) const { return m_ID == guid.m_ID; }
		bool operator!=( const Type& id ) const { return m_ID != id; }
		bool operator!=( const GUID& guid ) const { return m_ID != guid.m_ID; }
		bool operator<( const GUID& guid ) const { return m_ID < guid.m_ID; }
		bool operator>( const GUID& guid ) const { return m_ID > guid.m_ID; }
		bool operator<=( const GUID& guid ) const { return m_ID <= guid.m_ID; }
		bool operator>=( const GUID& guid ) const { return m_ID >= guid.m_ID; }

	private:
		Type m_ID;
	};
}

namespace std {
	template <>
	struct hash<Tridium::GUID>
	{
		std::size_t operator()( const Tridium::GUID& s ) const noexcept 
		{
			return std::hash<Tridium::GUID::Type>{}( s.ID() );
		}
	};
}