#include "tripch.h"
#include "SceneSerializer.h"

#include "SerializationUtil.h"

#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Rendering/Texture.h>

#include <fstream>

namespace Tridium {

	struct DeserializedGameObject
	{
		GameObject GameObject;
		std::optional<GUID> Parent = {};
		std::vector<GUID> Children;
	};

	SceneSerializer::SceneSerializer( const Ref<Scene>& scene )
		: m_Scene(scene)
	{

	}

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

			out << YAML::Key << "Mesh";
			if ( auto mesh = MeshLibrary::GetMesh( mc->GetMesh() ) )
				out << YAML::Value << YAML::DoubleQuoted << mesh->GetPath();
			else
				out << YAML::Value << YAML::DoubleQuoted << "";


			out << YAML::EndMap;
		}

		if ( auto sc = go.TryGetComponent<SpriteComponent>() )
		{
			out << YAML::Key << "SpriteComponent";
			out << YAML::BeginMap;

			out << YAML::Key << "Texture";
			if ( auto tex = TextureLibrary::GetTexture( sc->GetTexture() ) )
				out << YAML::Value << YAML::DoubleQuoted << tex->GetPath();
			else
				out << YAML::Value << YAML::DoubleQuoted << "";

			out << YAML::EndMap;
		}

		if ( auto lsc = go.TryGetComponent<LuaScriptComponent>() )
		{
			out << YAML::Key << "LuaScriptComponent";
			out << YAML::BeginMap;

			out << YAML::Key << "LuaScript"; out << YAML::Value << YAML::DoubleQuoted << lsc->GetScript()->GetFilePath().string();

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
		if ( auto tagComponent = go["TagComponent"] )
			name = tagComponent.as<std::string>();

		auto deserialisedGO = GameObject::Create( id, name );
		deserializedGameObject.GameObject = deserialisedGO;

		if ( auto transformComponent = go["TransformComponent"] )
		{
			TransformComponent* tc;  
			if ( !( tc = deserialisedGO.TryGetComponent<TransformComponent>() ) )
				tc = &deserialisedGO.AddComponent<TransformComponent>();

			tc->Position = transformComponent["Position"].as<Vector3>();
			tc->Rotation = glm::radians( transformComponent["Rotation"].as<Vector3>() );
			tc->Scale = transformComponent["Scale"].as<Vector3>();

			if ( auto parent = transformComponent["Parent"] )
				deserializedGameObject.Parent = parent.as<GUID>();

			if ( auto children = transformComponent["Children"] )
			{
				deserializedGameObject.Children.reserve( children.size() );
				for ( auto child : children )
				{
					deserializedGameObject.Children.push_back( child.as<GUID>() );
				}
			}
		}

		if ( auto meshComponent = go["MeshComponent"] )
		{
			MeshComponent* mc;
			if ( !( mc = deserialisedGO.TryGetComponent<MeshComponent>() ) )
				mc = &deserialisedGO.AddComponent<MeshComponent>();

			if ( auto mesh = meshComponent["Mesh"] )
			{
				MeshHandle meshHandle;
				auto meshFilePath = mesh.as<std::string>();

				if ( !MeshLibrary::GetMeshHandle( meshFilePath, meshHandle ) )
				{
					meshHandle = MeshHandle::Create();
					if ( Ref<Mesh> loadedMesh = MeshLoader::Import( meshFilePath ) )
					{
						loadedMesh->_SetHandle( meshHandle );
						if ( !MeshLibrary::AddMesh( meshFilePath, loadedMesh ) )
							meshHandle = {};
					}
				}

				mc->SetMesh( meshHandle );
			}
		}

		if ( auto spriteComponent = go["SpriteComponent"] )
		{
			SpriteComponent* sc;
			if ( !( sc = deserialisedGO.TryGetComponent<SpriteComponent>() ) )
				sc = &deserialisedGO.AddComponent<SpriteComponent>();

			if ( auto tex = spriteComponent["Texture"] )
			{
				TextureHandle textureHandle;
				auto texFilePath = tex.as<std::string>();
				textureHandle = TextureLibrary::GetTextureHandle( texFilePath );
				if ( !textureHandle.Valid() )
				{
					textureHandle = TextureHandle::Create();
					if ( Ref<Texture> loadedTex = TextureLoader::Import( texFilePath ) )
					{
						loadedTex->_SetHandle( textureHandle );
						if ( !TextureLibrary::AddTexture( texFilePath, loadedTex ) )
							textureHandle = {};
					}
				}

				sc->SetTexture( textureHandle );
			}
		}

		if ( auto luaScriptComponent = go["LuaScriptComponent"] )
		{
			LuaScriptComponent* lsc;
			if ( !( lsc = deserialisedGO.TryGetComponent<LuaScriptComponent>() ) )
				lsc = &deserialisedGO.AddComponent<LuaScriptComponent>();

			lsc->SetScript( Script::Create( luaScriptComponent["LuaScript"].as<std::string>() ) );
		}

		if ( auto cameraComponent = go["CameraComponent"] )
		{
			CameraComponent* cc;
			if ( !( cc = deserialisedGO.TryGetComponent<CameraComponent>() ) )
				cc = &deserialisedGO.AddComponent<CameraComponent>();

			if ( auto projectionType = cameraComponent["Projection Mode"] )
			{
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

	void SceneSerializer::SerializeText( const std::string& filepath )
	{
		TE_CORE_TRACE( "Begin Serializing Scene" );
		TE_CORE_TRACE( "Serializing to \"{0}\"", filepath );

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene";
		out << YAML::Value << m_Scene->m_Name.c_str();

		out << YAML::Key << "GameObjects";
		out << YAML::Value << YAML::BeginSeq;
		{
			auto view = m_Scene->m_Registry.view<TagComponent>();
			for ( auto it = view.rbegin(); it < view.rend(); it++ )
			{
				SerializeGameObject( out, *it );
			}
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;

		std::ofstream outFile( filepath );
		outFile << out.c_str();

		TE_CORE_TRACE( "Finished Serializing Scene" );
	}

	void SceneSerializer::SerializeBinary( const std::string& filepath )
	{
	}

	bool SceneSerializer::DeserializeText( const std::string& filepath )
	{

		YAML::Node data = YAML::LoadFile( filepath );
		if ( !data["Scene"] )
			return false;

		m_Scene->m_Name = data["Scene"].as<std::string>();
		TE_CORE_TRACE( "Begin Deserializing Scene '{0}'", m_Scene->m_Name );

		auto gameObjects = data["GameObjects"];
		std::unordered_map<GUID, DeserializedGameObject> deserializedGameObjects;
		deserializedGameObjects.reserve( gameObjects.size() );
		for ( auto go : gameObjects )
		{
			DeserializedGameObject deserializedGO;
			if ( DeserializeGameObject( go, deserializedGO ) )
				deserializedGameObjects.insert( { deserializedGO.GameObject.GetGUID(), deserializedGO } );
		}

		// Link up gameobject references to eachother
		for ( auto&& [guid, go] : deserializedGameObjects )
		{
			auto& tc = go.GameObject.GetComponent<TransformComponent>();
			if ( go.Parent.has_value() )
				tc.SetParent( deserializedGameObjects[go.Parent.value()].GameObject );

			tc.m_Children.reserve( go.Children.size() );
			for ( auto& childGUID : go.Children )
			{
				tc.m_Children.push_back( deserializedGameObjects[childGUID].GameObject);
			}
		}

		TE_CORE_TRACE( "Finished Deserializing Scene" );
		return true;
	}

	bool SceneSerializer::DeserializeBinary( const std::string& filepath )
	{
		return false;
	}
}