#pragma once
#include <Tridium/Scene/Scene.h>
#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/yaml.h"

namespace Tridium {

	class SceneSerializer
	{
	public:
		SceneSerializer( const Ref<Scene>& scene );

		void SerializeText( const std::string& filepath );
		void SerializeBinary( const std::string& filepath );

		bool DeserializeText( const std::string& filepath );
		bool DeserializeBinary( const std::string& filepath );

	private:
		static void SerializeGameObject( YAML::Emitter& out, GameObject go );
		static void DeserializeGameObject( YAML::detail::iterator_value& go );

	private:
		Ref<Scene> m_Scene;
	};

}