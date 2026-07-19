/* Minimal OpenGL ES 1.x fixed-function header (vendored).
 * The game's render code (Engine/PaintCanvas) uses the GLES1 fixed-function
 * pipeline, which GLES2/gl2.h does not declare. This provides the subset of
 * GLES1 entry points the engine calls, with the standard Khronos signatures.
 * Both the 32-bit match build and the native build include this so the GL
 * functions are declared by a real library header rather than hand-written
 * extern "C" forward-decls. */
#ifndef GOF2_VENDORED_GLES_GL_H
#define GOF2_VENDORED_GLES_GL_H

#include <GLES2/gl2platform.h>

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef int GLfixed;

#ifdef __cplusplus
extern "C" {
#endif

void glMatrixMode(GLenum mode);
void glLoadMatrixf(const GLfloat *m);
void glLoadIdentity(void);
void glScalef(GLfloat x, GLfloat y, GLfloat z);
void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void glMaterialf(GLenum face, GLenum pname, GLfloat param);
void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params);
void glLightfv(GLenum light, GLenum pname, const GLfloat *params);
void glLightModelfv(GLenum pname, const GLfloat *params);
void glTexEnvf(GLenum target, GLenum pname, GLfloat param);
void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer);
void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glEnableClientState(GLenum array);
void glDisableClientState(GLenum array);
GL_APICALL void GL_APIENTRY glLineWidth(GLfloat width);
GL_APICALL void GL_APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count);
GL_APICALL void GL_APIENTRY glActiveTexture(GLenum texture);
GL_APICALL void GL_APIENTRY glBindTexture(GLenum target, GLuint texture);
GL_APICALL void GL_APIENTRY glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
GL_APICALL void GL_APIENTRY glFinish(void);
void glFogf(GLenum pname, GLfloat param);
void glFogfv(GLenum pname, const GLfloat *params);
GL_APICALL void GL_APIENTRY glEnable(GLenum cap);
GL_APICALL void GL_APIENTRY glDisable(GLenum cap);
GL_APICALL void GL_APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor);
void glTexEnvi(GLenum target, GLenum pname, GLint param);
GL_APICALL void GL_APIENTRY glDepthMask(GLboolean flag);
void glAlphaFunc(GLenum func, GLclampf ref);

#ifdef __cplusplus
}
#endif

#endif
