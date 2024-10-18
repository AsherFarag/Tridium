#pragma once

namespace Tridium {

	class Project;

	class ProjectSerializer
	{
	public:
		ProjectSerializer( const SharedPtr<Project>& a_Project );

		void SerializeText( const std::string& a_Path );
		bool DeserializeText( const std::string& a_Path );

	private:
		SharedPtr<Project> m_Project;
	};

}