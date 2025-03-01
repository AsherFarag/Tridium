#pragma once
#include "OpenGL3.h"

namespace Tridium {

    class OpenGL4 : public OpenGL3
    {
    public:
    #pragma region OpenGL 4.0

		static FORCEINLINE void MinSampleShading( GLfloat a_Value )
		{
			glMinSampleShading( a_Value );
		}

        static FORCEINLINE void BlendEquationi( GLuint a_Buf, GLenum a_Mode )
        {
			glBlendEquationi( a_Buf, a_Mode );
        }

		static FORCEINLINE void BlendEquationSeparatei( GLuint a_Buf, GLenum a_ModeRGB, GLenum a_ModeAlpha )
        {
			glBlendEquationSeparatei( a_Buf, a_ModeRGB, a_ModeAlpha );
        }

        static FORCEINLINE void BlendFunci( GLuint a_Buf, GLenum a_Src, GLenum a_Dst )
        {
			glBlendFunci( a_Buf, a_Src, a_Dst );
        }

		static FORCEINLINE void BlendFuncSeparatei( GLuint a_Buf, GLenum a_SrcRGB, GLenum a_DstRGB, GLenum a_SrcAlpha, GLenum a_DstAlpha )
		{
			glBlendFuncSeparatei( a_Buf, a_SrcRGB, a_DstRGB, a_SrcAlpha, a_DstAlpha );
		}

        static FORCEINLINE void DrawArraysIndirect( GLenum a_Mode, const GLvoid* a_Indirect )
        {
			glDrawArraysIndirect( a_Mode, a_Indirect );
        }

		static FORCEINLINE void DrawElementsIndirect( GLenum a_Mode, GLenum a_Type, const GLvoid* a_Indirect )
		{
			glDrawElementsIndirect( a_Mode, a_Type, a_Indirect );
		}

		static FORCEINLINE void Uniform1d( GLint a_Location, GLdouble a_X )
		{
			glUniform1d( a_Location, a_X );
		}

		static FORCEINLINE void Uniform2d( GLint a_Location, GLdouble a_X, GLdouble a_Y )
		{
			glUniform2d( a_Location, a_X, a_Y );
		}

		static FORCEINLINE void Uniform3d( GLint a_Location, GLdouble a_X, GLdouble a_Y, GLdouble a_Z )
		{
			glUniform3d( a_Location, a_X, a_Y, a_Z );
		}

