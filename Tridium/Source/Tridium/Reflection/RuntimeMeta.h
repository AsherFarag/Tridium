#pragma once
#include <Tridium/Core/Config.h>
#include <Tridium/ECS/Registry.h>
#include <Tridium/Editor/UserActions/SceneActions.h>
#include <Tridium/Reflection/Meta.h>
#include <Tridium/Scene/Component.h>
#include <Tridium/UI/PropertyDrawers.h>
#include <Tridium/Utils/StaticInitializer.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>

namespace Tridium {

	using MetaContext = entt::meta_ctx;
	using MetaSequenceContainer = entt::meta_sequence_container;
	using MetaAssociativeContainer = entt::meta_associative_container;
	using MetaAny = entt::meta_any;
	using MetaHandle = entt::meta_handle;
	using MetaCustom = entt::meta_custom;
	using MetaField = entt::meta_data;
	using MetaFunction = entt::meta_func;
	using MetaType = entt::meta_type;

	template<typename T>
	using MetaFactory = entt::meta_factory<T>;

} // namespace Tridium

namespace Tridium::Meta {

	//=================================================================================================
	// This macro registers a type into the runtime reflection system.
	// This allows the type to be discovered and used at runtime for things like
	// serialization, scripting, editor integration, etc.
	// NOTE: The type must be Reflectable 
	// (i.e., have a Reflector specialization or be an aggregate type) to be registered.
	#define REGISTER_TYPE( _Type ) \
		DECLARE_INITIALIZER( __LINE__ ) \
		DEFINE_INITIALIZER( __LINE__ ) \
		{ \
			::Tridium::Meta::RegisterType<_Type>(); \
		}

	//=================================================================================================
	// Runtime Meta Info: Contains metadata used by various runtime systems such as scripting,
	// serialization, and the editor.
	//=================================================================================================
	struct RuntimeMetaInfo
	{
		struct
		{
			bool IsComponent = false;
			void*( *EmplaceOrReplace )( EntityComponentRegistry& a_Registry, Entity a_Entity ) = nullptr;
			void*( *TryGet )( EntityComponentRegistry& a_Registry, Entity a_Entity ) = nullptr;
			bool( *Has )( EntityComponentRegistry& a_Registry, Entity a_Entity ) = nullptr;
			void( *Remove )( EntityComponentRegistry& a_Registry, Entity a_Entity ) = nullptr;
		} Component;

		struct
		{

		} Script;

		struct
		{
		} Serialization;

	#if WITH_EDITOR
		struct
		{
			StringView DisplayName;
			bool( *Draw )( StringView a_Label, void* a_Instance ) = nullptr;
			struct
			{
				bool HideInInspector = false;
				UniquePtr<IUserAction>( *CreateUserAction )( GameObject, EComponentUserActionType );
			} Component;

		} Editor;
	#endif
	};

	//=================================================================================================
	template<typename T>
	constexpr RuntimeMetaInfo CreateRuntimeMetaInfo()
	{
		RuntimeMetaInfo info{};

		// Component Info
		if ( Concepts::Derived<T, Component> )
		{
			info.Component.IsComponent = true;

			info.Component.EmplaceOrReplace = []( EntityComponentRegistry& a_Registry, Entity a_Entity ) -> void*
			{
				return &a_Registry.EmplaceOrReplace<T>( a_Entity );
			};

			info.Component.TryGet = []( EntityComponentRegistry& a_Registry, Entity a_Entity ) -> void*
			{
				return a_Registry.TryGet<T>( a_Entity );
			};

			info.Component.Has = []( EntityComponentRegistry& a_Registry, Entity a_Entity ) -> bool
			{
				return a_Registry.AnyOf<T>( a_Entity );
			};

			info.Component.Remove = []( EntityComponentRegistry& a_Registry, Entity a_Entity )
			{
				a_Registry.Remove<T>( a_Entity );
			};
		}

		// Script Info
		{
		}

		// Serialization Info
		{
		}

	#if WITH_EDITOR

		info.Editor.DisplayName = GetStrippedTypeName<T>();

		if constexpr ( HasReflector<T> )
		{
			constexpr auto type = GetType<T>();

			if constexpr ( type.template Has<DisplayName>() )
			{
				info.Editor.DisplayName = type.template Get<DisplayName>().Value;
			}

			if constexpr ( type.template Has<HideInInspector>() )
			{
				info.Editor.Component.HideInInspector = true;
			}
		}

		info.Editor.Draw = []( StringView a_Label, void* a_Instance ) -> bool
		{
			T& instance = *Cast<T*>( a_Instance );
			return UIPropertyDrawer<T>::Draw( a_Label, instance, false );
		};

		if constexpr ( Concepts::Derived<T, Component> )
		{
			info.Editor.Component.CreateUserAction = +[]( GameObject a_GameObject, EComponentUserActionType a_ActionType ) -> UniquePtr<IUserAction>
			{
				if ( !a_GameObject.Valid() )
				{
					return nullptr;
				}

				if ( a_ActionType == EComponentUserActionType::Add )
				{
					return MakeUnique<ComponentUserAction<T>>( a_GameObject.Scene()->ID(), a_GameObject.Entity() );
				}
				else if ( T* data = a_GameObject.TryGet<T>() )
				{
					return MakeUnique<ComponentUserAction<T>>( a_GameObject.Scene()->ID(), a_GameObject.Entity(), a_ActionType, *data );
				}
				else
				{
					return nullptr;
				}
			};
		}

	#endif

		return info;
	}

	//=================================================================================================
	template<Reflectable T>
	void RegisterType()
	{
		entt::meta_factory<T>()
			.custom<RuntimeMetaInfo>( CreateRuntimeMetaInfo<T>() );
	}

	//=================================================================================================
	template<Reflectable T>
	const RuntimeMetaInfo* GetRuntimeMetaInfo()
	{
		const auto metaType = entt::resolve<T>();

		if ( !metaType )
		{
			return nullptr;
		}

		return metaType.custom();
	}

	//=================================================================================================
	inline const RuntimeMetaInfo* GetRuntimeMetaInfo( hash_t a_TypeID )
	{
		const auto metaType = entt::resolve( a_TypeID );

		if ( !metaType )
		{
			return nullptr;
		}

		return metaType.custom();
	}

} // namespace Tridium