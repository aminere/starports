/* 

Amine Rehioui
Created: August 15th 2010

Uses code from the Irrlicht engine extension handler

*/

#pragma once


#include "GraphicExtensionHandler.h"
#include "OpenGL.h"

namespace shoot
{
	//! base OpenGL Extension handler class
	class OpenGLExtensionHandler : public GraphicExtensionHandler
	{
	public:

		//! returns the extension handler
		inline static OpenGLExtensionHandler* Instance() { return static_cast<OpenGLExtensionHandler*>(m_spInstance); }

		// extensions
		virtual void extGlGenFramebuffers(GLsizei n, GLuint *framebuffers) = 0;
		virtual void extGlBindFramebuffer(GLenum target, GLuint framebuffer) = 0;
		virtual void extGlFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) = 0;
		virtual void extGlDeleteFramebuffers(GLsizei n, const GLuint *framebuffers) = 0;
		virtual GLenum extGlCheckFramebufferStatus(GLenum target) = 0;
		virtual void extGlBindRenderbuffer(GLenum target, GLuint renderbuffer) = 0;
		virtual void extGlDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) = 0;
		virtual void extGlGenRenderbuffers(GLsizei n, GLuint *renderbuffers) = 0;
		virtual void extGlRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) = 0;
		virtual void extGlFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) = 0;
		virtual void extGlActiveTexture(GLenum texture) = 0;
		virtual void extGlBindBuffer(GLenum target, GLuint buffer) = 0;
		virtual void extGlDeleteBuffers(GLsizei n, const GLuint *buffers) = 0;
		virtual void extGlGenBuffers(GLsizei n, GLuint *buffers) = 0;
		virtual void extGlBufferData(GLenum target, GLsizei size, const void* data, GLenum usage) = 0;
		virtual void* extGlMapBuffer(GLenum target, GLenum access) = 0;
		virtual GLboolean extGlUnmapBuffer(GLenum target) = 0;
		virtual void extGlGenVertexArrays(GLsizei n, GLuint *arrays) = 0;
		virtual void extGlBindVertexArray(GLuint array) = 0;
		virtual void extGlDeleteVertexArrays(GLsizei n, const GLuint *arrays) = 0;
		virtual void extGlCreateShader(E_ShaderType type, GLuint *shader) = 0;
		virtual void extGlDeleteShader(GLuint shader) = 0;
		virtual void extGlShaderSource(GLuint shader, GLsizei count, const char* *string, const GLint *length) = 0;
		virtual void extGlCompileShader(GLuint shader) = 0;
		virtual void extGlCreateProgram(GLuint* program) = 0;
		virtual void extGlDeleteProgram(GLuint program) = 0;
		virtual void extGlAttachShader(GLuint program, GLuint shader) = 0;
		virtual void extGlDetachShader(GLuint program, GLuint shader) = 0;
		virtual void extGlLinkProgram(GLuint program) = 0;
		virtual void extGlUseProgram(GLuint program) = 0;
		virtual bool extGlCheckCompileStatus(GLuint shader, const std::string& path) = 0;
		virtual bool extGlCheckLinkStatus(GLuint program, const std::string& path) = 0;
		virtual void extGlGetUniformLocation(GLuint program, const char* name, GLint* location) = 0;
		virtual void extGlUniform1fv(GLint location, GLsizei count, const GLfloat *value) = 0;
		virtual void extGlUniform2fv(GLint location, GLsizei count, const GLfloat *value) = 0;
		virtual void extGlUniform3fv(GLint location, GLsizei count, const GLfloat *value) = 0;
		virtual void extGlUniform4fv(GLint location, GLsizei count, const GLfloat *value) = 0;
		virtual void extGlUniform1iv(GLint location, GLsizei count, const GLint *value) = 0;
		virtual void extGlUniform2iv(GLint location, GLsizei count, const GLint *value) = 0;
		virtual void extGlUniform3iv(GLint location, GLsizei count, const GLint *value) = 0;
		virtual void extGlUniform4iv(GLint location, GLsizei count, const GLint *value) = 0;
		virtual void extGlUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) = 0;
		virtual void extGlUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) = 0;
		virtual void extGlUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) = 0;
		virtual void extGlGetAttribLocation(GLuint program, const char* name, GLint* location) = 0;
		virtual void extGlVertexAttrib1fv(GLuint index, const GLfloat *v) = 0;
		virtual void extGlVertexAttrib2fv(GLuint index, const GLfloat *v) = 0;
		virtual void extGlVertexAttrib3fv(GLuint index, const GLfloat *v) = 0;
		virtual void extGlVertexAttrib4fv(GLuint index, const GLfloat *v) = 0;
		virtual void extGlEnableVertexAttribArray(GLuint index) = 0;
		virtual void extGlDisableVertexAttribArray(GLuint index) = 0;
		virtual void extGlVertexAttribDivisor(GLuint index, GLuint divisor) = 0;
		virtual void extGlVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) = 0;
		virtual void extGLDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount) = 0;
		virtual void extGLDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount) = 0;
		virtual void extGlGenerateMipmap(GLenum target) = 0;

	protected:

		//! Constructor
		OpenGLExtensionHandler(const char* const strExtensionStrings[E_Count]);
	};
}



