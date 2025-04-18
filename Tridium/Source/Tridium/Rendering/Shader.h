#pragma once
#include <Tridium/Asset/Asset.h>

namespace Tridium {

	enum class EShaderDataType : uint8_t
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	static constexpr uint32_t ShaderDataTypeSize( EShaderDataType type )
	{
		switch ( type )
		{
		case EShaderDataType::Float:    return 4;
		case EShaderDataType::Float2:   return 4 * 2;
		case EShaderDataType::Float3:   return 4 * 3;
		case EShaderDataType::Float4:   return 4 * 4;
		case EShaderDataType::Mat3:	   return 4 * 3 * 3;
		case EShaderDataType::Mat4:	   return 4 * 4 * 4;
		case EShaderDataType::Int:	   return 4;
		case EShaderDataType::Int2:	   return 4 * 2;
		case EShaderDataType::Int3:	   return 4 * 3;
		case EShaderDataType::Int4:	   return 4 * 4;
		case EShaderDataType::Bool:	   return 1;
		}

		TE_CORE_ASSERT( false, "Unkown ShaderDataType!" );
		return 0;
	}

	class Shader : public Asset
	{
	public:
		ASSET_CLASS_TYPE( Shader )

		static Shader* Create();
		static Shader* Create( const std::string& a_Vertex, const std::string& a_Frag );
		virtual ~Shader() = default;

		virtual bool Compile( const IO::FilePath& a_Path ) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		// - SINGULAR -

		virtual bool SetInt( const char* name, const int val ) = 0;
		virtual bool SetInt2( const char* name, const iVector2& val ) = 0;
		virtual bool SetInt3( const char* name, const iVector3& val ) = 0;
		virtual bool SetInt4( const char* name, const iVector4& val ) = 0;

		virtual bool SetFloat( const char* name, const float val ) = 0;
		virtual bool SetFloat2( const char* name, const Vector2& val ) = 0;
		virtual bool SetFloat3( const char* name, const Vector3& val ) = 0;
		virtual bool SetFloat4( const char* name, const Vector4& val ) = 0;

		virtual bool SetMatrix2( const char* name, const Matrix2& val ) = 0;
		virtual bool SetMatrix3( const char* name, const Matrix3& val ) = 0;
		virtual bool SetMatrix4( const char* name, const Matrix4& val ) = 0;

		// - ARRAYS -

		virtual bool SetInt( const char* name, const uint32_t count, const int* val ) = 0;
		virtual bool SetInt2( const char* name, const uint32_t count, const iVector2* val ) = 0;
		virtual bool SetInt3( const char* name, const uint32_t count, const iVector3* val ) = 0;
		virtual bool SetInt4( const char* name, const uint32_t count, const iVector4* val ) = 0;

		virtual bool SetFloat( const char* name, const uint32_t count, const float* val ) = 0;
		virtual bool SetFloat2( const char* name, const uint32_t count, const Vector2* val ) = 0;
		virtual bool SetFloat3( const char* name, const uint32_t count, const Vector3* val ) = 0;
		virtual bool SetFloat4( const char* name, const uint32_t count, const Vector4* val ) = 0;

		virtual bool SetMatrix2( const char* name, const uint32_t count, const Matrix2* val ) = 0;
		virtual bool SetMatrix3( const char* name, const uint32_t count, const Matrix3* val ) = 0;
		virtual bool SetMatrix4( const char* name, const uint32_t count, const Matrix4* val ) = 0;
	};
}


