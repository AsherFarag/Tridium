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

#include <fstream>

namespace Tridium::IO {

	template<>
	void SerializeToText( Archive& a_Archive, const Scene& a_Data )
	{
		a_Archive << YAML::BeginMap;
		a_Archive << YAML::Key << "Scene";
		a_Archive << YAML::Value << a_Data.GetName().c_str();

		a_Archive << YAML::Key << "GameObjects";
		a_Archive << YAML::Value << YAML::BeginSeq;
		{
			auto view = a_Data.GetRegistry().view<GUIDComponent>();
			for ( auto it = view.rbegin(); it < view.rend(); it++ )
			{
				//SerializeGameObject( a_Archive, *it );
			}
		}
		a_Archive << YAML::EndSeq;

		a_Archive << YAML::EndMap;
	}

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, Scene& a_Data )
	{
		return false;
	}

}

namespace Tridium {

	struct DeserializedGameObject
	{
		GameObject GameObject;
		std::optional<GUID> Parent = {};
		std::vector<GUID> Children;
	};

	void SerializeGameObject( YAML::Emitter& out, GameObject go )
	{
		out << YAML::BeginMap;

		out << YAML::Key << "GameObject"; out << YAML::Value << go.GetGUID().ID();

		if ( auto tc = go.TryGetComponent<TagComponent>() )
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			out << YAML::Value << YAML::DoubleQuoted << tc->Tag;

			out << YAML::EndMap;
		}

		if ( auto tc = go.TryGetComponent<TransformComponent>() )
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			out << YAML::Key << "Position"; out << YAML::Value << tc->Position;
			out << YAML::Key << "Rotation"; out << YAML::Value << glm::degrees( tc->Rotation ); 
			out << YAML::Key << "Scale";    out << YAML::Value << tc->Scale;

			if ( go.HasParent() )
			{
				out << YAML::Key << "Parent";
				out << YAML::Value << go.GetParent().GetGUID().ID();
			}

			out << YAML::Key << "Children";
			out << YAML::Flow; out << YAML::Value << YAML::BeginSeq;
			for ( auto child : go.GetChildren() )
			{
				out << child.GetGUID().ID();
			}
			out << YAML::EndSeq;

