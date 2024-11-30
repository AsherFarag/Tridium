#pragma once
#if IS_EDITOR
#include "EditorLayer.h"
#include "Tridium/Core/Core.h"
#include "EditorEvents.h"
#include <any>

namespace Tridium::Editor {

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

	class EditorApplication
	{
	public:
		static SharedPtr<EditorApplication> Get() { return s_Instance; }
		static bool Init();
		static bool Shutdown();

		static PayloadManager& GetPayloadManager() { return s_Instance->m_PayloadManager; }

		EditorLayer* GetEditorLayer() { return m_EditorLayer; }

	private:
		EditorLayer* m_EditorLayer;
		PayloadManager m_PayloadManager;

	private: // Singleton
		EditorApplication();
		EditorApplication( const EditorApplication& ) = delete;
		EditorApplication& operator=( const EditorApplication& ) = delete;

		static SharedPtr<EditorApplication> s_Instance;
		static std::once_flag s_InitFlag;
	};

	inline EditorLayer* GetEditorLayer() { return EditorApplication::Get()->GetEditorLayer(); }
	inline SharedPtr<Scene> GetActiveScene() { return EditorApplication::Get()->GetEditorLayer()->GetActiveScene();}
};

#endif // IS_EDITOR