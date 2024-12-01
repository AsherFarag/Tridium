#pragma once
#include <entt.hpp>
#include <Tridium/Core/Types.h>
#include <Tridium/IO/TextSerializer.h>

namespace Tridium {

	class Component;
	class Scene;

	namespace Refl {

		using MetaIDType = entt::id_type;
		using MetaAny = entt::meta_any;
		using MetaHandle = entt::meta_handle;
		using MetaData = entt::meta_data;
		using MetaProp = entt::meta_prop;
		using MetaFunc = entt::meta_func;
		using TypeInfo = entt::type_info;
		template<typename _Type, typename _Iterator>
		using MetaRange = entt::meta_range<_Type, _Iterator>;

		namespace Internal {

			using namespace entt::literals;

			// Serialize function signature typedef.
			typedef void ( *SerializeFunc )( IO::Archive& a_Archive, const MetaAny& a_Data );
			typedef void ( *DeserializeFunc )( const YAML::Node& a_Node, MetaAny& a_Data );

			// ID for the serialize function.
			constexpr MetaIDType YAMLDeserializeFuncID = "YAMLDeserializeFuncID"_hs;
			constexpr MetaIDType YAMLSerializeFuncID = "YAMLSerializeFuncID"_hs;
			constexpr MetaIDType CleanClassNamePropID = "CleanClassNamePropID"_hs;

			constexpr MetaIDType IsComponentPropID = "IsComponent"_hs;
			constexpr MetaIDType AddToGameObjectPropID = "AddToGameObject"_hs;
			constexpr MetaIDType RemoveFromGameObjectPropID = "RemoveFromGameObject"_hs;
			typedef Component* ( *AddToGameObjectFunc )( Scene& a_Scene, GameObjectID a_GameObject );
			typedef void ( *RemoveFromGameObjectFunc )( Scene& a_Scene, GameObjectID a_GameObject );
		}

		class MetaType final : private entt::meta_type
		{
		public:
			using SizeType = entt::meta_type::size_type;

			const char* GetCleanTypeName() const
			{
				MetaProp p = prop( Internal::CleanClassNamePropID );
				return p ? p.value().cast<const char*>() : "";
			}

			void Serialize( IO::Archive& a_Archive, const MetaAny& a_Data ) const
			{
				GetSerializeFunc()( a_Archive, a_Data );
			}

			void Deserialize( const YAML::Node& a_Node, MetaAny& a_Data ) const
			{
				GetDeserializeFunc()( a_Node, a_Data );
			}

			bool TrySerialize( IO::Archive& a_Archive, const MetaAny& a_Data ) const
			{
				if ( Internal::SerializeFunc func = TryGetSerializeFunc() )
				{
					func( a_Archive, a_Data );
					return true;
				}

				return false;
			}

			bool TryDeserialize( const YAML::Node& a_Node, MetaAny& a_Data ) const
			{
				if ( Internal::DeserializeFunc func = TryGetDeserializeFunc() )
				{
					func( a_Node, a_Data );
					return true;
				}

				return false;
			}

			bool IsComponent() const
			{
				MetaProp isComponent = prop( Internal::IsComponentPropID );
				return (bool)isComponent;
			}

			Component* AddToGameObject( Scene& a_Scene, GameObjectID a_GameObject ) const
			{
				return GetAddToGameObjectFunc()( a_Scene, a_GameObject );
			}

			void RemoveFromGameObject( Scene& a_Scene, GameObjectID a_GameObject ) const
			{
				GetRemoveFromGameObjectFunc()( a_Scene, a_GameObject );
			}

			Component* TryAddToGameObject( Scene& a_Scene, GameObjectID a_GameObject ) const
			{
				if ( Internal::AddToGameObjectFunc func = TryGetAddToGameObjectFunc() )
				{
					return func( a_Scene, a_GameObject );
				}

				return nullptr;
			}

			bool TryRemoveFromGameObject( Scene& a_Scene, GameObjectID a_GameObject ) const
			{
				if ( Internal::RemoveFromGameObjectFunc func = TryGetRemoveFromGameObjectFunc() )
				{
					func( a_Scene, a_GameObject );
					return true;
				}

				return false;
			}