			out << YAML::EndMap;
		}

		if ( auto mc = go.TryGetComponent<MeshComponent>() )
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;

			out << YAML::Key << "Mesh" << YAML::Value << mc->GetMesh().GetAssetHandle();
			out << YAML::Key << "Material" << YAML::Value << mc->GetMaterial().GetAssetHandle();

			out << YAML::EndMap;
		}

		if ( auto sc = go.TryGetComponent<SpriteComponent>() )
		{
			out << YAML::Key << "SpriteComponent";
			out << YAML::BeginMap;

			out << YAML::Key << "Texture" << YAML::Value << sc->GetTexture().GetAssetHandle();

			out << YAML::EndMap;
		}

		if ( auto lsc = go.TryGetComponent<LuaScriptComponent>() )
		{
			out << YAML::Key << "LuaScriptComponent";
			out << YAML::BeginMap;

			//out << YAML::Key << "LuaScript" << YAML::Value << YAML::DoubleQuoted << ( lsc->GetScript() ? lsc->GetScript()->GetFilePath().ToString() : "" );

			out << YAML::EndMap;
		}

		if ( auto cc = go.TryGetComponent<CameraComponent>() )
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			out << YAML::Key << "Projection Mode";
			out << YAML::Value << ( cc->SceneCamera.GetProjectionType() == Camera::ProjectionType::Perspective ? "Perspective" : "Orthographic" );

			out << YAML::Key << "Perspective";
			out << YAML::BeginMap;

			out << YAML::Key << "FOV"; out << YAML::Value << cc->SceneCamera.GetPerspectiveFOV();
			out << YAML::Key << "Near"; out << YAML::Value << cc->SceneCamera.GetPerspectiveNearClip();
			out << YAML::Key << "Far"; out << YAML::Value << cc->SceneCamera.GetPerspectiveFarClip();

			out << YAML::EndMap;

			out << YAML::Key << "Orthographic";
			out << YAML::BeginMap;

			out << YAML::Key << "Size"; out << YAML::Value << cc->SceneCamera.GetOrthographicSize();
			out << YAML::Key << "Near"; out << YAML::Value << cc->SceneCamera.GetOrthographicNearClip();
			out << YAML::Key << "Far"; out << YAML::Value << cc->SceneCamera.GetOrthographicFarClip();

			out << YAML::EndMap;

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	bool DeserializeGameObject( YAML::detail::iterator_value& go, DeserializedGameObject& deserializedGameObject )
	{
		GUID id;
		if ( auto gameObject = go["GameObject"] )
			id = gameObject.as<GUID>();
		else
			return false;

		std::string name;
		if ( auto tagComponent = go["TagComponent"] ) {
			name = tagComponent.as<std::string>();
		}

		auto deserialisedGO = GameObject::Create( id, name );
		deserializedGameObject.GameObject = deserialisedGO;

		if ( auto transformComponent = go["TransformComponent"] )
		{
			TransformComponent* tc;  
			if ( !( tc = deserialisedGO.TryGetComponent<TransformComponent>() ) ) {
				tc = &deserialisedGO.AddComponent<TransformComponent>();
			}

			tc->Position = transformComponent["Position"].as<Vector3>();
			tc->Rotation = glm::radians( transformComponent["Rotation"].as<Vector3>() );
			tc->Scale = transformComponent["Scale"].as<Vector3>();

			if ( auto parent = transformComponent["Parent"] ) {
				deserializedGameObject.Parent = parent.as<GUID>();
			}

			if ( auto children = transformComponent["Children"] )
			{
				deserializedGameObject.Children.reserve( children.size() );
				for ( auto child : children ) {
					deserializedGameObject.Children.push_back( child.as<GUID>() );
				}
			}
		}

		if ( auto meshComponent = go["MeshComponent"] )
		{
			MeshComponent* mc;
			if ( !( mc = deserialisedGO.TryGetComponent<MeshComponent>() ) ) {
				mc = &deserialisedGO.AddComponent<MeshComponent>();
			}

			if ( auto mesh = meshComponent["Mesh"] ) {
				mc->SetMesh( AssetManager::GetAsset<Mesh>( mesh.as<GUID>() ) );
			}

			if ( auto mat = meshComponent["Material"] ) {
				mc->SetMaterial( AssetManager::GetAsset<Material>( mat.as<GUID>() ) );
			}
		}

		if ( auto spriteComponent = go["SpriteComponent"] )
		{
			SpriteComponent* sc;
			if ( !( sc = deserialisedGO.TryGetComponent<SpriteComponent>() ) ) {
				sc = &deserialisedGO.AddComponent<SpriteComponent>();
			}

			if ( auto tex = spriteComponent["Texture"] ) {
				sc->SetTexture( AssetManager::GetAsset<Texture>( tex.as<GUID>() ) );
			}
		}

		if ( auto luaScriptComponent = go["LuaScriptComponent"] )
		{
			LuaScriptComponent* lsc;
			if ( !( lsc = deserialisedGO.TryGetComponent<LuaScriptComponent>() ) ) {
				lsc = &deserialisedGO.AddComponent<LuaScriptComponent>();
			}

			//lsc->SetScript( Script::Create( luaScriptComponent["LuaScript"].as<std::string>() ) );
		}

		if ( auto cameraComponent = go["CameraComponent"] )
		{
			CameraComponent* cc;
			if ( !( cc = deserialisedGO.TryGetComponent<CameraComponent>() ) ) {
				cc = &deserialisedGO.AddComponent<CameraComponent>();
			}

			if ( auto projectionType = cameraComponent["Projection Mode"] ) {
				cc->SceneCamera.SetProjectionType( projectionType.as<std::string>() == "Perspective" ? Camera::ProjectionType::Perspective : Camera::ProjectionType::Orthographic );
			}

			if ( auto perspective = cameraComponent["Perspective"] )
			{
				cc->SceneCamera.SetPerspectiveFOV( perspective["FOV"].as<float>() );
				cc->SceneCamera.SetPerspectiveNearClip( perspective["Near"].as<float>() );
				cc->SceneCamera.SetPerspectiveFarClip( perspective["Far"].as<float>() );
			}

			if ( auto orthographic = cameraComponent["Orthographic"] )
			{
				cc->SceneCamera.SetOrthographicSize( orthographic["Size"].as<float>() );
				cc->SceneCamera.SetOrthographicNearClip( orthographic["Near"].as<float>() );
				cc->SceneCamera.SetOrthographicFarClip( orthographic["Far"].as<float>() );
			}
		}

		return true;
	}

	//bool SceneSerializer::DeserializeText( const std::string& filepath )
	//{
	//	YAML::Node data = YAML::LoadFile( filepath );
	//	if ( !data["Scene"] )
	//		return false;

	//	m_Scene->m_Path = filepath;

	//	m_Scene->m_Name = data["Scene"].as<std::string>();
	//	TE_CORE_TRACE( "Begin Deserializing Scene '{0}'", m_Scene->m_Name );

	//	auto gameObjects = data["GameObjects"];
	//	std::unordered_map<GUID, DeserializedGameObject> deserializedGameObjects;
	//	deserializedGameObjects.reserve( gameObjects.size() );
	//	for ( auto go : gameObjects )
	//	{
	//		DeserializedGameObject deserializedGO;
	//		if ( DeserializeGameObject( go, deserializedGO ) )
	//			deserializedGameObjects.insert( { deserializedGO.GameObject.GetGUID(), deserializedGO } );
	//	}

	//	// Link up gameobject references to eachother
	//	for ( auto&& [guid, go] : deserializedGameObjects )
	//	{
	//		auto& tc = go.GameObject.GetComponent<TransformComponent>();
	//		if ( go.Parent.has_value() )
	//			tc.SetParent( deserializedGameObjects[go.Parent.value()].GameObject );

	//		tc.m_Children.reserve( go.Children.size() );
	//		for ( auto& childGUID : go.Children )
	//		{
	//			tc.m_Children.push_back( deserializedGameObjects[childGUID].GameObject);
	//		}
	//	}

	//	TE_CORE_TRACE( "Finished Deserializing Scene" );
	//	return true;
	//}
}