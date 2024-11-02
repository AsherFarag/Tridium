#pragma once

namespace Tridium {

	class Project;

	class ProjectSerializer
	{
	public:
		ProjectSerializer( const SharedPtr<Project>& a_Project );

		void SerializeText( const IO::FilePath& a_Path );
		bool DeserializeText( const IO::FilePath& a_Path );

	private:
		SharedPtr<Project> m_Project;
	};

}