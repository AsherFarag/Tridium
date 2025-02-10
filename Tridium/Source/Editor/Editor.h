#pragma once
#if IS_EDITOR
#include "EditorConfig.h"
#include "EditorLayer.h"
#include "Tridium/Core/Core.h"
#include "EditorEvents.h"
#include <any>

namespace Tridium {

	struct Payload
	{
		std::string Type;
		std::any Data;

		bool IsEmpty() const { return !Data.has_value(); }

		template<typename T>
		T As()
		{
			return std::any_cast<T>( Data );
		}
	};

	class PayloadManager
	{
	public:
		PayloadManager() = default;

		const std::string& GetPayloadType() const { return m_Payload.Type; }
		const Payload& GetPayload() const { return m_Payload; }
		bool HasPayload() const { return !m_Payload.IsEmpty(); }

		Payload* const GetPayload( const std::string& a_PayloadType )
		{
			if ( m_Payload.Type == a_PayloadType )
			{
				return &m_Payload;
			}

			return nullptr;
		}

		void SetPayload( const std::string& a_PayloadType, const std::any& a_Payload )
		{
			m_Payload.Type = a_PayloadType;
			m_Payload.Data = a_Payload;
		}

		void SetPayload( const Payload& a_Payload )
		{
			m_Payload = a_Payload;
		}

		void ClearPayload()
		{
			m_Payload.Type.clear();
			m_Payload.Data.reset();
		}

	private:
		Payload m_Payload;
	};

	//////////////////////////////////////////////////////////////////////////

	class EditorApplication final : public ISingleton<EditorApplication, true, true, true>
	{
	public:
		bool Init();
		bool Shutdown();

		PayloadManager& GetPayloadManager() { return Get()->m_PayloadManager; }

		EditorLayer* GetEditorLayer() { return m_EditorLayer; }

	private:
		EditorLayer* m_EditorLayer;
		PayloadManager m_PayloadManager;
	};

	//=================================================================================================
	// Editor Namespace
	//  A namespace for editor specific functions and utilities
	namespace Editor {
		bool Init( EditorConfig a_Config );
		void Shutdown();

		inline PayloadManager* GetPayloadManager() { return EditorApplication::Get() ? &EditorApplication::Get()->GetPayloadManager() : nullptr; }
		inline EditorLayer* GetEditorLayer() { return EditorApplication::Get() ? EditorApplication::Get()->GetEditorLayer() : nullptr; }
	}
	//=================================================================================================
};

#endif // IS_EDITOR