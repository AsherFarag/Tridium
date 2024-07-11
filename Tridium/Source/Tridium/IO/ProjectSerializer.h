#pragma once

namespace Tridium {

	class Project;

	class ProjectSerializer
	{
	public:
		ProjectSerializer( const Ref<Project>& project );

		void SerializeText( const std::string& filepath );
		void SerializeBinary( const std::string& filepath ) {}

		bool DeserializeText( const std::string& filepath );
		bool DeserializeBinary( const std::string& filepath ) { return false; }

	private:
		Ref<Project> m_Project;
	};

}