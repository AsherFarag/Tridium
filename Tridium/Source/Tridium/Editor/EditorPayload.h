#pragma once
#include <Tridium/Containers/String.h>
#include <Tridium/Containers/Any.h>

namespace Tridium {

	struct EditorPayload
	{
		String Type;
		Any Data;

		bool IsEmpty() const { return !Data.HasValue(); }

		template<typename T>
		T As()
		{
			return Data.As<T>();
		}
	};

	class EditorPayloadManager
	{
	public:
		EditorPayloadManager() = default;

		const String& GetPayloadType() const { return m_Payload.Type; }
		const EditorPayload& GetPayload() const { return m_Payload; }
		bool HasPayload() const { return !m_Payload.IsEmpty(); }

		EditorPayload* const GetPayload( const String& a_PayloadType )
		{
			if ( m_Payload.Type == a_PayloadType )
			{
				return &m_Payload;
			}

			return nullptr;
		}

		void SetPayload( const String& a_PayloadType, const Any& a_Payload )
		{
			m_Payload.Type = a_PayloadType;
			m_Payload.Data = a_Payload;
		}

		void SetPayload( const EditorPayload& a_Payload )
		{
			m_Payload = a_Payload;
		}

		void ClearPayload()
		{
			m_Payload.Type.clear();
			m_Payload.Data.Reset();
		}

	private:
		EditorPayload m_Payload;
	};

} // namespace Tridiumq