/* 

Amine Rehioui
Created: August 15th 2010

Uses code from the Irrlicht engine extension handler

*/

#include "Shoot.h"

#include "OpenGLExtensionHandlerWin32.h"

namespace shoot
{
	const char* const OpenGLExtensionHandlerWin32::ms_strExtensionStrings[OpenGLExtensionHandlerWin32::E_Count] =
	{
		"GL_EXT_framebuffer_object", // E_FBO
		"GL_ARB_multitexture", // E_MultiTexture
		"None", // "GL_ARB_texture_cube_map", // E_CubeMap // Disabled for consistency with DX11		
		"GL_ARB_point_parameters", // E_PointParams
		"GL_ARB_vertex_buffer_object", // E_VBO
		"None", // "GL_ARB_vertex_array_object" // E_VAO // Disabled for consistency with GLES
		"GL_ARB_fragment_shader", // E_VertexShader
		"GL_ARB_vertex_shader", // E_FragmentShader
		"GL_ARB_texture_rectangle", // E_RectangleTexture
		"GL_ARB_instanced_arrays", // E_Instancing
		"Native", // E_DepthTexture
		"Native", // E_PackedDepthStencil
	};

	//! Constructor
	OpenGLExtensionHandlerWin32::OpenGLExtensionHandlerWin32()
		: super(ms_strExtensionStrings)
		, pGlGenFramebuffersEXT(0)
		, pGlBindFramebufferEXT(0)
		, pGlFramebufferTexture2DEXT(0)
		, pGlDeleteFramebuffersEXT(0)
		, pGlCheckFramebufferStatusEXT(0)
		, pGlBindRenderbufferEXT(0)
		, pGlDeleteRenderbuffersEXT(0)
		, pGlGenRenderbuffersEXT(0)
		, pGlRenderbufferStorageEXT(0)
		, pGlFramebufferRenderbufferEXT(0)
		, pGlActiveTextureARB(0)
		, pGlClientActiveTextureARB(0)
		, pGlPointParameterfARB(0)
		, pGlPointParameterfvARB(0)
		, pGlBindBufferARB(0)
		, pGlDeleteBuffersARB(0)
		, pGlGenBuffersARB(0)
		, pGlBufferDataARB(0)
		, pGlMapBufferARB(0)
		, pGlUnmapBufferARB(0)
		, pGlBindVertexArray(0)
		, pGlDeleteVertexArrays(0)
		, pGlGenVertexArrays(0)
		, pGlCreateShaderObjectARB(0)
		, pGlDeleteObjectARB(0)
		, pGlShaderSourceARB(0)
		, pGlCompileShaderARB(0)
		, pGlCreateProgramObjectARB(0)
		, pGlAttachObjectARB(0)
		, pGlDetachObjectARB(0)
		, pGlLinkProgramARB(0)
		, pGlUseProgramObjectARB(0)
		, pGlGetObjectParameterivARB(0)
		, pGlGetInfoLogARB(0)
		, pGlGetUniformLocationARB(0)
		, pGlUniform1fvARB(0)
		, pGlUniform2fvARB(0)
		, pGlUniform3fvARB(0)
		, pGlUniform4fvARB(0)
		, pGlUniform1ivARB(0)
		, pGlUniform2ivARB(0)
		, pGlUniform3ivARB(0)
		, pGlUniform4ivARB(0)
		, pGlUniformMatrix2fvARB(0)
		, pGlUniformMatrix3fvARB(0)
		, pGlUniformMatrix4fvARB(0)
		, pGlGetAttribLocationARB(0)
		, pGlVertexAttrib1fvARB(0)
		, pGlVertexAttrib2fvARB(0)
		, pGlVertexAttrib3fvARB(0)
		, pGlVertexAttrib4fvARB(0)
		, pGlEnableVertexAttribArrayARB(0)
		, pGlDisableVertexAttribArrayARB(0)
		, pGlVertexAttribPointerARB(0)
		, pGlVertexAttribDivisorARB(0)
		, pGLDrawArraysInstancedARB(0)
		, pGLDrawElementsInstancedARB(0)
		, pGLGenerateMipMap(0)
	{
		pGlGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) wglGetProcAddress("glGenFramebuffersEXT");
		pGlBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) wglGetProcAddress("glBindFramebufferEXT");
		pGlFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) wglGetProcAddress("glFramebufferTexture2DEXT");
		pGlDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) wglGetProcAddress("glDeleteFramebuffersEXT");
		pGlCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatusEXT");
		pGlBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) wglGetProcAddress("glBindRenderbufferEXT");
		pGlDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) wglGetProcAddress("glDeleteRenderbuffersEXT");
		pGlGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) wglGetProcAddress("glGenRenderbuffersEXT");
		pGlRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) wglGetProcAddress("glRenderbufferStorageEXT");
		pGlFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) wglGetProcAddress("glFramebufferRenderbufferEXT");
		pGlActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		pGlClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
		pGlPointParameterfARB  = (PFNGLPOINTPARAMETERFARBPROC)wglGetProcAddress("glPointParameterfARB");
        pGlPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)wglGetProcAddress("glPointParameterfvARB");
		pGlBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
		pGlDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
		pGlGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
		pGlBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
		pGlMapBufferARB = (PFNGLMAPBUFFERARBPROC )wglGetProcAddress("glMapBufferARB");
		pGlUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC  )wglGetProcAddress("glUnmapBufferARB");
		pGlBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
		pGlDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
		pGlGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
		pGlCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
		pGlDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
		pGlShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
		pGlCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
		pGlCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
		pGlAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
		pGlDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)wglGetProcAddress("glDetachObjectARB");
		pGlLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
		pGlUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
		pGlGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
		pGlGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
		pGlGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
		pGlUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)wglGetProcAddress("glUniform1fvARB");
		pGlUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)wglGetProcAddress("glUniform2fvARB");
		pGlUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)wglGetProcAddress("glUniform3fvARB");
		pGlUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)wglGetProcAddress("glUniform4fvARB");
		pGlUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)wglGetProcAddress("glUniform1ivARB");
		pGlUniform2ivARB = (PFNGLUNIFORM2IVARBPROC)wglGetProcAddress("glUniform2ivARB");
		pGlUniform3ivARB = (PFNGLUNIFORM3IVARBPROC)wglGetProcAddress("glUniform3ivARB");
		pGlUniform4ivARB = (PFNGLUNIFORM4IVARBPROC)wglGetProcAddress("glUniform4ivARB");
		pGlUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)wglGetProcAddress("glUniformMatrix2fvARB");
		pGlUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)wglGetProcAddress("glUniformMatrix3fvARB");
		pGlUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)wglGetProcAddress("glUniformMatrix4fvARB");
		pGlGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)wglGetProcAddress("glGetAttribLocationARB");
		pGlVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)wglGetProcAddress("glVertexAttrib1fvARB");
		pGlVertexAttrib2fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)wglGetProcAddress("glVertexAttrib2fvARB");
		pGlVertexAttrib3fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)wglGetProcAddress("glVertexAttrib3fvARB");
		pGlVertexAttrib4fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)wglGetProcAddress("glVertexAttrib4fvARB");
		pGlEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)wglGetProcAddress("glEnableVertexAttribArrayARB");
		pGlDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)wglGetProcAddress("glDisableVertexAttribArrayARB");
		pGlVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)wglGetProcAddress("glVertexAttribPointerARB");
		pGlVertexAttribDivisorARB = (PFNGLVERTEXATTRIBDIVISORARBPROC)wglGetProcAddress("glVertexAttribDivisorARB");
		pGLDrawArraysInstancedARB = (PFNGLDRAWARRAYSINSTANCEDARBPROC)wglGetProcAddress("glDrawArraysInstancedARB");
		pGLDrawElementsInstancedARB = (PFNGLDRAWELEMENTSINSTANCEDARBPROC)wglGetProcAddress("glDrawElementsInstancedARB");
		pGLGenerateMipMap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	}

	// extensions
	void OpenGLExtensionHandlerWin32::extGlGenFramebuffers(GLsizei n, GLuint *framebuffers)
	{
		GL_CHECK(pGlGenFramebuffersEXT(n, framebuffers));
	}
	
	void OpenGLExtensionHandlerWin32::extGlBindFramebuffer(GLenum target, GLuint framebuffer)
	{
		GL_CHECK(pGlBindFramebufferEXT(target, framebuffer));
	}

	void OpenGLExtensionHandlerWin32::extGlFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
	{
		GL_CHECK(pGlFramebufferTexture2DEXT(target, attachment, textarget, texture, level));
	}

	void OpenGLExtensionHandlerWin32::extGlDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
	{
		GL_CHECK(pGlDeleteFramebuffersEXT(n, framebuffers));
	}

	GLenum OpenGLExtensionHandlerWin32::extGlCheckFramebufferStatus(GLenum target)
	{
		return pGlCheckFramebufferStatusEXT(target);
	}

	void OpenGLExtensionHandlerWin32::extGlBindRenderbuffer(GLenum target, GLuint renderbuffer)
	{
		GL_CHECK(pGlBindRenderbufferEXT(target, renderbuffer));
	}

	void OpenGLExtensionHandlerWin32::extGlDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
	{
		GL_CHECK(pGlDeleteRenderbuffersEXT(n, renderbuffers));
	}

	void OpenGLExtensionHandlerWin32::extGlGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
	{
		GL_CHECK(pGlGenRenderbuffersEXT(n, renderbuffers));
	}

	void OpenGLExtensionHandlerWin32::extGlRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
	{
		GL_CHECK(pGlRenderbufferStorageEXT(target, internalformat, width, height));
	}

	void OpenGLExtensionHandlerWin32::extGlFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
	{
		GL_CHECK(pGlFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer));
	}

	void OpenGLExtensionHandlerWin32::extGlActiveTexture(GLenum texture)
	{
		GL_CHECK(pGlActiveTextureARB(texture));
	}

	void OpenGLExtensionHandlerWin32::extGlBindBuffer(GLenum target, GLuint buffer)
	{
		GL_CHECK(pGlBindBufferARB(target, buffer));
	}

	void OpenGLExtensionHandlerWin32::extGlDeleteBuffers(GLsizei n, const GLuint *buffers)
	{
		GL_CHECK(pGlDeleteBuffersARB(n, buffers));
	}

	void OpenGLExtensionHandlerWin32::extGlGenBuffers(GLsizei n, GLuint *buffers)
	{
		GL_CHECK(pGlGenBuffersARB(n, buffers));
	}

	void OpenGLExtensionHandlerWin32::extGlBufferData(GLenum target, GLsizei size, const void* data, GLenum usage)
	{
		GL_CHECK(pGlBufferDataARB(target, size, data, usage));
	}

	void* OpenGLExtensionHandlerWin32::extGlMapBuffer(GLenum target, GLenum access)
	{
		return pGlMapBufferARB(target, access);
	}

	GLboolean OpenGLExtensionHandlerWin32::extGlUnmapBuffer(GLenum target)
	{
		return pGlUnmapBufferARB(target);
	}

	void OpenGLExtensionHandlerWin32::extGlGenVertexArrays(GLsizei n, GLuint *arrays)
	{
		GL_CHECK(pGlGenVertexArrays(n, arrays));
	}

	void OpenGLExtensionHandlerWin32::extGlBindVertexArray(GLuint array)
	{
		GL_CHECK(pGlBindVertexArray(array));
	}

	void OpenGLExtensionHandlerWin32::extGlDeleteVertexArrays(GLsizei n, const GLuint *arrays)
	{
		GL_CHECK(pGlDeleteVertexArrays(n, arrays));
	}

	void OpenGLExtensionHandlerWin32::extGlCreateShader(E_ShaderType type, GLuint *shader)
	{
		static const GLenum glTypes[ST_Count] = 
		{
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER
		};

		*shader = pGlCreateShaderObjectARB(glTypes[type]);
	}

	void OpenGLExtensionHandlerWin32::extGlDeleteShader(GLuint shader)
	{
		GL_CHECK(pGlDeleteObjectARB(shader));
	}

	void OpenGLExtensionHandlerWin32::extGlShaderSource(GLuint shader, GLsizei count, const char* *string, const GLint *length)
	{
		GL_CHECK(pGlShaderSourceARB(shader, count, string, length));
	}

	void OpenGLExtensionHandlerWin32::extGlCompileShader(GLuint shader)
	{
		GL_CHECK(pGlCompileShaderARB(shader));
	}

	void OpenGLExtensionHandlerWin32::extGlCreateProgram(GLuint* program)
	{
		*program = pGlCreateProgramObjectARB();
	}

	void OpenGLExtensionHandlerWin32::extGlDeleteProgram(GLuint program)
	{
		GL_CHECK(pGlDeleteObjectARB(program));
	}

	void OpenGLExtensionHandlerWin32::extGlAttachShader(GLuint program, GLuint shader)
	{
		GL_CHECK(pGlAttachObjectARB(program, shader));
	}

	void OpenGLExtensionHandlerWin32::extGlDetachShader(GLuint program, GLuint shader)
	{
		GL_CHECK(pGlDetachObjectARB(program, shader));
	}

	void OpenGLExtensionHandlerWin32::extGlLinkProgram(GLuint program)
	{
		GL_CHECK(pGlLinkProgramARB(program));
	}

	void OpenGLExtensionHandlerWin32::extGlUseProgram(GLuint program)
	{
		GL_CHECK(pGlUseProgramObjectARB(program));
	}

	bool OpenGLExtensionHandlerWin32::extGlCheckCompileStatus(GLuint shader, const std::string& path)
	{
		GLint status = GL_TRUE;
		pGlGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
		if(status != GL_TRUE)
		{
			GLint logsize = 0;
			pGlGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logsize);
			char *log = snew char[logsize+1];
			memset(log, 0, logsize+1);
			pGlGetInfoLogARB(shader, logsize, &logsize, log);
			SHOOT_ASSERT(false, "Shader '%s' compile error: %s", path.c_str(), log);
			sdelete_array(log);
			return false;
		}
		return true;
	}

	bool OpenGLExtensionHandlerWin32::extGlCheckLinkStatus(GLuint program, const std::string& path)
	{
		GLint status = GL_TRUE;
		pGlGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB , &status);
		if(status != GL_TRUE)
		{
			GLint logsize = 0;
			pGlGetObjectParameterivARB(program, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logsize);
			char *log = snew char[logsize + 1];
			memset(log, 0, logsize + 1);
			pGlGetInfoLogARB(program, logsize, &logsize, log);
			SHOOT_ASSERT(false, "Shader '%s' link error: %s", path.c_str(), log);
			sdelete_array(log);
			return false;
		}
		return true;
	}

	void OpenGLExtensionHandlerWin32::extGlGetUniformLocation(GLuint program, const char* name, GLint* location)
	{
		*location = pGlGetUniformLocationARB(program, name);
	}

	void OpenGLExtensionHandlerWin32::extGlUniform1fv(GLint location, GLsizei count, const GLfloat *value)
	{
		SHOOT_LOG_WARNING(location >= 0, "Invalid extGlUniform1fv location");
		GL_CHECK(pGlUniform1fvARB(location, count, value));
	}

	void OpenGLExtensionHandlerWin32::extGlUniform2fv(GLint location, GLsizei count, const GLfloat *value)
	{
		SHOOT_LOG_WARNING(location >= 0, "Invalid extGlUniform2fv location");
		GL_CHECK(pGlUniform2fvARB(location, count, value));
	}

	void OpenGLExtensionHandlerWin32::extGlUniform3fv(GLint location, GLsizei count, const GLfloat *value)
	{
		SHOOT_LOG_WARNING(location >= 0, "Invalid extGlUniform3fv location");
		GL_CHECK(pGlUniform3fvARB(location, count, value));
	}

	void OpenGLExtensionHandlerWin32::extGlUniform4fv(GLint location, GLsizei count, const GLfloat *value)
	{
		SHOOT_LOG_WARNING(location >= 0, "Invalid extGlUniform4fv location");
		GL_CHECK(pGlUniform4fvARB(location, count, value));
	}

	void OpenGLExtensionHandlerWin32::extGlUniform1iv(GLint location, GLsizei count, const GLint *value)
	{
		SHOOT_LOG_WARNING(location >= 0, "Invalid extGlUniform1iv location");
		GL_CHECK(pGlUniform1ivARB(location, count, value));
	}

	void OpenGLExtensionHandlerWin32::extGlUniform2iv(GLint location, GLsizei count, const GLint *value)
	{
		SHOOT_LOG_WARNING(location >= 0, "Invalid extGlUniform2iv location");
		GL_CHECK(pGlUniform2ivARB(location, count, value));
	}

	void OpenGLExtensionHandlerWin32::extGlUniform3iv(GLint location, GLsizei count, const GLint *value)
	{
		SHOOT_LOG_WARNING(location >= 0, "Invalid extGlUniform3iv location");
		GL_CHECK(pGlUniform3ivARB(location, count, value));
	}

	void OpenGLExtensionHandlerWin32::extGlUniform4iv(GLint location, GLsizei count, const GLint *value)
	{
		SHOOT_LOG_WARNING(location >= 0, "Invalid extGlUniform4iv location");
		GL_CHECK(pGlUniform4ivARB(location, count, value));
	}

	void OpenGLExtensionHandlerWin32::extGlUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	{
		SHOOT_LOG_WARNING(location >= 0, "Invalid extGlUniformMatrix2fv location");
		GL_CHECK(pGlUniformMatrix2fvARB(location, count, transpose, value));
	}

	void OpenGLExtensionHandlerWin32::extGlUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	{
		SHOOT_LOG_WARNING(location >= 0, "Invalid extGlUniformMatrix3fv location");
		GL_CHECK(pGlUniformMatrix3fvARB(location, count, transpose, value));
	}

	void OpenGLExtensionHandlerWin32::extGlUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	{
		SHOOT_LOG_WARNING(location >= 0, "Invalid extGlUniformMatrix4fv location");
		GL_CHECK(pGlUniformMatrix4fvARB(location, count, transpose, value));
	}

	void OpenGLExtensionHandlerWin32::extGlGetAttribLocation(GLuint program, const char* name, GLint* location)
	{
		*location = pGlGetAttribLocationARB(program, name);
	}

	void OpenGLExtensionHandlerWin32::extGlVertexAttrib1fv(GLuint index, const GLfloat *v)
	{
		GL_CHECK(pGlVertexAttrib1fvARB(index, v));
	}

	void OpenGLExtensionHandlerWin32::extGlVertexAttrib2fv(GLuint index, const GLfloat *v)
	{
		GL_CHECK(pGlVertexAttrib2fvARB(index, v));
	}

	void OpenGLExtensionHandlerWin32::extGlVertexAttrib3fv(GLuint index, const GLfloat *v)
	{
		GL_CHECK(pGlVertexAttrib3fvARB(index, v));
	}

	void OpenGLExtensionHandlerWin32::extGlVertexAttrib4fv(GLuint index, const GLfloat *v)
	{
		GL_CHECK(pGlVertexAttrib4fvARB(index, v));
	}

	void OpenGLExtensionHandlerWin32::extGlEnableVertexAttribArray(GLuint index)
	{
		GL_CHECK(pGlEnableVertexAttribArrayARB(index));
	}

	void OpenGLExtensionHandlerWin32::extGlDisableVertexAttribArray(GLuint index)
	{		
		GL_CHECK(pGlDisableVertexAttribArrayARB(index));		
	}

	void OpenGLExtensionHandlerWin32::extGlVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
	{
		GL_CHECK(pGlVertexAttribPointerARB(index, size, type, normalized, stride, pointer));
	}

	void OpenGLExtensionHandlerWin32::extGlVertexAttribDivisor(GLuint index, GLuint divisor)
	{
		GL_CHECK(pGlVertexAttribDivisorARB(index, divisor));
	}

	void OpenGLExtensionHandlerWin32::extGLDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount)
	{
		GL_CHECK(pGLDrawArraysInstancedARB(mode, first, count, primcount));
	}

	void OpenGLExtensionHandlerWin32::extGLDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount)
	{
		GL_CHECK(pGLDrawElementsInstancedARB(mode, count, type, indices, primcount));
	}

	void OpenGLExtensionHandlerWin32::extGlGenerateMipmap(GLenum target)
	{
		GL_CHECK(pGLGenerateMipMap(target));
	}
}

