#include "tripch.h"
#include "SceneSerializer.h"
#include <Tridium/Scene/Scene.h>

#include "SerializationUtil.h"

#include "Tridium/Asset/AssetManager.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Material.h>
#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Rendering/EnvironmentMap.h>

#include <fstream>

namespace Tridium::IO {

	static const std::unordered_set<Refl::MetaIDType> s_BlacklistedComponents =
	{
		"Component"_hs.value(),
		"ScriptableComponent"_hs.value(),
		"TransformComponent"_hs.value(),
		"TagComponent"_hs.value(),
		"GUIDComponent"_hs.value(),
	};

	struct DeserializedGameObject
	{
		GameObject GameObject;
		std::optional<GUID> Parent = {};
		std::vector<GUID> Children;
	};

	void SerializeGameObject( Archive& out, GameObject go );
	bool DeserializeGameObject( const YAML::Node& a_Node, Scene& a_Scene, std::unordered_map<GUID, DeserializedGameObject>& a_DeserializedGameObjects );

	template<>
	void SerializeToText( Archive& a_Archive, const Scene& a_Data )
	{
		a_Archive << YAML::BeginMap;
		a_Archive << YAML::Key << "Scene";
		a_Archive << YAML::Value << a_Data.GetName().c_str();

		a_Archive << YAML::Key << "SceneEnvironment" << YAML::BeginMap;
		{
			a_Archive << YAML::Key << "HDRI" << YAML::BeginMap;
			{
				a_Archive << YAML::Key << "EnvironmentMapHandle";
				a_Archive << YAML::Value << a_Data.GetSceneEnvironment().HDRI.EnvironmentMapHandle;

				a_Archive << YAML::Key << "Exposure";
				a_Archive << YAML::Value << a_Data.GetSceneEnvironment().HDRI.Exposure;

				a_Archive << YAML::Key << "Gamma";
				a_Archive << YAML::Value << a_Data.GetSceneEnvironment().HDRI.Gamma;

				a_Archive << YAML::Key << "Blur";
				a_Archive << YAML::Value << a_Data.GetSceneEnvironment().HDRI.Blur;

				a_Archive << YAML::Key << "Intensity";
				a_Archive << YAML::Value << a_Data.GetSceneEnvironment().HDRI.Intensity;

				a_Archive << YAML::Key << "RotationEular";
				a_Archive << YAML::Value << a_Data.GetSceneEnvironment().HDRI.RotationEular;
			}
			a_Archive << YAML::EndMap; // End HDRI
		}
		a_Archive << YAML::EndMap; // End SceneEnvironment

		a_Archive << YAML::Key << "GameObjects";
		a_Archive << YAML::Value << YAML::BeginSeq;
		{
			auto gameObjects = a_Data.GetRegistry().view<GUIDComponent>();
			for ( auto it = gameObjects.rbegin(); it < gameObjects.rend(); it++ )
			{
				SerializeGameObject( a_Archive, GameObject( *it ) );
			}
		}
		a_Archive << YAML::EndSeq;

		a_Archive << YAML::EndMap;
	}

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, Scene& a_Data )
	{
		if ( auto sceneNameNode = a_Node["Scene"] )
			a_Data.SetName( sceneNameNode.as<std::string>() );
		else
			return false;

		if ( auto sceneEnvironmentNode = a_Node["SceneEnvironment"] )
		{
			if ( auto hdriNode = sceneEnvironmentNode["HDRI"] )
			{
				a_Data.GetSceneEnvironment().HDRI.EnvironmentMapHandle = hdriNode["EnvironmentMapHandle"].as<AssetHandle>();
				a_Data.GetSceneEnvironment().HDRI.Exposure = hdriNode["Exposure"].as<float>();
				a_Data.GetSceneEnvironment().HDRI.Gamma = hdriNode["Gamma"].as<float>();
				a_Data.GetSceneEnvironment().HDRI.Blur = hdriNode["Blur"].as<float>();
				a_Data.GetSceneEnvironment().HDRI.Intensity = hdriNode["Intensity"].as<float>();
				a_Data.GetSceneEnvironment().HDRI.RotationEular = hdriNode["RotationEular"].as<Vector3>();

				if ( a_Data.GetSceneEnvironment().HDRI.EnvironmentMapHandle.Valid() )
				{
					a_Data.GetSceneEnvironment().HDRI.EnvironmentMap = EnvironmentMap::Create( a_Data.GetSceneEnvironment().HDRI.EnvironmentMapHandle );
				}
			}
		}

		auto gameObjectsNode = a_Node["GameObjects"];

		if ( !gameObjectsNode )
			return false;

		std::unordered_map<GUID, DeserializedGameObject> deserializedGameObjects;
		for ( auto goNode : gameObjectsNode )
		{
			TE_CORE_ASSERT( DeserializeGameObject( goNode, a_Data, deserializedGameObjects ), "Failed to deserialize GameObject from a scene file!" );
		}

		// Link up gameobject references to eachother
		for ( auto&& [guid, go] : deserializedGameObjects )
		{
			if ( go.Parent.has_value() )
			{
				if ( auto parent = deserializedGameObjects.find( go.Parent.value() ); parent != deserializedGameObjects.end() )
				{
					TransformComponent& tc = go.GameObject.GetComponent<TransformComponent>();
					tc.AttachToParent( parent->second.GameObject );
				}
			}
		}

		return true;
	}

	void SerializeGameObject( Archive& out, GameObject go )
	{
		out << YAML::BeginMap; // Begin GameObject

		// GUID
		out << YAML::Key << "GameObject"; out << YAML::Value << go.GetGUID().ID();

		// TagComponent
		if ( auto tc = go.TryGetComponent<TagComponent>() )
		{
			out << YAML::Key << "Tag" << YAML::Value << tc->Tag;
		}

		// TransformComponent
		if ( auto tc = go.TryGetComponent<TransformComponent>() )
		{
			out << YAML::Key << "Transform" << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "Position"; out << YAML::Value << tc->Position;
				out << YAML::Key << "Rotation"; out << YAML::Value << tc->Rotation.Euler;
				out << YAML::Key << "Scale";    out << YAML::Value << tc->Scale;

				if ( go.HasParent() )
				{
					out << YAML::Key << "Parent";
					out << YAML::Value << go.GetParent().GetGUID().ID();
				}

				if ( go.HasParent() )
				{
					out << YAML::Key << "Parent";
					out << YAML::Value << go.GetParent().GetGUID().ID();
				}

				out << YAML::Key << "Children";
				out << YAML::Value << YAML::Flow << YAML::BeginSeq;
				for ( auto child : go.GetChildren() )
				{
					out << child.GetGUID().ID();
				}
				out << YAML::EndSeq;
			}
			out << YAML::EndMap;
		}

		// Write all components
		for ( auto [type, component] : go.GetAllComponents() )
		{
			if ( s_BlacklistedComponents.contains( type.id() ) )
				continue;

			out << YAML::Key << Refl::MetaRegistry::GetCleanTypeName( type );

			Refl::Internal::SerializeFunc serFunc;
			if ( Refl::MetaRegistry::TryGetMetaPropertyFromClass( type, serFunc, Refl::Internal::YAMLSerializeFuncID ) )
			{
				serFunc( out, type.from_void( component ) );
			}
		}


		out << YAML::EndMap; // End GameObject
	}

	bool DeserializeGameObject( const YAML::Node& a_Node, Scene& a_Scene, std::unordered_map<GUID, DeserializedGameObject>& a_DeserializedGameObjects )
	{
		GUID guid;
		if ( auto guidNode = a_Node["GameObject"] )
			guid = guidNode.as<GUID>();
		else
			return false;

		std::string tag;
		if ( auto tagNode = a_Node["Tag"] )
			tag = tagNode.as<std::string>();
		else
			return false;

		GameObject go = a_Scene.InstantiateGameObject( guid, tag );
		DeserializedGameObject deserializedGO;
		deserializedGO.GameObject = go;

		if ( auto transformNode = a_Node["Transform"] )
		{
			TransformComponent& tc = go.GetTransform();
			tc.Position = transformNode["Position"].as<Vector3>();
			tc.Rotation.SetFromEuler( transformNode["Rotation"].as<Vector3>() );
			tc.Scale = transformNode["Scale"].as<Vector3>();

			if ( auto parentNode = transformNode["Parent"] )
			{
				deserializedGO.Parent = parentNode.as<GUID>();
			}

			for ( auto child : transformNode["Children"] )
			{
				deserializedGO.Children.push_back( child.as<GUID>() );
			}
		}

		a_DeserializedGameObjects.emplace( guid, std::move( deserializedGO ) );

		auto it = a_Node.begin();
		++it; // Skip GameObject node
		++it; // Skip Tag node
		++it; // Skip Transform node

		// Deserialize all components
		for ( ; it != a_Node.end(); ++it )
		{
			auto componentNode = *it;
			auto componentName = componentNode.first.as<std::string>();

			Refl::MetaType componentType = Refl::MetaRegistry::ResolveMetaType( entt::hashed_string( componentName.c_str() ) );

			Refl::Internal::AddToGameObjectFunc addToGameObjectFunc;
			if ( !Refl::MetaRegistry::TryGetMetaPropertyFromClass( componentType, addToGameObjectFunc, Refl::Internal::AddToGameObjectPropID ) )
			{
				TE_CORE_ERROR( "Failed to deserialize component '{0}' from GameObject", componentName );
				continue;
			}

			Refl::Internal::DeserializeFunc deserFunc;
			if ( !Refl::MetaRegistry::TryGetMetaPropertyFromClass( componentType, deserFunc, Refl::Internal::YAMLDeserializeFuncID ) )
			{
				TE_CORE_ERROR( "Failed to deserialize component '{0}' from GameObject", componentName );
				continue;
			}

			Component* component = addToGameObjectFunc( go );
			if ( !component )
			{
				TE_CORE_ERROR( "Failed to deserialize component '{0}' from GameObject", componentName );
				continue;
			}

			Refl::MetaAny componentAsAny = componentType.from_void( component );
			deserFunc( componentNode.second, componentAsAny );
		}

		return true;
	}

}