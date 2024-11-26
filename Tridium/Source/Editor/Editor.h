#pragma once
#if IS_EDITOR
#include "EditorLayer.h"
#include "Tridium/Core/Core.h"
#include <any>

namespace Tridium::Editor {

	class PayloadManager
	{
	public:
		PayloadManager() = default;

		const std::string& GetPayloadType() const { return m_PayloadType; }
		const std::any& GetPayload() const { return m_Payload; }
		bool HasPayload() const { return !m_PayloadType.empty(); }

		std::any GetPayload( const std::string& a_PayloadType ) const
		{
			if ( m_PayloadType == a_PayloadType )
			{
				return m_Payload;
			}

			return std::any();
		}

		void SetPayload( const std::string& a_PayloadType, const std::any& a_Payload )
		{
			m_PayloadType = a_PayloadType;
			m_Payload = a_Payload;
		}

		void ClearPayload()
		{
			m_PayloadType.clear();
			m_Payload.reset();
		}

	private:
		std::string m_PayloadType;
		std::any m_Payload;
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