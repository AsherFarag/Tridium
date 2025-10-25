#pragma once
#include <Tridium/Containers/String.h>
#include <Tridium/Events/Event.h>

namespace Tridium {

	//=================================================================================================
	// Application Layer Interface: A layer represents a modular part of the application that can be
	// attached or detached from the application at runtime.
	// Layers receive events from the top most layer to the bottom layer in the stack.
	//=================================================================================================
	class IAppLayer
	{
	public:

		//=============================================================================================
		IAppLayer( String a_Name ) : m_Name( std::move( a_Name ) ) {}
		virtual ~IAppLayer() = default;

		//=============================================================================================
		const String& Name() const { return m_Name; }

	protected:

		//=============================================================================================
		friend class AppLayerStack;

		//=============================================================================================
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiDraw() {}
		virtual void OnEvent( Event& a_Event ) {}

	protected:

		//=============================================================================================
		String m_Name;

	};

	//=================================================================================================
	// Application Layer Stack: Manages a stack of application layers.
	//=================================================================================================
	class AppLayerStack final
	{
	public:

		//=============================================================================================
		void PushLayer( SharedPtr<IAppLayer> a_Layer );
		void PushOverlay( SharedPtr<IAppLayer> a_Overlay );
		void PopLayer( const SharedPtr<IAppLayer>& a_Layer );
		void PopOverlay( const SharedPtr<IAppLayer>& a_Overlay );

		//=============================================================================================
		template<typename T, typename... Args>
		SharedPtr<T> EmplaceLayer( Args&&... a_Args )
		{
			SharedPtr<T> layer = MakeShared<T>( std::forward<Args>( a_Args )... );
			PushLayer( layer );
			return layer;
		}

		//=============================================================================================
		template<typename T, typename... Args>
		SharedPtr<T> EmplaceOverlay( Args&&... a_Args )
		{
			SharedPtr<T> overlay = MakeShared<T>( std::forward<Args>( a_Args )... );
			PushOverlay( overlay );
			return overlay;
		}

		//=============================================================================================
		auto Begin() { return m_Layers.Begin(); }
		auto End() { return m_Layers.End(); }
		auto RBegin() { return m_Layers.RBegin(); }
		auto REnd() { return m_Layers.REnd(); }

		//=============================================================================================
		size_t Size() const { return m_Layers.Size(); }

		//=============================================================================================
		void OnUpdate( float a_DeltaTime );
		void OnEvent( Event& a_Event );

	private:

		//=============================================================================================
		Array<SharedPtr<IAppLayer>> m_Layers;
		size_t m_LayerInsertIndex = 0;

	};

} // namespace Tridium


