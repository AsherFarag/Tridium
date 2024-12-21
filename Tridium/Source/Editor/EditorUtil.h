#pragma once
#if IS_EDITOR
#include <Tridium/ImGui/ImGui.h>
#include <Tridium/Core/Application.h>

namespace Tridium::Editor::Util {

	typedef std::function<void( const std::string& )> FileDialogCallback;

	class FileDialogLayer : public Layer
	{
	public:
		FileDialogLayer( FileDialogCallback a_Callback, const std::string& a_DefaultPath );
		virtual void OnImGuiDraw() {};
	protected:
		FileDialogCallback m_Callback;
		std::string m_FilePath;
	};

	class SaveFileDialogLayer : public FileDialogLayer
	{
	public:
		SaveFileDialogLayer( FileDialogCallback a_Callback, const std::string& a_DefaultPath )
			: FileDialogLayer( a_Callback, a_DefaultPath ) {}
		virtual void OnImGuiDraw() override;
	};

	inline void OpenSaveFileDialog( const std::string& a_DefaultFilePath, FileDialogCallback a_Callback )
	{
		Application::Get().PushOverlay( new SaveFileDialogLayer( a_Callback, a_DefaultFilePath ) );
	}

	class LoadFileDialogLayer : public FileDialogLayer
	{
	public:
		LoadFileDialogLayer( FileDialogCallback a_Callback, const std::string& a_DefaultPath )
			: FileDialogLayer( a_Callback, a_DefaultPath ) {}
		virtual void OnImGuiDraw() override;
	};

	inline void OpenLoadFileDialog( const std::string& a_DefaultFilePath, FileDialogCallback a_Callback )
	{
		Application::Get().PushOverlay( new LoadFileDialogLayer( a_Callback, a_DefaultFilePath ) );
	}

	class NewFileDialogLayer : public FileDialogLayer
	{
	public:
		NewFileDialogLayer( const std::string& a_FileTypeName, FileDialogCallback a_Callback, const std::string& a_DefaultPath );
		virtual void OnImGuiDraw() override;

	protected:
		std::string m_FileTypeName;
	};

	inline void OpenNewFileDialog( const std::string& a_FileTypeName, const std::string& a_DefaultFilePath, FileDialogCallback a_Callback )
	{
		Application::Get().PushOverlay( new NewFileDialogLayer( a_FileTypeName, a_Callback, a_DefaultFilePath ) );
	}
};

#endif // IS_EDITOR