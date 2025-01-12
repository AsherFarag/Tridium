#pragma once
#include <unordered_map>

namespace Tridium {

	template<typename _Key, typename _Value>
	class BidirectionalMap
	{
		template<typename _Key, typename _Value>
		using StorageType = std::unordered_map<_Key, _Value>;

	public:
		void Insert( const _Key& key, const _Value& value )
		{
			m_KeyToValue[key] = value;
			m_ValueToKey[value] = key;
		}

		const _Value& GetValue( const _Key& key ) const
		{
			return m_KeyToValue.at( key );
		}

		const _Key& GetKey( const _Value& value ) const
		{
			return m_ValueToKey.at( value );
		}

		bool ContainsKey( const _Key& key ) const
		{
			return m_KeyToValue.find( key ) != m_KeyToValue.end();
		}

		bool ContainsValue( const _Value& value ) const
		{
			return m_ValueToKey.find( value ) != m_ValueToKey.end();
		}

		void Erase( const _Key& key )
		{
			const auto value = m_KeyToValue.at( key );
			m_KeyToValue.erase( key );
			m_ValueToKey.erase( value );
		}

		void Erase( const _Value& value )
		{
			const auto key = m_ValueToKey.at( value );
			m_ValueToKey.erase( value );
			m_KeyToValue.erase( key );
		}

		void Clear()
		{
			m_KeyToValue.clear();
			m_ValueToKey.clear();
		}

		size_t Size() const
		{
			return m_KeyToValue.size();
		}

		_Key* FindKey( const _Value& value )
		{
			if ( auto it = m_ValueToKey.find( value ); it != m_ValueToKey.end() )
				return &it->second;

			return nullptr;
		}

		_Value* FindValue( const _Key& key )
		{
			if ( auto it = m_KeyToValue.find( key ); it != m_KeyToValue.end() )
				return &it->second;

			return nullptr;
		}

		const _Key* FindKey( const _Value& value ) const
		{
			if ( auto it = m_ValueToKey.find( value ); it != m_ValueToKey.end() )
				return &it->second;

			return nullptr;
		}

		const _Value* FindValue( const _Key& key ) const
		{
			if ( auto it = m_KeyToValue.find( key ); it != m_KeyToValue.end() )
				return &it->second;

			return nullptr;
		}

		//////////////////////////////////////////////////////////////////////////

		auto begin() const
		{
			return m_KeyToValue.begin();
		}

		auto end() const
		{
			return m_KeyToValue.end();
		}

		//////////////////////////////////////////////////////////////////////////

	private:
		StorageType<_Key, _Value> m_KeyToValue;
		StorageType<_Value, _Key> m_ValueToKey;
	};

}