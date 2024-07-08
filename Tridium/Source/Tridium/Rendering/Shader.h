#pragma once
#include <Tridium/Core/Asset.h>

namespace Tridium {

	using ShaderHandle = AssetHandle;

	enum class ShaderDataType : uint8_t
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	static constexpr uint32_t ShaderDataTypeSize( ShaderDataType type )
	{
		switch ( type )
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:	   return 4 * 3 * 3;
		case ShaderDataType::Mat4:	   return 4 * 4 * 4;
		case ShaderDataType::Int:	   return 4;
		case ShaderDataType::Int2:	   return 4 * 2;
		case ShaderDataType::Int3:	   return 4 * 3;
		case ShaderDataType::Int4:	   return 4 * 4;
		case ShaderDataType::Bool:	   return 1;
		}

		TE_CORE_ASSERT( false, "Unkown ShaderDataType!" );
		return 0;
	}

	class Shader : public Asset<ShaderHandle>
	{
	public:
		static Ref<Shader> Create( const std::string& filePath );
		static Ref<Shader> Create( const std::string& path, const std::string& vertex, const std::string& frag );
		virtual ~Shader() = default;

		virtual bool Recompile() = 0;

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

	class ShaderLibrary : public AssetLibrary<ShaderLibrary, ShaderHandle, Shader>
	{
	public:
		static inline Ref<Shader> GetShader( const ShaderHandle& shaderHandle ) { return Get().GetAsset( shaderHandle ); }
		static inline bool GetShaderHandle( const std::string& path, ShaderHandle& outShaderHandle ) { return Get().GetHandle( path, outShaderHandle ); }
		static inline ShaderHandle GetShaderHandle( const std::string& path ) { return Get().GetHandle( path ); }
		static inline bool HasShaderHandle( const std::string& path ) { return Get().HasHandle( path ); }
		static inline bool AddShader( const std::string& path, const Ref<Shader>& shader ) { return Get().AddAsset( path, shader ); }
		static inline bool RemoveShader( const ShaderHandle& shaderHandle ) { return Get().RemoveAsset( shaderHandle ); }

		static inline ShaderHandle GetDefaultShader() { return Get().m_Default; }
		static inline ShaderHandle GetSpriteShader() { return Get().m_Sprite; }

		static void RecompileAll();

		virtual void Init() override;

	private:
		ShaderHandle m_Default;
		ShaderHandle m_Sprite;

	};

}


