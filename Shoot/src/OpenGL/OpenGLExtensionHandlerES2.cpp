/* 

Amine Rehioui
Created: February 8th 2014

*/

#include "Shoot.h"

#include "OpenGLExtensionHandlerES2.h"

namespace shoot
{
#if SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID	
	const char* const OpenGLExtensionHandlerES2::ms_strExtensionStrings[OpenGLExtensionHandlerES2::E_Count] =
	{
		"Native", // E_FBO
		"Native", // E_MultiTexture
		"None", // "GL_OES_texture_cube_map", // E_CubeMap // Disabled for consistency with DX11
		"Native", // E_PointParams
		"Native", // E_VBO
		"None", // E_VAO
		"Native", // E_VertexBuffer
		"Native", // E_FragmentShader
		"None", // E_RectangleTexture
		"None", // E_Instancing
		"GL_OES_depth_texture", // E_DepthTexture
		"GL_OES_packed_depth_stencil", // E_PackedDepthStencil
	};
#else
	const char* const OpenGLExtensionHandlerES2::ms_strExtensionStrings[OpenGLExtensionHandlerES2::E_Count] =
	{
		"Native", // E_FBO
		"Native", // E_MultiTexture
		"None", // "GL_OES_texture_cube_map", // E_CubeMap // Disabled for consistency with DX11
		"Native", // E_PointParams
		"Native", // E_VBO
		"GL_OES_vertex_array_object", // E_VAO
		"Native", // E_VertexBuffer
		"Native", // E_FragmentShader
		"None", // E_RectangleTexture
		"GL_EXT_draw_instanced", // E_Instancing
		"Native", // E_DepthTexture
		"Native", // E_PackedDepthStencil
	};	
#endif

	//! Constructor
	OpenGLExtensionHandlerES2::OpenGLExtensionHandlerES2()
		: super(ms_strExtensionStrings)
	{
	}

	// extensions
	void OpenGLExtensionHandlerES2::extGlGenFramebuffers(GLsizei n, GLuint *framebuffers)
	{
		GL_CHECK(glGenFramebuffers(n, framebuffers));		
	}
	
	void OpenGLExtensionHandlerES2::extGlBindFramebuffer(GLenum target, GLuint framebuffer)
	{
		GL_CHECK(glBindFramebuffer(target, framebuffer));
	}

	void OpenGLExtensionHandlerES2::extGlFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
	{
		glFramebufferTexture2D(target, attachment, textarget, texture, level);
	}

	void OpenGLExtensionHandlerES2::extGlDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
	{
		GL_CHECK(glDeleteFramebuffers(n, framebuffers));
	}

	GLenum OpenGLExtensionHandlerES2::extGlCheckFramebufferStatus(GLenum target)
	{
		return glCheckFramebufferStatus(target);
	}

	void OpenGLExtensionHandlerES2::extGlBindRenderbuffer(GLenum target, GLuint renderbuffer)
	{
		GL_CHECK(glBindRenderbuffer(target, renderbuffer));
	}

	void OpenGLExtensionHandlerES2::extGlDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
	{
		GL_CHECK(glDeleteRenderbuffers(n, renderbuffers));
	}

	void OpenGLExtensionHandlerES2::extGlGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
	{
		GL_CHECK(glGenRenderbuffers(n, renderbuffers));
	}

	void OpenGLExtensionHandlerES2::extGlRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
	{
		GL_CHECK(glRenderbufferStorage(target, internalformat, width, height));
	}

