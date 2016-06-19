/* 

Amine Rehioui
Created: February 8th 2014

*/

#pragma once

#include "OpenGLExtensionHandler.h"

namespace shoot
{
	//! OpenGLExtensionHandlerES2
	class OpenGLExtensionHandlerES2 : public OpenGLExtensionHandler
	{
		typedef OpenGLExtensionHandler super;

	public:

		//! constructor
		OpenGLExtensionHandlerES2();

		// extensions
		void extGlGenFramebuffers(GLsizei n, GLuint *framebuffers);
		void extGlBindFramebuffer(GLenum target, GLuint framebuffer);
		void extGlFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
		void extGlDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);
		GLenum extGlCheckFramebufferStatus(GLenum target);
		void extGlBindRenderbuffer(GLenum target, GLuint renderbuffer);
		void extGlDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
		void extGlGenRenderbuffers(GLsizei n, GLuint *renderbuffers);
		void extGlRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
		void extGlFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
		void extGlActiveTexture(GLenum texture);
		void extGlBindBuffer(GLenum target, GLuint buffer);
		void extGlDeleteBuffers(GLsizei n, const GLuint *buffers);
		void extGlGenBuffers(GLsizei n, GLuint *buffers);
		void extGlBufferData(GLenum target, GLsizei size, const void* data, GLenum usage);
        void* extGlMapBuffer(GLenum target, GLenum access) { return 0; }
        GLboolean extGlUnmapBuffer(GLenum target) { return false; }
		void extGlGenVertexArrays(GLsizei n, GLuint *arrays);
		void extGlBindVertexArray(GLuint array);
		void extGlDeleteVertexArrays(GLsizei n, const GLuint *arrays);
		void extGlCreateShader(E_ShaderType type, GLuint *shader);
		void extGlDeleteShader(GLuint shader);
		void extGlShaderSource(GLuint shader, GLsizei count, const char* *string, const GLint *length);
		void extGlCompileShader(GLuint shader);
		void extGlCreateProgram(GLuint* program);
		void extGlDeleteProgram(GLuint program);
		void extGlAttachShader(GLuint program, GLuint shader);
		void extGlDetachShader(GLuint program, GLuint shader);
		void extGlLinkProgram(GLuint program);
		void extGlUseProgram(GLuint program);
		bool extGlCheckCompileStatus(GLuint shader, const std::string& path);
		bool extGlCheckLinkStatus(GLuint program, const std::string& path);
		void extGlGetUniformLocation(GLuint program, const char* name, GLint* location);
		void extGlUniform1fv(GLint location, GLsizei count, const GLfloat *value);
		void extGlUniform2fv(GLint location, GLsizei count, const GLfloat *value);
		void extGlUniform3fv(GLint location, GLsizei count, const GLfloat *value);
		void extGlUniform4fv(GLint location, GLsizei count, const GLfloat *value);
		void extGlUniform1iv(GLint location, GLsizei count, const GLint *value);
		void extGlUniform2iv(GLint location, GLsizei count, const GLint *value);
		void extGlUniform3iv(GLint location, GLsizei count, const GLint *value);
		void extGlUniform4iv(GLint location, GLsizei count, const GLint *value);
		void extGlUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
		void extGlUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
		void extGlUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
		void extGlGetAttribLocation(GLuint program, const char* name, GLint* location);
		void extGlVertexAttrib1fv(GLuint index, const GLfloat *v);
		void extGlVertexAttrib2fv(GLuint index, const GLfloat *v);
		void extGlVertexAttrib3fv(GLuint index, const GLfloat *v);
		void extGlVertexAttrib4fv(GLuint index, const GLfloat *v);
		void extGlEnableVertexAttribArray(GLuint index);
		void extGlDisableVertexAttribArray(GLuint index);
		void extGlVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
		void extGlVertexAttribDivisor(GLuint index, GLuint divisor);
		void extGLDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
		void extGLDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount);
		void extGlGenerateMipmap(GLenum target);

	private:

		static const char* const ms_strExtensionStrings[E_Count];
	};
}

