#pragma once
#ifdef IS_EDITOR
#include <Tridium/Core/Core.h>
#include <Tridium/ImGui/ImGui.h>
#include <Editor/EditorStyle.h>
#include <Tridium/Utils/Reflection/Meta.h>

// Temp?
#include <Tridium/Rendering/FrameBuffer.h>

namespace Tridium::Editor {

	enum class EDrawPropertyFlags : Byte
	{
		ReadOnly,
		Editable,
	};

	namespace Internal {
		struct ScopedDisable
		{
			ScopedDisable( bool a_Enabled )
				: Enabled( a_Enabled )
			{
				if ( Enabled )
					ImGui::BeginDisabled( true );
			}

			~ScopedDisable()
			{
				if ( Enabled )
					ImGui::EndDisabled();
			}

			bool Enabled;
		};

		// Draw Property function signature typedef.
		typedef bool ( *DrawPropFunc )( const char* a_Name, ::Tridium::Refl::MetaAny& a_Handle, ::Tridium::Refl::PropertyFlags a_Flags );

		// ID for the  Draw Property function.
		constexpr ::Tridium::Refl::MetaIDType DrawPropFuncID = entt::hashed_string( "DrawPropFuncID" ).value();

	}

	struct MinMax_PropertyDrawModifier
	{
		float Min = 0.0f;
		float Max = 1.0f;
	};

	template<typename _PropertyType>
	bool DrawProperty( const char* a_Name, _PropertyType& a_Value, EDrawPropertyFlags a_Flags )
	{
		static_assert( sizeof( _PropertyType ) == 0, "No property drawer found for type." );
		return false;
	}

	// Function declarations
	template<>
	bool DrawProperty( const char* a_Name, bool& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, int& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, float& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, std::string& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, Vector2& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, Vector3& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, Vector4& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, Rotator& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, Color& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, AssetHandle& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, SceneHandle& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, MaterialHandle& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, MeshSourceHandle& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, StaticMeshHandle& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, ShaderHandle& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, TextureHandle& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, CubeMapHandle& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, LuaHandle& a_Value, EDrawPropertyFlags a_Flags );



	// Temp ?
	template<>
	bool DrawProperty( const char* a_Name, SharedPtr<Framebuffer>& a_Value, EDrawPropertyFlags a_Flags );
}

#endif // IS_EDITOR