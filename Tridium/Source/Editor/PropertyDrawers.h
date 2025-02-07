#pragma once
#if IS_EDITOR
#include <Tridium/Core/Core.h>
#include <Tridium/ImGui/ImGui.h>
#include <Editor/EditorStyle.h>

// Temp?
#include <Tridium/Graphics/Rendering/FrameBuffer.h>

namespace Tridium {
	// Forward declarations
	class GameObject;
	// -------------------
}

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
	bool DrawProperty( const char* a_Name, int8_t& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, int16_t& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, int32_t& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, int64_t& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, uint8_t& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, uint16_t& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, uint32_t& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, uint64_t& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, float& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, double& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, std::string& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, Vector2& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, Vector3& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, Vector4& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, iVector2& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, iVector3& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, iVector4& a_Value, EDrawPropertyFlags a_Flags );

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
	bool DrawProperty( const char* a_Name, LuaScriptHandle& a_Value, EDrawPropertyFlags a_Flags );

	template<>
	bool DrawProperty( const char* a_Name, GameObject& a_Value, EDrawPropertyFlags a_Flags );

	// Temp ?
	template<>
	bool DrawProperty( const char* a_Name, SharedPtr<Framebuffer>& a_Value, EDrawPropertyFlags a_Flags );
}

#endif // IS_EDITOR