			[[nodiscard]] MetaIDType ID() const { return id(); }
			[[nodiscard]] const TypeInfo& Info() const { return info(); }
			[[nodiscard]] bool IsArithmetic() const { return is_arithmetic(); }
			[[nodiscard]] bool IsIntegral() const { return is_integral(); }
			[[nodiscard]] bool IsSigned() const { return is_signed(); }
			[[nodiscard]] bool IsArray() const { return is_array(); }
			[[nodiscard]] bool IsEnum() const { return is_enum(); }
			[[nodiscard]] bool IsClass() const { return is_class(); }
			[[nodiscard]] bool IsPointer() const { return is_pointer(); }
			[[nodiscard]] MetaType RemovePointer() const { return remove_pointer(); }
			[[nodiscard]] bool IsPointerLike() const { return is_pointer_like(); }
			[[nodiscard]] bool IsSequenceContainer() const { return is_sequence_container(); }
			[[nodiscard]] bool IsAssociativeContainer() const { return is_associative_container(); }
			[[nodiscard]] bool IsTemplateSpecialization() const { return is_template_specialization(); }
			[[nodiscard]] size_t TemplateArity() const { return template_arity(); }
			[[nodiscard]] MetaType TemplateType() const { return template_type(); }
			[[nodiscard]] MetaType TemplateArgument( size_t a_Index ) const { return template_arg( a_Index ); }
			[[nodiscard]] bool CanCast( const MetaType& a_To ) const { return can_cast( a_To ); }
			[[nodiscard]] bool CanConvert( const MetaType& a_To ) const { return can_convert( a_To ); }

			[[nodiscard]] MetaData Data( MetaIDType a_ID ) const { return data( a_ID ); }
			[[nodiscard]] MetaProp Prop( MetaIDType a_ID ) const { return prop( a_ID ); }
			[[nodiscard]] MetaFunc Func( MetaIDType a_ID ) const { return func( a_ID ); }
			[[nodiscard]] auto Data() const { return data(); }
			[[nodiscard]] auto Prop() const { return prop(); }
			[[nodiscard]] auto Func() const { return func(); }
			[[nodiscard]] auto Base() const { return base(); }

			[[nodiscard]] MetaAny FromVoid( void* a_Element ) const { return from_void( a_Element ); }
			[[nodiscard]] MetaAny FromVoid( const void* a_Element ) const { return from_void( a_Element ); }
			[[nodiscard]] MetaAny Construct( MetaAny* const a_Args, SizeType a_Size ) const { return construct( a_Args, a_Size ); }
			template<typename... _Args>
			[[nodiscard]] MetaAny Construct( _Args&&... a_Args ) const { return construct( std::forward<_Args>( a_Args )... ); }


			//////////////////////////////////////////////////////////////////////////

			// To entt
			operator entt::meta_type() const { return *this; }
			operator const entt::meta_type&() const { return *this; }
			// From entt
			MetaType( const entt::meta_type& a_Type ) : entt::meta_type( a_Type ) {}
			MetaType& operator=( const entt::meta_type& a_Type ) { static_cast<entt::meta_type&>( *this ) = a_Type; return *this; }

		private:
			Internal::SerializeFunc GetSerializeFunc() const
			{
				return prop( Internal::YAMLSerializeFuncID ).value().cast<Internal::SerializeFunc>();
			}

			Internal::DeserializeFunc GetDeserializeFunc() const
			{
				return prop( Internal::YAMLDeserializeFuncID ).value().cast<Internal::DeserializeFunc>();
			}

			Internal::SerializeFunc TryGetSerializeFunc() const
			{
				if ( auto p = prop( Internal::YAMLSerializeFuncID ) )
				{
					return p.value().try_cast<Internal::SerializeFunc>( ) ? GetSerializeFunc() : nullptr;
				}
				return nullptr;
			}

			Internal::DeserializeFunc TryGetDeserializeFunc() const
			{
				if ( auto p = prop( Internal::YAMLDeserializeFuncID ) )
				{
					return p.value().try_cast<Internal::DeserializeFunc>( ) ? GetDeserializeFunc() : nullptr;
				}
				return nullptr;
			}

			Internal::AddToGameObjectFunc GetAddToGameObjectFunc() const
			{
				return IsComponent() ? prop( Internal::AddToGameObjectPropID ).value().cast<Internal::AddToGameObjectFunc>() : nullptr;
			}

			Internal::RemoveFromGameObjectFunc GetRemoveFromGameObjectFunc() const
			{
				return IsComponent() ? prop( Internal::RemoveFromGameObjectPropID ).value().cast<Internal::RemoveFromGameObjectFunc>() : nullptr;
			}

			Internal::AddToGameObjectFunc TryGetAddToGameObjectFunc() const
			{
				if ( auto p = prop( Internal::AddToGameObjectPropID ) )
				{
					Internal::AddToGameObjectFunc* func = p.value().try_cast<Internal::AddToGameObjectFunc>();
					return func ? *func : nullptr;
				}
				return nullptr;
			}

			Internal::RemoveFromGameObjectFunc TryGetRemoveFromGameObjectFunc() const
			{
				if ( auto p = prop( Internal::RemoveFromGameObjectPropID ) )
				{
					Internal::RemoveFromGameObjectFunc* func = p.value().try_cast<Internal::RemoveFromGameObjectFunc>();
					return func ? *func : nullptr;
				}
				return nullptr;
			}
		};

	} // namespace Refl

} // namespace Tridium