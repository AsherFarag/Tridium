#pragma once
#include <entt.hpp>
#include <Tridium/Core/Types.h>
#include <Tridium/IO/TextSerializer.h>
#include "ReflProps.h"

namespace Tridium {

	// Forward Declarations
	class Scene;
	class Component;
	// -------------------

	namespace Refl {

		// A MetaType stores information about a type.
		class MetaType final : private entt::meta_type
		{
		public:
			using SizeType = entt::meta_type::size_type;

			[[nodiscard]] bool IsValid() const { return entt::meta_type::operator bool(); }
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

			[[nodiscard]] MetaProp GetProperty( MetaIDType a_ID ) const { return data( a_ID ); }
			[[nodiscard]] MetaAttribute GetMetaAttribute( MetaIDType a_ID ) const { return prop( a_ID ); }
			[[nodiscard]] MetaFunc GetFunction( MetaIDType a_ID ) const { return func( a_ID ); }
			[[nodiscard]] auto Properties() const { return data(); }
			[[nodiscard]] auto MetaAttributes() const { return prop(); }
			[[nodiscard]] auto Functions() const { return func(); }
			[[nodiscard]] auto Bases() const { return base(); }

			[[nodiscard]] MetaAny FromVoid( void* a_Element ) const { return from_void( a_Element ); }
			[[nodiscard]] MetaAny FromVoid( const void* a_Element ) const { return from_void( a_Element ); }
			[[nodiscard]] MetaAny Construct( MetaAny* const a_Args, SizeType a_Size ) const { return construct( a_Args, a_Size ); }
			template<typename... _Args>
			[[nodiscard]] MetaAny Construct( _Args&&... a_Args ) const { return construct( std::forward<_Args>( a_Args )... ); }


			//////////////////////////////////////////////////////////////////////////
			// Meta Attribute
			//////////////////////////////////////////////////////////////////////////

			EClassFlags GetClassFlags() const
			{
				if ( MetaAttribute p = prop( Props::ClassFlagsProp::ID ) )
					return p.value().cast<EClassFlags>();

				return EClassFlags::ECF_None;
			}

			const char* GetCleanTypeName() const
			{
				if ( MetaAttribute p = GetMetaAttribute( Props::CleanClassNameProp::ID ) )
					return p.value().cast<const char*>();

				return nullptr;
			}

			bool HasMetaAttribute( MetaIDType a_ID ) const
			{
				return static_cast<bool>( GetMetaAttribute( a_ID ) );
			}

			//////////////////////////////////////////////////////////////////////////
			// Serialization
			//////////////////////////////////////////////////////////////////////////

			Props::TextSerializeProp::Type TryGetTextSerializeFunc() const
			{
				if ( MetaAttribute p = GetMetaAttribute( Props::TextSerializeProp::ID ) )
					return p.value().cast<Props::TextSerializeProp::Type>();

				return nullptr;
			}

			Props::TextDeserializeProp::Type TryGetTextDeserializeFunc() const
			{
				if ( MetaAttribute p = GetMetaAttribute( Props::TextDeserializeProp::ID ) )
					return p.value().cast<Props::TextDeserializeProp::Type>();

				return nullptr;
			}

			bool TrySerialize( IO::Archive& a_Archive, const MetaAny& a_Data ) const
			{
				if ( auto func = TryGetTextSerializeFunc() )
				{
					func( a_Archive, a_Data );
					return true;
				}

				return false;
			}

			bool TryDeserialize( const YAML::Node& a_Node, MetaAny& a_Data ) const
			{
				if ( auto func = TryGetTextDeserializeFunc() )
				{
					func( a_Node, a_Data );
					return true;
				}

				return false;
			}

			//////////////////////////////////////////////////////////////////////////
			// GameObject
			//////////////////////////////////////////////////////////////////////////

			bool IsComponent() const
			{
				if ( MetaAttribute p = GetMetaAttribute( Props::IsComponentProp::ID ) )
					return p.value().cast<bool>();

				return false;
			}

			Props::AddToGameObjectProp::Type TryGetAddToGameObjectFunc() const
			{
				if ( MetaAttribute p = GetMetaAttribute( Props::AddToGameObjectProp::ID ) )
					return p.value().cast<Props::AddToGameObjectProp::Type>();

				return nullptr;
			}

			Component* TryAddToGameObject( Scene& a_Scene, GameObjectID a_GameObject ) const
			{
				if ( auto func = TryGetAddToGameObjectFunc() )
					return func( a_Scene, a_GameObject );

				return nullptr;
			}

			Props::RemoveFromGameObjectProp::Type TryGetRemoveFromGameObjectFunc() const
			{
				if ( MetaAttribute p = prop( Props::RemoveFromGameObjectProp::ID ) )
					return p.value().cast<Props::RemoveFromGameObjectProp::Type>();

				return nullptr;
			}

			void TryRemoveFromGameObject( Scene& a_Scene, GameObjectID a_GameObject ) const
			{
				if ( auto func = TryGetRemoveFromGameObjectFunc() )
					func( a_Scene, a_GameObject );
			}

			//////////////////////////////////////////////////////////////////////////

			// To entt
			operator entt::meta_type() const { return *this; }
			operator const entt::meta_type& ( ) const { return *this; }
			// From entt
			MetaType( const entt::meta_type& a_Type ) : entt::meta_type( a_Type ) {}
			MetaType( entt::meta_type&& a_Type ) : entt::meta_type( std::move( a_Type ) ) {}
			MetaType& operator=( const entt::meta_type& a_Type ) { static_cast<entt::meta_type&>( *this ) = a_Type; return *this; }
		};

		inline auto ResolveMetaTypes() { return entt::resolve(); }
		inline MetaType ResolveMetaType( const TypeInfo& a_Info ) { return entt::resolve( a_Info ); }
		inline MetaType ResolveMetaType( MetaIDType a_ID ) { return entt::resolve( a_ID ); }
		inline MetaType ResolveMetaType( const char* a_Name ) { return entt::resolve( entt::hashed_string(a_Name) ); }
		template <typename T>
		inline MetaType ResolveMetaType() { return entt::resolve<T>(); }

	} // namespace Refl

} // namespace Tridium