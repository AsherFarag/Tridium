#pragma once
#include <Tridium/Rendering/Shader.h>

#include <glad/glad.h>

namespace Tridium {

    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader( const std::string& vertex, const std::string& frag );
        virtual ~OpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual bool SetInt( const char* name, const int val ) override;
        virtual bool SetInt2( const char* name, const iVector2& val ) override;
        virtual bool SetInt3( const char* name, const iVector3& val ) override;
        virtual bool SetInt4( const char* name, const iVector4& val ) override;
        virtual bool SetFloat( const char* name, const float val ) override;
        virtual bool SetFloat2( const char* name, const Vector2& val ) override;
        virtual bool SetFloat3( const char* name, const Vector3& val ) override;
        virtual bool SetFloat4( const char* name, const Vector4& val ) override;
        virtual bool SetMatrix2( const char* name, const Matrix2& val ) override;
        virtual bool SetMatrix3( const char* name, const Matrix3& val ) override;
        virtual bool SetMatrix4( const char* name, const Matrix4& val ) override;
        virtual bool SetInt( const char* name, const uint32_t count, const int* val ) override;
        virtual bool SetInt2( const char* name, const uint32_t count, const iVector2* val ) override;
        virtual bool SetInt3( const char* name, const uint32_t count, const iVector3* val ) override;
        virtual bool SetInt4( const char* name, const uint32_t count, const iVector4* val ) override;
        virtual bool SetFloat( const char* name, const uint32_t count, const float* val ) override;
        virtual bool SetFloat2( const char* name, const uint32_t count, const Vector2* val ) override;
        virtual bool SetFloat3( const char* name, const uint32_t count, const Vector3* val ) override;
        virtual bool SetFloat4( const char* name, const uint32_t count, const Vector4* val ) override;
        virtual bool SetMatrix2( const char* name, const uint32_t count, const Matrix2* val ) override;
        virtual bool SetMatrix3( const char* name, const uint32_t count, const Matrix3* val ) override;
        virtual bool SetMatrix4( const char* name, const uint32_t count, const Matrix4* val ) override;

    private:
        uint32_t m_RendererID;
    };

}


