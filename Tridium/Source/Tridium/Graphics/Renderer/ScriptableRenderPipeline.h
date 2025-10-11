#pragma once
#include <Tridium/Graphics/Renderer/RenderPipeline.h>

namespace Tridium {

	//=============================================================================================
	// Scriptable Render Context:
	//=============================================================================================
	class ScriptableRenderContext
	{

	};

	//=============================================================================================
	// Scriptable Render Pipeline Interface (SRP):
	// A flexible rendering pipeline that allows for custom rendering workflows.
	// Users can define their own render passes, manage resources, and control the rendering process.
	//=============================================================================================
	class IScriptableRenderPipeline : public IRenderPipeline
	{
	public:

		//=========================================================================================
		IScriptableRenderPipeline() = default;
		~IScriptableRenderPipeline() override = default;

		//=========================================================================================
		void AddView( const RenderView& a_View )
		{
			IRenderPipeline::AddView( a_View ); 
		}

	protected:

		//=========================================================================================
		UniquePtr<class IRenderPipeline> Create() const = 0;

		//=========================================================================================
		void Render() override final 
		{ 
			ScriptableRenderContext context;
			Render( context ); 
		}

		//=========================================================================================
		virtual void Render( ScriptableRenderContext& a_Context ) = 0;

	};

} // namespace Tridium