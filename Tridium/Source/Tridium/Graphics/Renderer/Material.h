#pragma once
#include <Tridium/Containers/Variant.h>

namespace Tridium {

	struct MaterialTextureProperty
	{
		AssetRef<class Texture> Texture;
		Vector2 Offset = Vector2::Zero();
		Vector2 Scale = Vector2::One();
	};

	using MaterialPropertyVariant = Variant<
		int32_t, uint32_t,
		float, double,
		Color4,
		Vector2, Vector3, Vector4,
		Matrix2, Matrix3, Matrix4,
		MaterialTextureProperty
	>;

	enum class EMaterialPropertyType : uint8_t
	{
		Int,
		UInt,
		Float,
		Double,
		Color,
		Vector2,
		Vector3,
		Vector4,
		Matrix2,
		Matrix3,
		Matrix4,
		Texture,
		Unknown = ~0
	};

	struct MaterialProperty
	{
		String Name;
		MaterialPropertyVariant Value;
	};

	//=================================================================================================
	// Material Interface: Master materials and material instances implement this interface.
	//=================================================================================================
	class IMaterialInterface
	{
	public:

		//=============================================================================================
		virtual ~IMaterialInterface() = default;

		//=============================================================================================
		template<typename T>
		void SetProperty( HashedString a_Name, T&& a_Value )
		{
			SetProperty( a_Name, MaterialPropertyVariant( std::forward<T>( a_Value ) ) );
		}

		//=============================================================================================
		void SetProperty( HashedString a_Name, const AssetRef<class Texture>& a_Texture, Vector2 a_Offset = Vector2::Zero(), Vector2 a_Scale = Vector2::One() )
		{
			SetProperty( a_Name, MaterialTextureProperty{ .Texture = a_Texture, .Offset = a_Offset, .Scale = a_Scale } );
		}

		//=============================================================================================
		virtual void SetProperty( HashedString a_Name, MaterialPropertyVariant a_Value ) = 0;

	};

}