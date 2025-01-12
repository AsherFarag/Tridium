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

namespace YAML {

	template<>
	struct convert<Tridium::EntityID>
	{
		static Node encode( const Tridium::EntityID& rhs )
		{
			Node node;
			node.push_back( static_cast<Tridium::EntityIDType>( rhs ) );
			return node;
		}

		static bool decode( const Node& node, Tridium::EntityID& rhs )
		{
			rhs = Tridium::EntityID( node.as<Tridium::EntityIDType>() );
			return true;
		}
	};

	YAML::Emitter& operator<<( YAML::Emitter& out, const Tridium::EntityID& v )
	{
		out << static_cast<Tridium::EntityIDType>( v );
		return out;
	};

}

namespace Tridium::IO {

	using namespace entt::literals;
	static const std::unordered_set<Refl::MetaIDType> s_BlacklistedComponents =
	{
		"Component"_hs.value(),
		"NativeScriptComponent"_hs.value(),
		"TransformComponent"_hs.value(),
		"TagComponent"_hs.value(),
		"GUIDComponent"_hs.value(),
	};

	struct DeserializedGameObject
	{
		GameObject GameObject;
		std::optional<EntityID> Parent = {};
		std::vector<EntityID> Children;
	};

	template<>
	struct Serializer<GameObject>
	{
		static void SerializeGameObject( Archive& out, GameObject go );
		static bool DeserializeGameObject( const YAML::Node& a_Node, Scene& a_Scene );
	};

	static void SerializeGameObject( Archive& out, GameObject go )
	{
		Serializer<GameObject>::SerializeGameObject( out, go );
	}

	static bool DeserializeGameObject( const YAML::Node& a_Node, Scene& a_Scene )
	{
		return Serializer<GameObject>::DeserializeGameObject( a_Node, a_Scene );
	}

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
			auto gameObjects = a_Data.GetECS().View<GUIDComponent>();
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

				if ( a_Data.GetSceneEnvironment().HDRI.EnvironmentMapHandle.IsValid() )
				{
					a_Data.GetSceneEnvironment().HDRI.EnvironmentMap = EnvironmentMap::Create( a_Data.GetSceneEnvironment().HDRI.EnvironmentMapHandle );
				}
			}
		}

		auto gameObjectsNode = a_Node["GameObjects"];

		if ( !gameObjectsNode )
			return false;

		for ( auto goNode : gameObjectsNode )
		{
			TE_CORE_ASSERT( DeserializeGameObject( goNode, a_Data ), "Failed to deserialize GameObject from a scene file!" );
		}

		return true;
	}

	void Serializer<GameObject>::SerializeGameObject( Archive& out, GameObject go )
	{
		if ( !go.IsValid() || !go.TryGetComponent<GUIDComponent>() )
		{
			TE_CORE_ASSERT( false, "GameObject is invalid or does not have a GUIDComponent!" )
				return;
		}

		out << YAML::BeginMap; // Begin GameObject

		// GameObject ID
		out << YAML::Key << "GameObject"; out << YAML::Value << go.ID();

		// GUIDComponent
		if ( auto gc = go.TryGetComponent<GUIDComponent>() )
		{
			out << YAML::Key << "GUID" << YAML::Value << gc->GetID();
		}

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
					out << YAML::Value << go.GetParent().ID();
				}

				out << YAML::Key << "Children";
				out << YAML::Value << YAML::Flow << YAML::BeginSeq;
				for ( auto child : go.GetChildren() )
				{
					out << child.ID();
				}
				out << YAML::EndSeq;
			}
			out << YAML::EndMap;
		}

		// Write all components
		for ( auto [type, component] : go.GetAllComponents() )
		{
			if ( s_BlacklistedComponents.contains( type.ID() ) )
				continue;

			out << YAML::Key << type.GetCleanTypeName();
			type.TrySerialize( out, type.FromVoid( component ) );
		}


		out << YAML::EndMap; // End GameObject
	}

	bool Serializer<GameObject>::DeserializeGameObject( const YAML::Node& a_Node, Scene& a_Scene )
	{
		GameObject go;
		if ( auto gameObjectIDNode = a_Node["GameObject"] )
		{
			go = GameObject( gameObjectIDNode.as<EntityID>() );
		}
		else
		{
			return false;
		}

		GUID guid;
		if ( auto guidNode = a_Node["GUID"] )
			guid = guidNode.as<GUID>();
		else
			return false;

		std::string tag;
		if ( auto tagNode = a_Node["Tag"] )
			tag = tagNode.as<std::string>();
		else
			return false;

		TE_CORE_ASSERT( a_Scene.GetECS().CreateEntity( go.ID() ) == go.ID(), "The created GameObject should be the same as the hint!" );
		a_Scene.AddComponentToGameObject<GUIDComponent>( go, guid );
		a_Scene.AddComponentToGameObject<TagComponent>( go, tag );
		a_Scene.AddComponentToGameObject<TransformComponent>( go );

		DeserializedGameObject deserializedGO;
		deserializedGO.GameObject = go;

		if ( auto transformNode = a_Node["Transform"] )
		{
			TransformComponent& tc = a_Scene.GetComponentFromGameObject<TransformComponent>( go );
			tc.Position = transformNode["Position"].as<Vector3>();
			tc.Rotation.SetFromEuler( transformNode["Rotation"].as<Vector3>() );
			tc.Scale = transformNode["Scale"].as<Vector3>();

			if ( auto parentNode = transformNode["Parent"] )
			{
				tc.m_Parent = parentNode.as<EntityID>();
			}

			for ( auto child : transformNode["Children"] )
			{
				tc.m_Children.push_back( child.as<EntityID>() );
			}
		}

		auto it = a_Node.begin();
		++it; // Skip GameObject node
		++it; // Skip GUID node
		++it; // Skip Tag node
		++it; // Skip Transform node

		// Deserialize all components
		for ( ; it != a_Node.end(); ++it )
		{
			auto componentNode = *it;
			auto componentName = componentNode.first.as<std::string>();

			Refl::MetaType componentType = Refl::ResolveMetaType( componentName.c_str() );

			Component* component = nullptr;
			if ( auto func = componentType.GetMetaAttribute<Refl::Props::AddToGameObjectProp::Type>( Refl::Props::AddToGameObjectProp::ID ) )
				component = func.value()( a_Scene, go );

			if ( !component )
			{
				TE_CORE_ERROR( "Failed to deserialize component '{0}' from GameObject", componentName );
				continue;
			}

			Refl::MetaAny componentAsAny = componentType.FromVoid( component );
			if ( !componentType.TryDeserialize( componentNode.second, componentAsAny ) )
			{
				if ( auto func = componentType.GetMetaAttribute<Refl::Props::RemoveFromGameObjectProp::Type>( Refl::Props::RemoveFromGameObjectProp::ID ) )
					func.value()( a_Scene, go );

				TE_CORE_ERROR( "Failed to deserialize component '{0}' from GameObject", componentName );
				continue;
			}
		}

		return true;
	}

}