#pragma once

namespace Tridium {

#define INVALID_GUID 0

	typedef uint64_t GUIDType;

	// Globally-Unique Identifier
	class GUID
	{
	public:
		GUID() : m_ID( INVALID_GUID ) {}
		GUID( GUIDType id ) : m_ID( id ) {}

		static GUID Create();
		const GUIDType ID() const { return m_ID; }
		const bool Valid() const { return m_ID != INVALID_GUID; }

		operator GUIDType() const { return m_ID; }
		operator const GUIDType() const { return m_ID; }
		GUID& operator=( const GUIDType& id ) { m_ID = id; return *this; }
		GUID& operator=( const GUID& id ) { m_ID = id.m_ID; return *this; }
		bool operator==( const GUIDType& id ) const { return m_ID == id; }
		bool operator==( const GUID& guid ) const { return m_ID == guid.m_ID; }
		bool operator!=( const GUIDType& id ) const { return m_ID != id; }
		bool operator!=( const GUID& guid ) const { return m_ID != guid.m_ID; }

	private:
		GUIDType m_ID = 0;
	};
}

namespace std {
	template <>
	struct hash<Tridium::GUID>
	{
		std::size_t operator()( const Tridium::GUID& s ) const noexcept 
		{
			return std::hash<Tridium::GUIDType>{}( s.ID() );
		}
	};
}