	void OpenGLExtensionHandlerES2::extGlFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
	{
		GL_CHECK(glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer));
	}

	void OpenGLExtensionHandlerES2::extGlActiveTexture(GLenum texture)
	{
		GL_CHECK(glActiveTexture(texture));
	}

	void OpenGLExtensionHandlerES2::extGlBindBuffer(GLenum target, GLuint buffer)
	{
		GL_CHECK(glBindBuffer(target, buffer));
	}

	void OpenGLExtensionHandlerES2::extGlDeleteBuffers(GLsizei n, const GLuint *buffers)
	{
		GL_CHECK(glDeleteBuffers(n, buffers));
	}

	void OpenGLExtensionHandlerES2::extGlGenBuffers(GLsizei n, GLuint *buffers)
	{
		GL_CHECK(glGenBuffers(n, buffers));
	}

	void OpenGLExtensionHandlerES2::extGlBufferData(GLenum target, GLsizei size, const void* data, GLenum usage)
	{
		GL_CHECK(glBufferData(target, size, data, usage));
	}

	void OpenGLExtensionHandlerES2::extGlGenVertexArrays(GLsizei n, GLuint *arrays)
	{
		#if SHOOT_PLATFORM != SHOOT_PLATFORM_ANDROID
			GL_CHECK(glGenVertexArraysOES(n, arrays));
		#endif
	}

	void OpenGLExtensionHandlerES2::extGlBindVertexArray(GLuint array)
	{
#if SHOOT_PLATFORM != SHOOT_PLATFORM_ANDROID
		GL_CHECK(glBindVertexArrayOES(array));
#endif
	}

	void OpenGLExtensionHandlerES2::extGlDeleteVertexArrays(GLsizei n, const GLuint *arrays)
	{
#if SHOOT_PLATFORM != SHOOT_PLATFORM_ANDROID
		GL_CHECK(glDeleteVertexArraysOES(n, arrays));
#endif
	}

	void OpenGLExtensionHandlerES2::extGlCreateShader(E_ShaderType type, GLuint *shader)
	{
		static const GLenum glTypes[ST_Count] = 
		{
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER
		};
		
		*shader = glCreateShader(glTypes[type]);
	}

	void OpenGLExtensionHandlerES2::extGlDeleteShader(GLuint shader)
	{
		GL_CHECK(glDeleteShader(shader));
	}

	void OpenGLExtensionHandlerES2::extGlShaderSource(GLuint shader, GLsizei count, const char* *string, const GLint *length)
	{
		GL_CHECK(glShaderSource(shader, count, string, length));
	}

	void OpenGLExtensionHandlerES2::extGlCompileShader(GLuint shader)
	{
		GL_CHECK(glCompileShader(shader));
	}

	void OpenGLExtensionHandlerES2::extGlCreateProgram(GLuint* program)
	{
		*program = glCreateProgram();
	}

	void OpenGLExtensionHandlerES2::extGlDeleteProgram(GLuint program)
	{
		GL_CHECK(glDeleteProgram(program));
	}

	void OpenGLExtensionHandlerES2::extGlAttachShader(GLuint program, GLuint shader)
	{
		GL_CHECK(glAttachShader(program, shader));
	}

	void OpenGLExtensionHandlerES2::extGlDetachShader(GLuint program, GLuint shader)
	{
		GL_CHECK(glDetachShader(program, shader));
	}

	void OpenGLExtensionHandlerES2::extGlLinkProgram(GLuint program)
	{
		GL_CHECK(glLinkProgram(program));
	}

	void OpenGLExtensionHandlerES2::extGlUseProgram(GLuint program)
	{
		GL_CHECK(glUseProgram(program));
	}

	bool OpenGLExtensionHandlerES2::extGlCheckCompileStatus(GLuint shader, const std::string& path)
	{
		GLint status = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if(status != GL_TRUE)
		{
			GLint logsize = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);			
			char *log = snew char[logsize + 1];
			memset(log, 0, logsize + 1);
			glGetShaderInfoLog(shader, logsize, &logsize, log);
			SHOOT_ASSERT(false, "Shader '%s' compile error: %s", path.c_str(), log);
			sdelete_array(log);
			return false;
		}
		return true;
	}

	bool OpenGLExtensionHandlerES2::extGlCheckLinkStatus(GLuint program, const std::string& path)
	{
		GLint status = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if(status != GL_TRUE)
		{
			GLint logsize = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logsize);			
			char *log = snew char[logsize + 1];
			memset(log, 0, logsize + 1);
			glGetProgramInfoLog(program, logsize, &logsize, log);
			SHOOT_ASSERT(false, "Shader '%s' link error: %s", path.c_str(), log);
			sdelete_array(log);
			return false;			
		}
		return true;
	}
	
	void OpenGLExtensionHandlerES2::extGlGetUniformLocation(GLuint program, const char* name, GLint* location)
	{
		*location = glGetUniformLocation(program, name);
	}
	
	void OpenGLExtensionHandlerES2::extGlUniform1fv(GLint location, GLsizei count, const GLfloat *value)
	{
		glUniform1fv(location, count, value);
	}
	
	void OpenGLExtensionHandlerES2::extGlUniform2fv(GLint location, GLsizei count, const GLfloat *value)
	{
		glUniform2fv(location, count, value);
	}
	
	void OpenGLExtensionHandlerES2::extGlUniform3fv(GLint location, GLsizei count, const GLfloat *value)
	{
		glUniform3fv(location, count, value);
	}
	
	void OpenGLExtensionHandlerES2::extGlUniform4fv(GLint location, GLsizei count, const GLfloat *value)
	{
		glUniform4fv(location, count, value);
	}
	
	void OpenGLExtensionHandlerES2::extGlUniform1iv(GLint location, GLsizei count, const GLint *value)
	{
		glUniform1iv(location, count, value);
	}
	
	void OpenGLExtensionHandlerES2::extGlUniform2iv(GLint location, GLsizei count, const GLint *value)
	{
		glUniform2iv(location, count, value);
	}
	
	void OpenGLExtensionHandlerES2::extGlUniform3iv(GLint location, GLsizei count, const GLint *value)
	{
		glUniform3iv(location, count, value);
	}
	
	void OpenGLExtensionHandlerES2::extGlUniform4iv(GLint location, GLsizei count, const GLint *value)
	{
		glUniform4iv(location, count, value);
	}
	
	void OpenGLExtensionHandlerES2::extGlUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	{
		glUniformMatrix2fv(location, count, transpose, value);
	}
	
	void OpenGLExtensionHandlerES2::extGlUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	{
		glUniformMatrix3fv(location, count, transpose, value);
	}
	
	void OpenGLExtensionHandlerES2::extGlUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	{
		glUniformMatrix4fv(location, count, transpose, value);
	}
	
	void OpenGLExtensionHandlerES2::extGlGetAttribLocation(GLuint program, const char* name, GLint* location)
	{
		*location = glGetAttribLocation(program, name);
	}
	
	void OpenGLExtensionHandlerES2::extGlVertexAttrib1fv(GLuint index, const GLfloat *v)
	{
		glVertexAttrib1fv(index, v);
	}
	
	void OpenGLExtensionHandlerES2::extGlVertexAttrib2fv(GLuint index, const GLfloat *v)
	{
		glVertexAttrib2fv(index, v);
	}
	
	void OpenGLExtensionHandlerES2::extGlVertexAttrib3fv(GLuint index, const GLfloat *v)
	{
		glVertexAttrib3fv(index, v);
	}
	
	void OpenGLExtensionHandlerES2::extGlVertexAttrib4fv(GLuint index, const GLfloat *v)
	{
		glVertexAttrib4fv(index, v);
	}
	
	void OpenGLExtensionHandlerES2::extGlEnableVertexAttribArray(GLuint index)
	{
		glEnableVertexAttribArray(index);
	}

	void OpenGLExtensionHandlerES2::extGlDisableVertexAttribArray(GLuint index)
	{
		glDisableVertexAttribArray(index);
	}
	
	void OpenGLExtensionHandlerES2::extGlVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
	{
		glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	}

	void OpenGLExtensionHandlerES2::extGlVertexAttribDivisor(GLuint index, GLuint divisor)
	{
#if SHOOT_PLATFORM != SHOOT_PLATFORM_ANDROID
		glVertexAttribDivisorEXT(index, divisor);
#endif
	}

	void OpenGLExtensionHandlerES2::extGLDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount)
	{
#if SHOOT_PLATFORM != SHOOT_PLATFORM_ANDROID
		glDrawArraysInstancedEXT(mode, first, count, primcount);
#endif
	}

	void OpenGLExtensionHandlerES2::extGLDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount)
	{
#if SHOOT_PLATFORM != SHOOT_PLATFORM_ANDROID
		glDrawElementsInstancedEXT(mode, count, type, indices, primcount);
#endif
	}

	void OpenGLExtensionHandlerES2::extGlGenerateMipmap(GLenum target)
	{
		GL_CHECK(glGenerateMipmap(target));
	}
}