		static FORCEINLINE void Uniform4d( GLint a_Location, GLdouble a_X, GLdouble a_Y, GLdouble a_Z, GLdouble a_W )
		{
			glUniform4d( a_Location, a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void Uniform1dv( GLint a_Location, GLsizei a_Count, const GLdouble* a_Value )
		{
			glUniform1dv( a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void Uniform2dv( GLint a_Location, GLsizei a_Count, const GLdouble* a_Value )
		{
			glUniform2dv( a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void Uniform3dv( GLint a_Location, GLsizei a_Count, const GLdouble* a_Value )
		{
			glUniform3dv( a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void Uniform4dv( GLint a_Location, GLsizei a_Count, const GLdouble* a_Value )
		{
			glUniform4dv( a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void UniformMatrix2dv( GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLdouble* a_Value )
		{
			glUniformMatrix2dv( a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void UniformMatrix3dv( GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLdouble* a_Value )
		{
			glUniformMatrix3dv( a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void UniformMatrix4dv( GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLdouble* a_Value )
		{
			glUniformMatrix4dv( a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void GetUniformdv( GLuint a_Program, GLint a_Location, GLdouble* a_Params )
		{
			glGetUniformdv( a_Program, a_Location, a_Params );
		}

		static FORCEINLINE GLint GetSubroutineUniformLocation( GLuint a_Shader, GLenum a_ShaderType, const GLchar* a_Name )
		{
			return glGetSubroutineUniformLocation( a_Shader, a_ShaderType, a_Name );
		}

		static FORCEINLINE GLuint GetSubroutineIndex( GLuint a_Shader, GLenum a_ShaderType, const GLchar* a_Name )
		{
			return glGetSubroutineIndex( a_Shader, a_ShaderType, a_Name );
		}

		static FORCEINLINE void GetActiveSubroutineUniformiv( GLuint a_Shader, GLenum a_ShaderType, GLuint a_Index, GLenum a_Pname, GLint* a_Params )
		{
			glGetActiveSubroutineUniformiv( a_Shader, a_ShaderType, a_Index, a_Pname, a_Params );
		}

		static FORCEINLINE void GetActiveSubroutineUniformName( GLuint a_Shader, GLenum a_ShaderType, GLuint a_Index, GLsizei a_BufSize, GLsizei* a_Length, GLchar* a_Name )
		{
			glGetActiveSubroutineUniformName( a_Shader, a_ShaderType, a_Index, a_BufSize, a_Length, a_Name );
		}

		static FORCEINLINE void GetActiveSubroutineName( GLuint a_Shader, GLenum a_ShaderType, GLuint a_Index, GLsizei a_BufSize, GLsizei* a_Length, GLchar* a_Name )
		{
			glGetActiveSubroutineName( a_Shader, a_ShaderType, a_Index, a_BufSize, a_Length, a_Name );
		}

		static FORCEINLINE void UniformSubroutinesuiv( GLenum a_ShaderType, GLsizei a_Count, const GLuint* a_Indices )
		{
			glUniformSubroutinesuiv( a_ShaderType, a_Count, a_Indices );
		}

		static FORCEINLINE void GetUniformSubroutineuiv( GLenum a_ShaderType, GLint a_Location, GLuint* a_Params )
		{
			glGetUniformSubroutineuiv( a_ShaderType, a_Location, a_Params );
		}

		static FORCEINLINE void GetProgramStageiv( GLuint a_Program, GLenum a_ShaderType, GLenum a_Pname, GLint* a_Values )
		{
			glGetProgramStageiv( a_Program, a_ShaderType, a_Pname, a_Values );
		}

		static FORCEINLINE void PatchParameteri( GLenum a_Pname, GLint a_Value )
		{
			glPatchParameteri( a_Pname, a_Value );
		}

		static FORCEINLINE void PatchParameterfv( GLenum a_Pname, const GLfloat* a_Values )
		{
			glPatchParameterfv( a_Pname, a_Values );
		}

		static FORCEINLINE void BindTransformFeedback( GLenum a_Target, GLuint a_Id )
		{
			glBindTransformFeedback( a_Target, a_Id );
		}

		static FORCEINLINE void DeleteTransformFeedbacks( GLsizei a_N, const GLuint* a_Id )
		{
			glDeleteTransformFeedbacks( a_N, a_Id );
		}

		static FORCEINLINE void GenTransformFeedbacks( GLsizei a_N, GLuint* a_Id )
		{
			glGenTransformFeedbacks( a_N, a_Id );
		}

		static FORCEINLINE GLboolean IsTransformFeedback( GLuint a_Id )
		{
			return glIsTransformFeedback( a_Id );
		}

		static FORCEINLINE void PauseTransformFeedback( void )
		{
			glPauseTransformFeedback();
		}

		static FORCEINLINE void ResumeTransformFeedback( void )
		{
			glResumeTransformFeedback();
		}

		static FORCEINLINE void DrawTransformFeedback( GLenum a_Mode, GLuint a_Id )
		{
			glDrawTransformFeedback( a_Mode, a_Id );
		}

		static FORCEINLINE void DrawTransformFeedbackStream( GLenum a_Mode, GLuint a_Id, GLuint a_Stream )
		{
			glDrawTransformFeedbackStream( a_Mode, a_Id, a_Stream );
		}

		static FORCEINLINE void BeginQueryIndexed( GLenum a_Target, GLuint a_Index, GLuint a_Id )
		{
			glBeginQueryIndexed( a_Target, a_Index, a_Id );
		}

		static FORCEINLINE void EndQueryIndexed( GLenum a_Target, GLuint a_Index )
		{
			glEndQueryIndexed( a_Target, a_Index );
		}

		static FORCEINLINE void GetQueryIndexediv( GLenum a_Target, GLuint a_Index, GLenum a_Pname, GLint* a_Params )
		{
			glGetQueryIndexediv( a_Target, a_Index, a_Pname, a_Params );
		}

	#pragma endregion

	#pragma region OpenGL 4.1

		static FORCEINLINE void ReleaseShaderCompiler( void )
		{
			glReleaseShaderCompiler();
		}

		static FORCEINLINE void ShaderBinary( GLsizei a_Count, const GLuint* a_Shaders, GLenum a_BinaryFormat, const GLvoid* a_Binary, GLsizei a_Length )
		{
			glShaderBinary( a_Count, a_Shaders, a_BinaryFormat, a_Binary, a_Length );
		}

		static FORCEINLINE void GetShaderPrecisionFormat( GLenum a_ShaderType, GLenum a_PrecisionType, GLint* a_Range, GLint* a_Precision )
		{
			glGetShaderPrecisionFormat( a_ShaderType, a_PrecisionType, a_Range, a_Precision );
		}

		static FORCEINLINE void DepthRangef( GLfloat a_Near, GLfloat a_Far )
		{
			glDepthRangef( a_Near, a_Far );
		}

		static FORCEINLINE void ClearDepthf( GLfloat a_Depth )
		{
			glClearDepthf( a_Depth );
		}

		static FORCEINLINE void GetProgramBinary( GLuint a_Program, GLsizei a_BufSize, GLsizei* a_Length, GLenum* a_BinaryFormat, GLvoid* a_Binary )
		{
			glGetProgramBinary( a_Program, a_BufSize, a_Length, a_BinaryFormat, a_Binary );
		}

		static FORCEINLINE void ProgramBinary( GLuint a_Program, GLenum a_BinaryFormat, const GLvoid* a_Binary, GLsizei a_Length )
		{
			glProgramBinary( a_Program, a_BinaryFormat, a_Binary, a_Length );
		}

		static FORCEINLINE void ProgramParameteri( GLuint a_Program, GLenum a_Pname, GLint a_Value )
		{
			glProgramParameteri( a_Program, a_Pname, a_Value );
		}

		static FORCEINLINE void UseProgramStages( GLuint a_Pipeline, GLbitfield a_Stages, GLuint a_Program )
		{
			glUseProgramStages( a_Pipeline, a_Stages, a_Program );
		}

		static FORCEINLINE void ActiveShaderProgram( GLuint a_Pipeline, GLuint a_Program )
		{
			glActiveShaderProgram( a_Pipeline, a_Program );
		}

		static FORCEINLINE GLuint CreateShaderProgramv( GLenum a_Type, GLsizei a_Count, const GLchar** a_Strings )
		{
			return glCreateShaderProgramv( a_Type, a_Count, a_Strings );
		}

		static FORCEINLINE void BindProgramPipeline( GLuint a_Pipeline )
		{
			glBindProgramPipeline( a_Pipeline );
		}

		static FORCEINLINE void DeleteProgramPipelines( GLsizei a_N, const GLuint* a_Pipelines )
		{
			glDeleteProgramPipelines( a_N, a_Pipelines );
		}

		static FORCEINLINE void GenProgramPipelines( GLsizei a_N, GLuint* a_Pipelines )
		{
			glGenProgramPipelines( a_N, a_Pipelines );
		}

		static FORCEINLINE GLboolean IsProgramPipeline( GLuint a_Pipeline )
		{
			return glIsProgramPipeline( a_Pipeline );
		}

		static FORCEINLINE void GetProgramPipelineiv( GLuint a_Pipeline, GLenum a_Pname, GLint* a_Params )
		{
			glGetProgramPipelineiv( a_Pipeline, a_Pname, a_Params );
		}

		static FORCEINLINE void ProgramUniform1i( GLuint a_Program, GLint a_Location, GLint a_V0 )
		{
			glProgramUniform1i( a_Program, a_Location, a_V0 );
		}

		static FORCEINLINE void ProgramUniform2i( GLuint a_Program, GLint a_Location, GLint a_V0, GLint a_V1 )
		{
			glProgramUniform2i( a_Program, a_Location, a_V0, a_V1 );
		}

		static FORCEINLINE void ProgramUniform3i( GLuint a_Program, GLint a_Location, GLint a_V0, GLint a_V1, GLint a_V2 )
		{
			glProgramUniform3i( a_Program, a_Location, a_V0, a_V1, a_V2 );
		}

		static FORCEINLINE void ProgramUniform4i( GLuint a_Program, GLint a_Location, GLint a_V0, GLint a_V1, GLint a_V2, GLint a_V3 )
		{
			glProgramUniform4i( a_Program, a_Location, a_V0, a_V1, a_V2, a_V3 );
		}

		static FORCEINLINE void ProgramUniform1ui( GLuint a_Program, GLint a_Location, GLuint a_V0 )
		{
			glProgramUniform1ui( a_Program, a_Location, a_V0 );
		}

		static FORCEINLINE void ProgramUniform2ui( GLuint a_Program, GLint a_Location, GLuint a_V0, GLuint a_V1 )
		{
			glProgramUniform2ui( a_Program, a_Location, a_V0, a_V1 );
		}

		static FORCEINLINE void ProgramUniform3ui( GLuint a_Program, GLint a_Location, GLuint a_V0, GLuint a_V1, GLuint a_V2 )
		{
			glProgramUniform3ui( a_Program, a_Location, a_V0, a_V1, a_V2 );
		}

		static FORCEINLINE void ProgramUniform4ui( GLuint a_Program, GLint a_Location, GLuint a_V0, GLuint a_V1, GLuint a_V2, GLuint a_V3 )
		{
			glProgramUniform4ui( a_Program, a_Location, a_V0, a_V1, a_V2, a_V3 );
		}

		static FORCEINLINE void ProgramUniform1f( GLuint a_Program, GLint a_Location, GLfloat a_V0 )
		{
			glProgramUniform1f( a_Program, a_Location, a_V0 );
		}

		static FORCEINLINE void ProgramUniform2f( GLuint a_Program, GLint a_Location, GLfloat a_V0, GLfloat a_V1 )
		{
			glProgramUniform2f( a_Program, a_Location, a_V0, a_V1 );
		}

		static FORCEINLINE void ProgramUniform3f( GLuint a_Program, GLint a_Location, GLfloat a_V0, GLfloat a_V1, GLfloat a_V2 )
		{
			glProgramUniform3f( a_Program, a_Location, a_V0, a_V1, a_V2 );
		}

		static FORCEINLINE void ProgramUniform4f( GLuint a_Program, GLint a_Location, GLfloat a_V0, GLfloat a_V1, GLfloat a_V2, GLfloat a_V3 )
		{
			glProgramUniform4f( a_Program, a_Location, a_V0, a_V1, a_V2, a_V3 );
		}

		static FORCEINLINE void ProgramUniform1iv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLint* a_Value )
		{
			glProgramUniform1iv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniform2iv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLint* a_Value )
		{
			glProgramUniform2iv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniform3iv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLint* a_Value )
		{
			glProgramUniform3iv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniform4iv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLint* a_Value )
		{
			glProgramUniform4iv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniform1uiv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLuint* a_Value )
		{
			glProgramUniform1uiv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniform2uiv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLuint* a_Value )
		{
			glProgramUniform2uiv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniform3uiv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLuint* a_Value )
		{
			glProgramUniform3uiv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniform4uiv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLuint* a_Value )
		{
			glProgramUniform4uiv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniform1fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLfloat* a_Value )
		{
			glProgramUniform1fv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniform2fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLfloat* a_Value )
		{
			glProgramUniform2fv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniform3fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLfloat* a_Value )
		{
			glProgramUniform3fv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniform4fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, const GLfloat* a_Value )
		{
			glProgramUniform4fv( a_Program, a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void ProgramUniformMatrix2fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLfloat* a_Value )
		{
			glProgramUniformMatrix2fv( a_Program, a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void ProgramUniformMatrix3fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLfloat* a_Value )
		{
			glProgramUniformMatrix3fv( a_Program, a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void ProgramUniformMatrix4fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLfloat* a_Value )
		{
			glProgramUniformMatrix4fv( a_Program, a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void ProgramUniformMatrix2x3fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLfloat* a_Value )
		{
			glProgramUniformMatrix2x3fv( a_Program, a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void ProgramUniformMatrix3x2fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLfloat* a_Value )
		{
			glProgramUniformMatrix3x2fv( a_Program, a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void ProgramUniformMatrix2x4fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLfloat* a_Value )
		{
			glProgramUniformMatrix2x4fv( a_Program, a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void ProgramUniformMatrix4x2fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLfloat* a_Value )
		{
			glProgramUniformMatrix4x2fv( a_Program, a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void ProgramUniformMatrix3x4fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLfloat* a_Value )
		{
			glProgramUniformMatrix3x4fv( a_Program, a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void ProgramUniformMatrix4x3fv( GLuint a_Program, GLint a_Location, GLsizei a_Count, GLboolean a_Transpose, const GLfloat* a_Value )
		{
			glProgramUniformMatrix4x3fv( a_Program, a_Location, a_Count, a_Transpose, a_Value );
		}

		static FORCEINLINE void ValidateProgramPipeline( GLuint a_Pipeline )
		{
			glValidateProgramPipeline( a_Pipeline );
		}

		static FORCEINLINE void GetProgramPipelineInfoLog( GLuint a_Pipeline, GLsizei a_BufSize, GLsizei* a_Length, GLchar* a_InfoLog )
		{
			glGetProgramPipelineInfoLog( a_Pipeline, a_BufSize, a_Length, a_InfoLog );
		}

		static FORCEINLINE void VertexAttribL1d( GLuint a_Index, GLdouble a_X )
		{
			glVertexAttribL1d( a_Index, a_X );
		}

		static FORCEINLINE void VertexAttribL2d( GLuint a_Index, GLdouble a_X, GLdouble a_Y )
		{
			glVertexAttribL2d( a_Index, a_X, a_Y );
		}

		static FORCEINLINE void VertexAttribL3d( GLuint a_Index, GLdouble a_X, GLdouble a_Y, GLdouble a_Z )
		{
			glVertexAttribL3d( a_Index, a_X, a_Y, a_Z );
		}

		static FORCEINLINE void VertexAttribL4d( GLuint a_Index, GLdouble a_X, GLdouble a_Y, GLdouble a_Z, GLdouble a_W )
		{
			glVertexAttribL4d( a_Index, a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void VertexAttribL1dv( GLuint a_Index, const GLdouble* a_V )
		{
			glVertexAttribL1dv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribL2dv( GLuint a_Index, const GLdouble* a_V )
		{
			glVertexAttribL2dv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribL3dv( GLuint a_Index, const GLdouble* a_V )
		{
			glVertexAttribL3dv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribL4dv( GLuint a_Index, const GLdouble* a_V )
		{
			glVertexAttribL4dv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribLPointer( GLuint a_Index, GLint a_Size, GLenum a_Type, GLsizei a_Stride, const GLvoid* a_Pointer )
		{
			glVertexAttribLPointer( a_Index, a_Size, a_Type, a_Stride, a_Pointer );
		}

		static FORCEINLINE void GetVertexAttribLdv( GLuint a_Index, GLenum a_PName, GLdouble* a_Params )
		{
			glGetVertexAttribLdv( a_Index, a_PName, a_Params );
		}

		static FORCEINLINE void ViewportArrayv( GLuint a_First, GLsizei a_Count, const GLfloat* a_Values )
		{
			glViewportArrayv( a_First, a_Count, a_Values );
		}

		static FORCEINLINE void ViewportIndexedf( GLuint a_Index, GLfloat a_X, GLfloat a_Y, GLfloat a_Width, GLfloat a_Height )
		{
			glViewportIndexedf( a_Index, a_X, a_Y, a_Width, a_Height );
		}

		static FORCEINLINE void ViewportIndexedfv( GLuint a_Index, const GLfloat* a_Values )
		{
			glViewportIndexedfv( a_Index, a_Values );
		}

		static FORCEINLINE void ScissorArrayv( GLuint a_First, GLsizei a_Count, const GLint* a_Values )
		{
			glScissorArrayv( a_First, a_Count, a_Values );
		}

		static FORCEINLINE void ScissorIndexed( GLuint a_Index, GLint a_Left, GLint a_Bottom, GLsizei a_Width, GLsizei a_Height )
		{
			glScissorIndexed( a_Index, a_Left, a_Bottom, a_Width, a_Height );
		}

		static FORCEINLINE void ScissorIndexedv( GLuint a_Index, const GLint* a_Values )
		{
			glScissorIndexedv( a_Index, a_Values );
		}

		static FORCEINLINE void DepthRangeArrayv( GLuint a_First, GLsizei a_Count, const GLdouble* a_Values )
		{
			glDepthRangeArrayv( a_First, a_Count, a_Values );
		}

		static FORCEINLINE void DepthRangeIndexed( GLuint a_Index, GLdouble a_Near, GLdouble a_Far )
		{
			glDepthRangeIndexed( a_Index, a_Near, a_Far );
		}

		static FORCEINLINE void GetFloati_v( GLenum a_Target, GLuint a_Index, GLfloat* a_Data )
		{
			glGetFloati_v( a_Target, a_Index, a_Data );
		}

		static FORCEINLINE void GetDoublei_v( GLenum a_Target, GLuint a_Index, GLdouble* a_Data )
		{
			glGetDoublei_v( a_Target, a_Index, a_Data );
		}

	#pragma endregion

	#pragma region OpenGL 4.2

		static FORCEINLINE void DrawArraysInstancedBaseInstance( GLenum a_Mode, GLint a_First, GLsizei a_Count, GLsizei a_InstanceCount, GLuint a_BaseInstance )
		{
			glDrawArraysInstancedBaseInstance( a_Mode, a_First, a_Count, a_InstanceCount, a_BaseInstance );
		}

		static FORCEINLINE void DrawElementsInstancedBaseInstance( GLenum a_Mode, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices, GLsizei a_InstanceCount, GLuint a_BaseInstance )
		{
			glDrawElementsInstancedBaseInstance( a_Mode, a_Count, a_Type, a_Indices, a_InstanceCount, a_BaseInstance );
		}

		static FORCEINLINE void DrawElementsInstancedBaseVertexBaseInstance( GLenum a_Mode, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices, GLsizei a_InstanceCount, GLint a_BaseVertex, GLuint a_BaseInstance )
		{
			glDrawElementsInstancedBaseVertexBaseInstance( a_Mode, a_Count, a_Type, a_Indices, a_InstanceCount, a_BaseVertex, a_BaseInstance );
		}

		static FORCEINLINE void GetInternalformativ( GLenum a_Target, GLenum a_Internalformat, GLenum a_Pname, GLsizei a_BufSize, GLint* a_Params )
		{
			glGetInternalformativ( a_Target, a_Internalformat, a_Pname, a_BufSize, a_Params );
		}

		static FORCEINLINE void MemoryBarrier( GLbitfield a_Barriers )
		{
			glMemoryBarrier( a_Barriers );
		}

		static FORCEINLINE void TexStorage2D( GLenum a_Target, GLsizei a_Levels, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height )
		{
			glTexStorage2D( a_Target, a_Levels, a_Internalformat, a_Width, a_Height );
		}

		static FORCEINLINE void TexStorage3D( GLenum a_Target, GLsizei a_Levels, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth )
		{
			glTexStorage3D( a_Target, a_Levels, a_Internalformat, a_Width, a_Height, a_Depth );
		}

		static FORCEINLINE void TextureStorage2D( GLuint a_Texture, GLsizei a_Levels, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height )
		{
			glTextureStorage2D( a_Texture, a_Levels, a_Internalformat, a_Width, a_Height );
		}

		static FORCEINLINE void TextureStorage3D( GLuint a_Texture, GLsizei a_Levels, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth )
		{
			glTextureStorage3D( a_Texture, a_Levels, a_Internalformat, a_Width, a_Height, a_Depth );
		}

		static FORCEINLINE void DrawTransformFeedbackInstanced( GLenum a_Mode, GLuint a_Id, GLsizei a_InstanceCount )
		{
			glDrawTransformFeedbackInstanced( a_Mode, a_Id, a_InstanceCount );
		}

		static FORCEINLINE void DrawTransformFeedbackStreamInstanced( GLenum a_Mode, GLuint a_Id, GLuint a_Stream, GLsizei a_InstanceCount )
		{
			glDrawTransformFeedbackStreamInstanced( a_Mode, a_Id, a_Stream, a_InstanceCount );
		}

	#pragma endregion

	#pragma region OpenGL 4.3

		static FORCEINLINE void ClearBufferData( GLenum a_Target, GLenum a_Internalformat, GLenum a_Format, GLenum a_Type, const GLvoid* a_Data )
		{
			glClearBufferData( a_Target, a_Internalformat, a_Format, a_Type, a_Data );
		}

		static FORCEINLINE void ClearBufferSubData( GLenum a_Target, GLenum a_Internalformat, GLintptr a_Offset, GLsizeiptr a_Size, GLenum a_Format, GLenum a_Type, const GLvoid* a_Data )
		{
			glClearBufferSubData( a_Target, a_Internalformat, a_Offset, a_Size, a_Format, a_Type, a_Data );
		}

		static FORCEINLINE void DispatchCompute( GLuint a_NumGroupsX, GLuint a_NumGroupsY, GLuint a_NumGroupsZ )
		{
			glDispatchCompute( a_NumGroupsX, a_NumGroupsY, a_NumGroupsZ );
		}

		static FORCEINLINE void DispatchComputeIndirect( GLintptr a_Indirect )
		{
			glDispatchComputeIndirect( a_Indirect );
		}

		static FORCEINLINE void CopyImageSubData( GLuint a_SrcName, GLenum a_SrcTarget, GLint a_SrcLevel, GLint a_SrcX, GLint a_SrcY, GLint a_SrcZ, GLuint a_DstName, GLenum a_DstTarget, GLint a_DstLevel, GLint a_DstX, GLint a_DstY, GLint a_DstZ, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth )
		{
			glCopyImageSubData( a_SrcName, a_SrcTarget, a_SrcLevel, a_SrcX, a_SrcY, a_SrcZ, a_DstName, a_DstTarget, a_DstLevel, a_DstX, a_DstY, a_DstZ, a_Width, a_Height, a_Depth );
		}

		static FORCEINLINE void FramebufferParameteri( GLenum a_Target, GLenum a_Pname, GLint a_Param )
		{
			glFramebufferParameteri( a_Target, a_Pname, a_Param );
		}

		static FORCEINLINE void GetFramebufferParameteriv( GLenum a_Target, GLenum a_Pname, GLint* a_Params )
		{
			glGetFramebufferParameteriv( a_Target, a_Pname, a_Params );
		}

		static FORCEINLINE void GetInternalformati64v( GLenum a_Target, GLenum a_Internalformat, GLenum a_Pname, GLsizei a_BufSize, GLint64* a_Params )
		{
			glGetInternalformati64v( a_Target, a_Internalformat, a_Pname, a_BufSize, a_Params );
		}

		static FORCEINLINE void InvalidateBufferData( GLuint a_Buffer )
		{
			glInvalidateBufferData( a_Buffer );
		}

		static FORCEINLINE void InvalidateBufferSubData( GLuint a_Buffer, GLintptr a_Offset, GLsizeiptr a_Length )
		{
			glInvalidateBufferSubData( a_Buffer, a_Offset, a_Length );
		}

		static FORCEINLINE void InvalidateFramebuffer( GLenum a_Target, GLsizei a_NumAttachments, const GLenum* a_Attachments )
		{
			glInvalidateFramebuffer( a_Target, a_NumAttachments, a_Attachments );
		}

		static FORCEINLINE void InvalidateSubFramebuffer( GLenum a_Target, GLsizei a_NumAttachments, const GLenum* a_Attachments, GLint a_X, GLint a_Y, GLsizei a_Width, GLsizei a_Height )
		{
			glInvalidateSubFramebuffer( a_Target, a_NumAttachments, a_Attachments, a_X, a_Y, a_Width, a_Height );
		}

		static FORCEINLINE void InvalidateTexImage( GLuint a_Texture, GLint a_Level )
		{
			glInvalidateTexImage( a_Texture, a_Level );
		}

		static FORCEINLINE void InvalidateTexSubImage( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_Zoffset, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth )
		{
			glInvalidateTexSubImage( a_Texture, a_Level, a_Xoffset, a_Yoffset, a_Zoffset, a_Width, a_Height, a_Depth );
		}

		static FORCEINLINE void MultiDrawArraysIndirect( GLenum a_Mode, const GLvoid* a_Indirect, GLsizei a_DrawCount, GLsizei a_Stride )
		{
			glMultiDrawArraysIndirect( a_Mode, a_Indirect, a_DrawCount, a_Stride );
		}

		static FORCEINLINE void MultiDrawElementsIndirect( GLenum a_Mode, GLenum a_Type, const GLvoid* a_Indirect, GLsizei a_DrawCount, GLsizei a_Stride )
		{
			glMultiDrawElementsIndirect( a_Mode, a_Type, a_Indirect, a_DrawCount, a_Stride );
		}

		static FORCEINLINE void GetProgramInterfaceiv( GLuint a_Program, GLenum a_ProgramInterface, GLenum a_Pname, GLint* a_Params )
		{
			glGetProgramInterfaceiv( a_Program, a_ProgramInterface, a_Pname, a_Params );
		}

		static FORCEINLINE GLuint GetProgramResourceIndex( GLuint a_Program, GLenum a_ProgramInterface, const GLchar* a_Name )
		{
			return glGetProgramResourceIndex( a_Program, a_ProgramInterface, a_Name );
		}

		static FORCEINLINE void GetProgramResourceName( GLuint a_Program, GLenum a_ProgramInterface, GLuint a_Index, GLsizei a_BufSize, GLsizei* a_Length, GLchar* a_Name )
		{
			glGetProgramResourceName( a_Program, a_ProgramInterface, a_Index, a_BufSize, a_Length, a_Name );
		}

		static FORCEINLINE void GetProgramResourceiv( GLuint a_Program, GLenum a_ProgramInterface, GLuint a_Index, GLsizei a_PropCount, const GLenum* a_Props, GLsizei a_BufSize, GLsizei* a_Length, GLint* a_PropsData )
		{
			glGetProgramResourceiv( a_Program, a_ProgramInterface, a_Index, a_PropCount, a_Props, a_BufSize, a_Length, a_PropsData );
		}

		static FORCEINLINE GLint GetProgramResourceLocation( GLuint a_Program, GLenum a_ProgramInterface, const GLchar* a_Name )
		{
			return glGetProgramResourceLocation( a_Program, a_ProgramInterface, a_Name );
		}

		static FORCEINLINE GLint GetProgramResourceLocationIndex( GLuint a_Program, GLenum a_ProgramInterface, const GLchar* a_Name )
		{
			return glGetProgramResourceLocationIndex( a_Program, a_ProgramInterface, a_Name );
		}

		static FORCEINLINE void ShaderStorageBlockBinding( GLuint a_Program, GLuint a_StorageBlockIndex, GLuint a_Binding )
		{
			glShaderStorageBlockBinding( a_Program, a_StorageBlockIndex, a_Binding );
		}

		static FORCEINLINE void TexBufferRange( GLenum a_Target, GLenum a_Internalformat, GLuint a_Buffer, GLintptr a_Offset, GLsizeiptr a_Size )
		{
			glTexBufferRange( a_Target, a_Internalformat, a_Buffer, a_Offset, a_Size );
		}

		static FORCEINLINE void TexStorage2DMultisample( GLenum a_Target, GLsizei a_Samples, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height, GLboolean a_Fixedsamplelocations )
		{
			glTexStorage2DMultisample( a_Target, a_Samples, a_Internalformat, a_Width, a_Height, a_Fixedsamplelocations );
		}

		static FORCEINLINE void TexStorage3DMultisample( GLenum a_Target, GLsizei a_Samples, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLboolean a_Fixedsamplelocations )
		{
			glTexStorage3DMultisample( a_Target, a_Samples, a_Internalformat, a_Width, a_Height, a_Depth, a_Fixedsamplelocations );
		}

		static FORCEINLINE void TextureView( GLuint a_Texture, GLenum a_Target, GLuint a_ViewName, GLenum a_Internalformat, GLuint a_Minlevel, GLuint a_Numlevels, GLuint a_Minlayer, GLuint a_Numlayers )
		{
			glTextureView( a_Texture, a_Target, a_ViewName, a_Internalformat, a_Minlevel, a_Numlevels, a_Minlayer, a_Numlayers );
		}

		static FORCEINLINE void BindVertexBuffer( GLuint a_Bindingindex, GLuint a_Buffer, GLintptr a_Offset, GLsizei a_Stride )
		{
			glBindVertexBuffer( a_Bindingindex, a_Buffer, a_Offset, a_Stride );
		}

		static FORCEINLINE void VertexAttribFormat( GLuint a_Attribindex, GLint a_Size, GLenum a_Type, GLboolean a_Normalized, GLuint a_Relativeoffset )
		{
			glVertexAttribFormat( a_Attribindex, a_Size, a_Type, a_Normalized, a_Relativeoffset );
		}

		static FORCEINLINE void VertexAttribIFormat( GLuint a_Attribindex, GLint a_Size, GLenum a_Type, GLuint a_Relativeoffset )
		{
			glVertexAttribIFormat( a_Attribindex, a_Size, a_Type, a_Relativeoffset );
		}

		static FORCEINLINE void VertexAttribLFormat( GLuint a_Attribindex, GLint a_Size, GLenum a_Type, GLuint a_Relativeoffset )
		{
			glVertexAttribLFormat( a_Attribindex, a_Size, a_Type, a_Relativeoffset );
		}

		static FORCEINLINE void VertexAttribBinding( GLuint a_Attribindex, GLuint a_Bindingindex )
		{
			glVertexAttribBinding( a_Attribindex, a_Bindingindex );
		}

		static FORCEINLINE void VertexBindingDivisor( GLuint a_Bindingindex, GLuint a_Divisor )
		{
			glVertexBindingDivisor( a_Bindingindex, a_Divisor );
		}

		static FORCEINLINE void DebugMessageControl( GLenum a_Source, GLenum a_Type, GLenum a_Severity, GLsizei a_Count, const GLuint* a_Id, GLboolean a_Enabled )
		{
			glDebugMessageControl( a_Source, a_Type, a_Severity, a_Count, a_Id, a_Enabled );
		}

		static FORCEINLINE void DebugMessageInsert( GLenum a_Source, GLenum a_Type, GLuint a_Id, GLenum a_Severity, GLsizei a_Length, const GLchar* a_Buf )
		{
			glDebugMessageInsert( a_Source, a_Type, a_Id, a_Severity, a_Length, a_Buf );
		}

		static FORCEINLINE void DebugMessageCallback( GLDEBUGPROC a_Callback, const GLvoid* a_UserParam )
		{
			glDebugMessageCallback( a_Callback, a_UserParam );
		}

		static FORCEINLINE GLuint GetDebugMessageLog( GLuint a_Count, GLsizei a_BufSize, GLenum* a_Sources, GLenum* a_Types, GLuint* a_Id, GLenum* a_Severities, GLsizei* a_Length, GLchar* a_MessageLog )
		{
			return glGetDebugMessageLog( a_Count, a_BufSize, a_Sources, a_Types, a_Id, a_Severities, a_Length, a_MessageLog );
		}

		static FORCEINLINE void PushDebugGroup( GLenum a_Source, GLuint a_Id, GLsizei a_Length, const GLchar* a_Message )
		{
			glPushDebugGroup( a_Source, a_Id, a_Length, a_Message );
		}

		static FORCEINLINE void PopDebugGroup( void )
		{
			glPopDebugGroup();
		}

		static FORCEINLINE void ObjectLabel( GLenum a_Identifier, GLuint a_Name, GLsizei a_Length, const GLchar* a_Label )
		{
			glObjectLabel( a_Identifier, a_Name, a_Length, a_Label );
		}

		static FORCEINLINE void GetObjectLabel( GLenum a_Identifier, GLuint a_Name, GLsizei a_BufSize, GLsizei* a_Length, GLchar* a_Label )
		{
			glGetObjectLabel( a_Identifier, a_Name, a_BufSize, a_Length, a_Label );
		}

		static FORCEINLINE void ObjectPtrLabel( const GLvoid* a_Ptr, GLsizei a_Length, const GLchar* a_Label )
		{
			glObjectPtrLabel( a_Ptr, a_Length, a_Label );
		}

		static FORCEINLINE void GetObjectPtrLabel( const GLvoid* a_Ptr, GLsizei a_BufSize, GLsizei* a_Length, GLchar* a_Label )
		{
			glGetObjectPtrLabel( a_Ptr, a_BufSize, a_Length, a_Label );
		}

		static FORCEINLINE void GetPointerv( GLenum a_Pname, GLvoid** a_Params )
		{
			glGetPointerv( a_Pname, a_Params );
		}

	#pragma endregion

	#pragma region OpenGL 4.4

		static FORCEINLINE void BufferStorage( GLenum a_Target, GLsizeiptr a_Size, const GLvoid* a_Data, GLbitfield a_StorageFlags )
		{
			glBufferStorage( a_Target, a_Size, a_Data, a_StorageFlags );
		}

		static FORCEINLINE void ClearTexImage( GLuint a_Texture, GLint a_Level, GLenum a_Format, GLenum a_Type, const GLvoid* a_Data )
		{
			glClearTexImage( a_Texture, a_Level, a_Format, a_Type, a_Data );
		}

		static FORCEINLINE void ClearTexSubImage( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_Zoffset, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLenum a_Format, GLenum a_Type, const GLvoid* a_Data )
		{
			glClearTexSubImage( a_Texture, a_Level, a_Xoffset, a_Yoffset, a_Zoffset, a_Width, a_Height, a_Depth, a_Format, a_Type, a_Data );
		}

		static FORCEINLINE void BindBuffersBase( GLenum a_Target, GLuint a_First, GLsizei a_Count, const GLuint* a_Buffers )
		{
			glBindBuffersBase( a_Target, a_First, a_Count, a_Buffers );
		}

		static FORCEINLINE void BindBuffersRange( GLenum a_Target, GLuint a_First, GLsizei a_Count, const GLuint* a_Buffers, const GLintptr* a_Offsets, const GLsizeiptr* a_Sizes )
		{
			glBindBuffersRange( a_Target, a_First, a_Count, a_Buffers, a_Offsets, a_Sizes );
		}

		static FORCEINLINE void BindTextures( GLuint a_First, GLsizei a_Count, const GLuint* a_Textures )
		{
			glBindTextures( a_First, a_Count, a_Textures );
		}

		static FORCEINLINE void BindSamplers( GLuint a_First, GLsizei a_Count, const GLuint* a_Samplers )
		{
			glBindSamplers( a_First, a_Count, a_Samplers );
		}

		static FORCEINLINE void BindImageTextures( GLuint a_First, GLsizei a_Count, const GLuint* a_Textures )
		{
			glBindImageTextures( a_First, a_Count, a_Textures );
		}

		static FORCEINLINE void BindVertexBuffers( GLuint a_First, GLsizei a_Count, const GLuint* a_Buffers, const GLintptr* a_Offsets, const GLsizei* a_Strides )
		{
			glBindVertexBuffers( a_First, a_Count, a_Buffers, a_Offsets, a_Strides );
		}

	#pragma endregion

	#pragma region OpenGL 4.5

		static FORCEINLINE void ClipControl( GLenum a_Origin, GLenum a_Depth )
		{
			glClipControl( a_Origin, a_Depth );
		}

		static FORCEINLINE void CreateTransformFeedbacks( GLsizei a_N, GLuint* a_Tf )
		{
			glCreateTransformFeedbacks( a_N, a_Tf );
		}

		static FORCEINLINE void TransformFeedbackBufferBase( GLuint a_Xfb, GLuint a_Index, GLuint a_Buffer )
		{
			glTransformFeedbackBufferBase( a_Xfb, a_Index, a_Buffer );
		}

		static FORCEINLINE void TransformFeedbackBufferRange( GLuint a_Xfb, GLuint a_Index, GLuint a_Buffer, GLintptr a_Offset, GLsizeiptr a_Size )
		{
			glTransformFeedbackBufferRange( a_Xfb, a_Index, a_Buffer, a_Offset, a_Size );
		}

		static FORCEINLINE void GetTransformFeedbackiv( GLuint a_Xfb, GLenum a_Pname, GLint* a_Param )
		{
			glGetTransformFeedbackiv( a_Xfb, a_Pname, a_Param );
		}

		static FORCEINLINE void GetTransformFeedbacki_v( GLuint a_Xfb, GLenum a_Pname, GLuint a_Index, GLint* a_Param )
		{
			glGetTransformFeedbacki_v( a_Xfb, a_Pname, a_Index, a_Param );
		}

		static FORCEINLINE void GetTransformFeedbacki64_v( GLuint a_Xfb, GLenum a_Pname, GLuint a_Index, GLint64* a_Param )
		{
			glGetTransformFeedbacki64_v( a_Xfb, a_Pname, a_Index, a_Param );
		}

		static FORCEINLINE void CreateBuffers( GLsizei a_N, GLuint* a_Buffers )
		{
			glCreateBuffers( a_N, a_Buffers );
		}

		static FORCEINLINE void NamedBufferStorage( GLuint a_Buffer, GLsizeiptr a_Size, const GLvoid* a_Data, GLbitfield a_StorageFlags )
		{
			glNamedBufferStorage( a_Buffer, a_Size, a_Data, a_StorageFlags );
		}

		static FORCEINLINE void NamedBufferData( GLuint a_Buffer, GLsizeiptr a_Size, const GLvoid* a_Data, GLenum a_Usage )
		{
			glNamedBufferData( a_Buffer, a_Size, a_Data, a_Usage );
		}

		static FORCEINLINE void NamedBufferSubData( GLuint a_Buffer, GLintptr a_Offset, GLsizeiptr a_Size, const GLvoid* a_Data )
		{
			glNamedBufferSubData( a_Buffer, a_Offset, a_Size, a_Data );
		}

		static FORCEINLINE void CopyNamedBufferSubData( GLuint a_ReadBuffer, GLuint a_WriteBuffer, GLintptr a_ReadOffset, GLintptr a_WriteOffset, GLsizeiptr a_Size )
		{
			glCopyNamedBufferSubData( a_ReadBuffer, a_WriteBuffer, a_ReadOffset, a_WriteOffset, a_Size );
		}

		static FORCEINLINE void ClearNamedBufferData( GLuint a_Buffer, GLenum a_Internalformat, GLenum a_Format, GLenum a_Type, const GLvoid* a_Data )
		{
			glClearNamedBufferData( a_Buffer, a_Internalformat, a_Format, a_Type, a_Data );
		}

		static FORCEINLINE void ClearNamedBufferSubData( GLuint a_Buffer, GLenum a_Internalformat, GLintptr a_Offset, GLsizeiptr a_Size, GLenum a_Format, GLenum a_Type, const GLvoid* a_Data )
		{
			glClearNamedBufferSubData( a_Buffer, a_Internalformat, a_Offset, a_Size, a_Format, a_Type, a_Data );
		}

		static FORCEINLINE void MapNamedBuffer( GLuint a_Buffer, GLenum a_Access )
		{
			glMapNamedBuffer( a_Buffer, a_Access );
		}

		static FORCEINLINE void MapNamedBufferRange( GLuint a_Buffer, GLintptr a_Offset, GLsizeiptr a_Length, GLbitfield a_Access )
		{
			glMapNamedBufferRange( a_Buffer, a_Offset, a_Length, a_Access );
		}

		static FORCEINLINE GLboolean UnmapNamedBuffer( GLuint a_Buffer )
		{
			return glUnmapNamedBuffer( a_Buffer );
		}

		static FORCEINLINE void FlushMappedNamedBufferRange( GLuint a_Buffer, GLintptr a_Offset, GLsizeiptr a_Length )
		{
			glFlushMappedNamedBufferRange( a_Buffer, a_Offset, a_Length );
		}

		static FORCEINLINE void GetNamedBufferParameteriv( GLuint a_Buffer, GLenum a_Pname, GLint* a_Param )
		{
			glGetNamedBufferParameteriv( a_Buffer, a_Pname, a_Param );
		}

		static FORCEINLINE void GetNamedBufferParameteri64v( GLuint a_Buffer, GLenum a_Pname, GLint64* a_Param )
		{
			glGetNamedBufferParameteri64v( a_Buffer, a_Pname, a_Param );
		}

		static FORCEINLINE void GetNamedBufferPointerv( GLuint a_Buffer, GLenum a_Pname, GLvoid** a_Param )
		{
			glGetNamedBufferPointerv( a_Buffer, a_Pname, a_Param );
		}

		static FORCEINLINE void CreateFramebuffers( GLsizei a_N, GLuint* a_Framebuffers )
		{
			glCreateFramebuffers( a_N, a_Framebuffers );
		}

		static FORCEINLINE void NamedFramebufferRenderbuffer( GLuint a_Framebuffer, GLenum a_Attachment, GLenum a_Renderbuffertarget, GLuint a_Renderbuffer )
		{
			glNamedFramebufferRenderbuffer( a_Framebuffer, a_Attachment, a_Renderbuffertarget, a_Renderbuffer );
		}

		static FORCEINLINE void NamedFramebufferParameteri( GLuint a_Framebuffer, GLenum a_Pname, GLint a_Param )
		{
			glNamedFramebufferParameteri( a_Framebuffer, a_Pname, a_Param );
		}

		static FORCEINLINE void NamedFramebufferTexture( GLuint a_Framebuffer, GLenum a_Attachment, GLuint a_Texture, GLint a_Level )
		{
			glNamedFramebufferTexture( a_Framebuffer, a_Attachment, a_Texture, a_Level );
		}

		static FORCEINLINE void NamedFramebufferTextureLayer( GLuint a_Framebuffer, GLenum a_Attachment, GLuint a_Texture, GLint a_Level, GLint a_Layer )
		{
			glNamedFramebufferTextureLayer( a_Framebuffer, a_Attachment, a_Texture, a_Level, a_Layer );
		}

		static FORCEINLINE void NamedFramebufferDrawBuffer( GLuint a_Framebuffer, GLenum a_Buf )
		{
			glNamedFramebufferDrawBuffer( a_Framebuffer, a_Buf );
		}

		static FORCEINLINE void NamedFramebufferDrawBuffers( GLuint a_Framebuffer, GLsizei a_N, const GLenum* a_Bufs )
		{
			glNamedFramebufferDrawBuffers( a_Framebuffer, a_N, a_Bufs );
		}

		static FORCEINLINE void NamedFramebufferReadBuffer( GLuint a_Framebuffer, GLenum a_Buf )
		{
			glNamedFramebufferReadBuffer( a_Framebuffer, a_Buf );
		}

		static FORCEINLINE void InvalidateNamedFramebufferData( GLuint a_Framebuffer, GLsizei a_NumAttachments, const GLenum* a_Attachment )
		{
			glInvalidateNamedFramebufferData( a_Framebuffer, a_NumAttachments, a_Attachment );
		}

		static FORCEINLINE void InvalidateNamedFramebufferSubData( GLuint a_Framebuffer, GLsizei a_NumAttachments, const GLenum* a_Attachment, GLint a_X, GLint a_Y, GLsizei a_Width, GLsizei a_Height )
		{
			glInvalidateNamedFramebufferSubData( a_Framebuffer, a_NumAttachments, a_Attachment, a_X, a_Y, a_Width, a_Height );
		}

		static FORCEINLINE void ClearNamedFramebufferiv( GLuint a_Framebuffer, GLenum a_Buffer, GLint a_Drawbuffer, const GLint* a_Value )
		{
			glClearNamedFramebufferiv( a_Framebuffer, a_Buffer, a_Drawbuffer, a_Value );
		}

		static FORCEINLINE void ClearNamedFramebufferuiv( GLuint a_Framebuffer, GLenum a_Buffer, GLint a_Drawbuffer, const GLuint* a_Value )
		{
			glClearNamedFramebufferuiv( a_Framebuffer, a_Buffer, a_Drawbuffer, a_Value );
		}

		static FORCEINLINE void ClearNamedFramebufferfv( GLuint a_Framebuffer, GLenum a_Buffer, GLint a_Drawbuffer, const GLfloat* a_Value )
		{
			glClearNamedFramebufferfv( a_Framebuffer, a_Buffer, a_Drawbuffer, a_Value );
		}

		static FORCEINLINE void ClearNamedFramebufferfi( GLuint a_Framebuffer, GLenum a_Buffer, GLint a_Drawbuffer, GLfloat a_Depth, GLint a_Stencil )
		{
			glClearNamedFramebufferfi( a_Framebuffer, a_Buffer, a_Drawbuffer, a_Depth, a_Stencil );
		}

		static FORCEINLINE void BlitNamedFramebuffer( GLuint a_ReadFramebuffer, GLuint a_DrawFramebuffer, GLint a_SrcX0, GLint a_SrcY0, GLint a_SrcX1, GLint a_SrcY1, GLint a_DstX0, GLint a_DstY0, GLint a_DstX1, GLint a_DstY1, GLbitfield a_BufferMask, GLenum a_Filter )
		{
			glBlitNamedFramebuffer( a_ReadFramebuffer, a_DrawFramebuffer, a_SrcX0, a_SrcY0, a_SrcX1, a_SrcY1, a_DstX0, a_DstY0, a_DstX1, a_DstY1, a_BufferMask, a_Filter );
		}

		static FORCEINLINE GLenum CheckNamedFramebufferStatus( GLuint a_Framebuffer, GLenum a_Target )
		{
			return glCheckNamedFramebufferStatus( a_Framebuffer, a_Target );
		}

		static FORCEINLINE void GetNamedFramebufferParameteriv( GLuint a_Framebuffer, GLenum a_Pname, GLint* a_Param )
		{
			glGetNamedFramebufferParameteriv( a_Framebuffer, a_Pname, a_Param );
		}

		static FORCEINLINE void GetNamedFramebufferAttachmentParameteriv( GLuint a_Framebuffer, GLenum a_Attachment, GLenum a_Pname, GLint* a_Param )
		{
			glGetNamedFramebufferAttachmentParameteriv( a_Framebuffer, a_Attachment, a_Pname, a_Param );
		}

		static FORCEINLINE void CreateRenderbuffers( GLsizei a_N, GLuint* a_Renderbuffers )
		{
			glCreateRenderbuffers( a_N, a_Renderbuffers );
		}

		static FORCEINLINE void NamedRenderbufferStorage( GLuint a_Renderbuffer, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height )
		{
			glNamedRenderbufferStorage( a_Renderbuffer, a_Internalformat, a_Width, a_Height );
		}

		static FORCEINLINE void NamedRenderbufferStorageMultisample( GLuint a_Renderbuffer, GLsizei a_Samples, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height )
		{
			glNamedRenderbufferStorageMultisample( a_Renderbuffer, a_Samples, a_Internalformat, a_Width, a_Height );
		}

		static FORCEINLINE void GetNamedRenderbufferParameteriv( GLuint a_Renderbuffer, GLenum a_Pname, GLint* a_Param )
		{
			glGetNamedRenderbufferParameteriv( a_Renderbuffer, a_Pname, a_Param );
		}

		static FORCEINLINE void CreateTextures( GLenum a_Target, GLsizei a_N, GLuint* a_Textures )
		{
			glCreateTextures( a_Target, a_N, a_Textures );
		}

		static FORCEINLINE void TextureBuffer( GLuint a_Texture, GLenum a_Internalformat, GLuint a_Buffer )
		{
			glTextureBuffer( a_Texture, a_Internalformat, a_Buffer );
		}

		static FORCEINLINE void TextureBufferRange( GLuint a_Texture, GLenum a_Internalformat, GLuint a_Buffer, GLintptr a_Offset, GLsizeiptr a_Size )
		{
			glTextureBufferRange( a_Texture, a_Internalformat, a_Buffer, a_Offset, a_Size );
		}

		static FORCEINLINE void TextureStorage1D( GLuint a_Texture, GLsizei a_Levels, GLenum a_Internalformat, GLsizei a_Width )
		{
			glTextureStorage1D( a_Texture, a_Levels, a_Internalformat, a_Width );
		}

		//static FORCEINLINE void TextureStorage2D( GLuint a_Texture, GLsizei a_Levels, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height )
		//{
		//	glTextureStorage2D( a_Texture, a_Levels, a_Internalformat, a_Width, a_Height );
		//}

		//static FORCEINLINE void TextureStorage3D( GLuint a_Texture, GLsizei a_Levels, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth )
		//{
		//	glTextureStorage3D( a_Texture, a_Levels, a_Internalformat, a_Width, a_Height, a_Depth );
		//}

		static FORCEINLINE void TextureStorage2DMultisample( GLuint a_Texture, GLsizei a_Samples, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height, GLboolean a_Fixedsamplelocations )
		{
			glTextureStorage2DMultisample( a_Texture, a_Samples, a_Internalformat, a_Width, a_Height, a_Fixedsamplelocations );
		}

		static FORCEINLINE void TextureStorage3DMultisample( GLuint a_Texture, GLsizei a_Samples, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLboolean a_Fixedsamplelocations )
		{
			glTextureStorage3DMultisample( a_Texture, a_Samples, a_Internalformat, a_Width, a_Height, a_Depth, a_Fixedsamplelocations );
		}

		static FORCEINLINE void TextureSubImage1D( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLsizei a_Width, GLenum a_Format, GLenum a_Type, const GLvoid* a_Pixels )
		{
			glTextureSubImage1D( a_Texture, a_Level, a_Xoffset, a_Width, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void TextureSubImage2D( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLsizei a_Width, GLsizei a_Height, GLenum a_Format, GLenum a_Type, const GLvoid* a_Pixels )
		{
			glTextureSubImage2D( a_Texture, a_Level, a_Xoffset, a_Yoffset, a_Width, a_Height, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void TextureSubImage3D( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_Zoffset, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLenum a_Format, GLenum a_Type, const GLvoid* a_Pixels )
		{
			glTextureSubImage3D( a_Texture, a_Level, a_Xoffset, a_Yoffset, a_Zoffset, a_Width, a_Height, a_Depth, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void CompressedTextureSubImage1D( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLsizei a_Width, GLenum a_Format, GLsizei a_ImageSize, const GLvoid* a_Data )
		{
			glCompressedTextureSubImage1D( a_Texture, a_Level, a_Xoffset, a_Width, a_Format, a_ImageSize, a_Data );
		}

		static FORCEINLINE void CompressedTextureSubImage2D( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLsizei a_Width, GLsizei a_Height, GLenum a_Format, GLsizei a_ImageSize, const GLvoid* a_Data )
		{
			glCompressedTextureSubImage2D( a_Texture, a_Level, a_Xoffset, a_Yoffset, a_Width, a_Height, a_Format, a_ImageSize, a_Data );
		}

		static FORCEINLINE void CompressedTextureSubImage3D( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_Zoffset, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLenum a_Format, GLsizei a_ImageSize, const GLvoid* a_Data )
		{
			glCompressedTextureSubImage3D( a_Texture, a_Level, a_Xoffset, a_Yoffset, a_Zoffset, a_Width, a_Height, a_Depth, a_Format, a_ImageSize, a_Data );
		}

		static FORCEINLINE void CopyTextureSubImage1D( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLint a_X, GLint a_Y, GLsizei a_Width )
		{
			glCopyTextureSubImage1D( a_Texture, a_Level, a_Xoffset, a_X, a_Y, a_Width );
		}

		static FORCEINLINE void CopyTextureSubImage2D( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_X, GLint a_Y, GLsizei a_Width, GLsizei a_Height )
		{
			glCopyTextureSubImage2D( a_Texture, a_Level, a_Xoffset, a_Yoffset, a_X, a_Y, a_Width, a_Height );
		}

		static FORCEINLINE void CopyTextureSubImage3D( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_Zoffset, GLint a_X, GLint a_Y, GLsizei a_Width, GLsizei a_Height )
		{
			glCopyTextureSubImage3D( a_Texture, a_Level, a_Xoffset, a_Yoffset, a_Zoffset, a_X, a_Y, a_Width, a_Height );
		}

		static FORCEINLINE void TextureParameterf( GLuint a_Texture, GLenum a_Pname, GLfloat a_Param )
		{
			glTextureParameterf( a_Texture, a_Pname, a_Param );
		}

		static FORCEINLINE void TextureParameterfv( GLuint a_Texture, GLenum a_Pname, const GLfloat* a_Param )
		{
			glTextureParameterfv( a_Texture, a_Pname, a_Param );
		}

		static FORCEINLINE void TextureParameteri( GLuint a_Texture, GLenum a_Pname, GLint a_Param )
		{
			glTextureParameteri( a_Texture, a_Pname, a_Param );
		}

		static FORCEINLINE void TextureParameterIiv( GLuint a_Texture, GLenum a_Pname, const GLint* a_Param )
		{
			glTextureParameterIiv( a_Texture, a_Pname, a_Param );
		}

		static FORCEINLINE void TextureParameterIuiv( GLuint a_Texture, GLenum a_Pname, const GLuint* a_Param )
		{
			glTextureParameterIuiv( a_Texture, a_Pname, a_Param );
		}

		static FORCEINLINE void TextureParameteriv( GLuint a_Texture, GLenum a_Pname, const GLint* a_Param )
		{
			glTextureParameteriv( a_Texture, a_Pname, a_Param );
		}

		static FORCEINLINE void GenerateTextureMipmap( GLuint a_Texture )
		{
			glGenerateTextureMipmap( a_Texture );
		}

		static FORCEINLINE void BindTextureUnit( GLuint a_Unit, GLuint a_Texture )
		{
			glBindTextureUnit( a_Unit, a_Texture );
		}

		static FORCEINLINE void GetTextureImage( GLuint a_Texture, GLint a_Level, GLenum a_Format, GLenum a_Type, GLsizei a_BufSize, GLvoid* a_Pixels )
		{
			glGetTextureImage( a_Texture, a_Level, a_Format, a_Type, a_BufSize, a_Pixels );
		}

		static FORCEINLINE void GetCompressedTextureImage( GLuint a_Texture, GLint a_Level, GLsizei a_BufSize, GLvoid* a_Pixels )
		{
			glGetCompressedTextureImage( a_Texture, a_Level, a_BufSize, a_Pixels );
		}

		static FORCEINLINE void GetTextureLevelParameterfv( GLuint a_Texture, GLint a_Level, GLenum a_Pname, GLfloat* a_Param )
		{
			glGetTextureLevelParameterfv( a_Texture, a_Level, a_Pname, a_Param );
		}

		static FORCEINLINE void GetTextureLevelParameteriv( GLuint a_Texture, GLint a_Level, GLenum a_Pname, GLint* a_Param )
		{
			glGetTextureLevelParameteriv( a_Texture, a_Level, a_Pname, a_Param );
		}

		static FORCEINLINE void GetTextureParameterfv( GLuint a_Texture, GLenum a_Pname, GLfloat* a_Param )
		{
			glGetTextureParameterfv( a_Texture, a_Pname, a_Param );
		}

		static FORCEINLINE void GetTextureParameterIiv( GLuint a_Texture, GLenum a_Pname, GLint* a_Param )
		{
			glGetTextureParameterIiv( a_Texture, a_Pname, a_Param );
		}

		static FORCEINLINE void GetTextureParameterIuiv( GLuint a_Texture, GLenum a_Pname, GLuint* a_Param )
		{
			glGetTextureParameterIuiv( a_Texture, a_Pname, a_Param );
		}

		static FORCEINLINE void GetTextureParameteriv( GLuint a_Texture, GLenum a_Pname, GLint* a_Param )
		{
			glGetTextureParameteriv( a_Texture, a_Pname, a_Param );
		}

		static FORCEINLINE void CreateVertexArrays( GLsizei a_N, GLuint* a_Arrays )
		{
			glCreateVertexArrays( a_N, a_Arrays );
		}

		static FORCEINLINE void DisableVertexArrayAttrib( GLuint a_Array, GLuint a_Index )
		{
			glDisableVertexArrayAttrib( a_Array, a_Index );
		}

		static FORCEINLINE void EnableVertexArrayAttrib( GLuint a_Array, GLuint a_Index )
		{
			glEnableVertexArrayAttrib( a_Array, a_Index );
		}

		static FORCEINLINE void VertexArrayElementBuffer( GLuint a_Array, GLuint a_Buffer )
		{
			glVertexArrayElementBuffer( a_Array, a_Buffer );
		}

		static FORCEINLINE void VertexArrayVertexBuffer( GLuint a_Array, GLuint a_Bindingindex, GLuint a_Buffer, GLintptr a_Offset, GLsizei a_Stride )
		{
			glVertexArrayVertexBuffer( a_Array, a_Bindingindex, a_Buffer, a_Offset, a_Stride );
		}

		static FORCEINLINE void VertexArrayVertexBuffers( GLuint a_Array, GLuint a_First, GLsizei a_Count, const GLuint* a_Buffers, const GLintptr* a_Offsets, const GLsizei* a_Strides )
		{
			glVertexArrayVertexBuffers( a_Array, a_First, a_Count, a_Buffers, a_Offsets, a_Strides );
		}

		static FORCEINLINE void VertexArrayAttribBinding( GLuint a_Array, GLuint a_Attribindex, GLuint a_Bindingindex )
		{
			glVertexArrayAttribBinding( a_Array, a_Attribindex, a_Bindingindex );
		}

		static FORCEINLINE void VertexArrayAttribFormat( GLuint a_Array, GLuint a_Attribindex, GLint a_Size, GLenum a_Type, GLboolean a_Normalized, GLuint a_Relativeoffset )
		{
			glVertexArrayAttribFormat( a_Array, a_Attribindex, a_Size, a_Type, a_Normalized, a_Relativeoffset );
		}

		static FORCEINLINE void VertexArrayAttribIFormat( GLuint a_Array, GLuint a_Attribindex, GLint a_Size, GLenum a_Type, GLuint a_Relativeoffset )
		{
			glVertexArrayAttribIFormat( a_Array, a_Attribindex, a_Size, a_Type, a_Relativeoffset );
		}

		static FORCEINLINE void VertexArrayAttribLFormat( GLuint a_Array, GLuint a_Attribindex, GLint a_Size, GLenum a_Type, GLuint a_Relativeoffset )
		{
			glVertexArrayAttribLFormat( a_Array, a_Attribindex, a_Size, a_Type, a_Relativeoffset );
		}

		static FORCEINLINE void VertexArrayBindingDivisor( GLuint a_Array, GLuint a_Bindingindex, GLuint a_Divisor )
		{
			glVertexArrayBindingDivisor( a_Array, a_Bindingindex, a_Divisor );
		}

		static FORCEINLINE void GetVertexArrayiv( GLuint a_Array, GLenum a_Pname, GLint* a_Param )
		{
			glGetVertexArrayiv( a_Array, a_Pname, a_Param );
		}

		static FORCEINLINE void GetVertexArrayIndexediv( GLuint a_Array, GLuint a_Index, GLenum a_Pname, GLint* a_Param )
		{
			glGetVertexArrayIndexediv( a_Array, a_Index, a_Pname, a_Param );
		}

		static FORCEINLINE void GetVertexArrayIndexed64iv( GLuint a_Array, GLuint a_Index, GLenum a_Pname, GLint64* a_Param )
		{
			glGetVertexArrayIndexed64iv( a_Array, a_Index, a_Pname, a_Param );
		}

		static FORCEINLINE void CreateSamplers( GLsizei a_N, GLuint* a_Samplers )
		{
			glCreateSamplers( a_N, a_Samplers );
		}

		static FORCEINLINE void CreateProgramPipelines( GLsizei a_N, GLuint* a_Pipelines )
		{
			glCreateProgramPipelines( a_N, a_Pipelines );
		}

		static FORCEINLINE void CreateQueries( GLenum a_Target, GLsizei a_N, GLuint* a_Ids )
		{
			glCreateQueries( a_Target, a_N, a_Ids );
		}

		static FORCEINLINE void GetQueryBufferObjecti64v( GLuint a_Id, GLuint a_Buffer, GLenum a_Pname, GLintptr a_Offset )
		{
			glGetQueryBufferObjecti64v( a_Id, a_Buffer, a_Pname, a_Offset );
		}

		static FORCEINLINE void GetQueryBufferObjectiv( GLuint a_Id, GLuint a_Buffer, GLenum a_Pname, GLintptr a_Offset )
		{
			glGetQueryBufferObjectiv( a_Id, a_Buffer, a_Pname, a_Offset );
		}

		static FORCEINLINE void GetQueryBufferObjectui64v( GLuint a_Id, GLuint a_Buffer, GLenum a_Pname, GLintptr a_Offset )
		{
			glGetQueryBufferObjectui64v( a_Id, a_Buffer, a_Pname, a_Offset );
		}

		static FORCEINLINE void GetQueryBufferObjectuiv( GLuint a_Id, GLuint a_Buffer, GLenum a_Pname, GLintptr a_Offset )
		{
			glGetQueryBufferObjectuiv( a_Id, a_Buffer, a_Pname, a_Offset );
		}

		static FORCEINLINE void MemoryBarrierByRegion( GLbitfield a_Barriers )
		{
			glMemoryBarrierByRegion( a_Barriers );
		}

		static FORCEINLINE void GetTextureSubImage( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_Zoffset, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLenum a_Format, GLenum a_Type, GLsizei a_BufSize, GLvoid* a_Pixels )
		{
			glGetTextureSubImage( a_Texture, a_Level, a_Xoffset, a_Yoffset, a_Zoffset, a_Width, a_Height, a_Depth, a_Format, a_Type, a_BufSize, a_Pixels );
		}

		static FORCEINLINE void GetCompressedTextureSubImage( GLuint a_Texture, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_Zoffset, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLsizei a_BufSize, GLvoid* a_Pixels )
		{
			glGetCompressedTextureSubImage( a_Texture, a_Level, a_Xoffset, a_Yoffset, a_Zoffset, a_Width, a_Height, a_Depth, a_BufSize, a_Pixels );
		}

		static FORCEINLINE GLenum GetGraphicsResetStatus( void )
		{
			return glGetGraphicsResetStatus();
		}

		static FORCEINLINE void GetnCompressedTexImage( GLenum a_Target, GLint a_Level, GLsizei a_BufSize, GLvoid* a_Pixels )
		{
			glGetnCompressedTexImage( a_Target, a_Level, a_BufSize, a_Pixels );
		}

		static FORCEINLINE void GetnTexImage( GLenum a_Target, GLint a_Level, GLenum a_Format, GLenum a_Type, GLsizei a_BufSize, GLvoid* a_Pixels )
		{
			glGetnTexImage( a_Target, a_Level, a_Format, a_Type, a_BufSize, a_Pixels );
		}

		static FORCEINLINE void GetnUniformfv( GLuint a_Program, GLint a_Location, GLsizei a_BufSize, GLfloat* a_Params )
		{
			glGetnUniformfv( a_Program, a_Location, a_BufSize, a_Params );
		}

		static FORCEINLINE void GetnUniformiv( GLuint a_Program, GLint a_Location, GLsizei a_BufSize, GLint* a_Params )
		{
			glGetnUniformiv( a_Program, a_Location, a_BufSize, a_Params );
		}

		static FORCEINLINE void GetnUniformuiv( GLuint a_Program, GLint a_Location, GLsizei a_BufSize, GLuint* a_Params )
		{
			glGetnUniformuiv( a_Program, a_Location, a_BufSize, a_Params );
		}

		static FORCEINLINE void ReadnPixels( GLint a_X, GLint a_Y, GLsizei a_Width, GLsizei a_Height, GLenum a_Format, GLenum a_Type, GLsizei a_BufSize, GLvoid* a_Data )
		{
			glReadnPixels( a_X, a_Y, a_Width, a_Height, a_Format, a_Type, a_BufSize, a_Data );
		}

		static FORCEINLINE void GetnMapdv( GLenum a_Target, GLenum a_Query, GLsizei a_BufSize, GLdouble* a_V )
		{
			glGetnMapdv( a_Target, a_Query, a_BufSize, a_V );
		}

		static FORCEINLINE void GetnMapfv( GLenum a_Target, GLenum a_Query, GLsizei a_BufSize, GLfloat* a_V )
		{
			glGetnMapfv( a_Target, a_Query, a_BufSize, a_V );
		}

		static FORCEINLINE void GetnMapiv( GLenum a_Target, GLenum a_Query, GLsizei a_BufSize, GLint* a_V )
		{
			glGetnMapiv( a_Target, a_Query, a_BufSize, a_V );
		}

		static FORCEINLINE void GetnPixelMapfv( GLenum a_Map, GLsizei a_BufSize, GLfloat* a_Values )
		{
			glGetnPixelMapfv( a_Map, a_BufSize, a_Values );
		}

		static FORCEINLINE void GetnPixelMapuiv( GLenum a_Map, GLsizei a_BufSize, GLuint* a_Values )
		{
			glGetnPixelMapuiv( a_Map, a_BufSize, a_Values );
		}

		static FORCEINLINE void GetnPixelMapusv( GLenum a_Map, GLsizei a_BufSize, GLushort* a_Values )
		{
			glGetnPixelMapusv( a_Map, a_BufSize, a_Values );
		}

		static FORCEINLINE void GetnPolygonStipple( GLsizei a_BufSize, GLubyte* a_Pattern )
		{
			glGetnPolygonStipple( a_BufSize, a_Pattern );
		}

		static FORCEINLINE void GetnColorTable( GLenum a_Target, GLenum a_Format, GLenum a_Type, GLsizei a_BufSize, GLvoid* a_Table )
		{
			glGetnColorTable( a_Target, a_Format, a_Type, a_BufSize, a_Table );
		}

		static FORCEINLINE void GetnConvolutionFilter( GLenum a_Target, GLenum a_Format, GLenum a_Type, GLsizei a_BufSize, GLvoid* a_Image )
		{
			glGetnConvolutionFilter( a_Target, a_Format, a_Type, a_BufSize, a_Image );
		}

		static FORCEINLINE void GetnSeparableFilter( GLenum a_Target, GLenum a_Format, GLenum a_Type, GLsizei a_RowBufSize, GLvoid* a_Row, GLsizei a_ColumnBufSize, GLvoid* a_Column, GLvoid* a_Span )
		{
			glGetnSeparableFilter( a_Target, a_Format, a_Type, a_RowBufSize, a_Row, a_ColumnBufSize, a_Column, a_Span );
		}

		static FORCEINLINE void GetnHistogram( GLenum a_Target, GLboolean a_Reset, GLenum a_Format, GLenum a_Type, GLsizei a_BufSize, GLvoid* a_Values )
		{
			glGetnHistogram( a_Target, a_Reset, a_Format, a_Type, a_BufSize, a_Values );
		}

		static FORCEINLINE void GetnMinmax( GLenum a_Target, GLboolean a_Reset, GLenum a_Format, GLenum a_Type, GLsizei a_BufSize, GLvoid* a_Values )
		{
			glGetnMinmax( a_Target, a_Reset, a_Format, a_Type, a_BufSize, a_Values );
		}

		static FORCEINLINE void TextureBarrier( void )
		{
			glTextureBarrier();
		}

	#pragma endregion

	#pragma region OpenGL 4.6

		static FORCEINLINE void SpecializeShader( GLuint a_Shader, const GLchar* a_Entry, GLuint a_NumSpecializationConstants, const GLuint* a_ConstantIndexes, const GLuint* a_ConstantValues )
		{
			glSpecializeShader( a_Shader, a_Entry, a_NumSpecializationConstants, a_ConstantIndexes, a_ConstantValues );
		}

		static FORCEINLINE void MultiDrawArraysIndirectCount( GLenum a_Mode, const GLvoid* a_Indirect, GLintptr a_Drawcount, GLsizei a_Maxdrawcount, GLsizei a_Stride )
		{
			glMultiDrawArraysIndirectCount( a_Mode, a_Indirect, a_Drawcount, a_Maxdrawcount, a_Stride );
		}

		static FORCEINLINE void MultiDrawElementsIndirectCount( GLenum a_Mode, GLenum a_Type, const GLvoid* a_Indirect, GLintptr a_Drawcount, GLsizei a_Maxdrawcount, GLsizei a_Stride )
		{
			glMultiDrawElementsIndirectCount( a_Mode, a_Type, a_Indirect, a_Drawcount, a_Maxdrawcount, a_Stride );
		}

		static FORCEINLINE void PolygonOffsetClamp( GLfloat a_Factor, GLfloat a_Units, GLfloat a_Clamp )
		{
			glPolygonOffsetClamp( a_Factor, a_Units, a_Clamp );
		}

	#pragma endregion
    };


} // namespace Tridium