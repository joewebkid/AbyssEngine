// GLES1 fixed-function compatibility stubs.
//
// The engine renders through the GLES2 programmable pipeline (GLES2/gl2.h), but the
// render code still contains calls to the legacy GLES1 fixed-function entry points
// (glMatrixMode/glLoadMatrixf/glColor4f/...). The original binary defines each of
// these as an empty no-op (e.g. `void glColor4f(void) { return; }`) and does not
// link libGLESv1_CM at all -- the legacy calls are inert. These definitions
// reproduce that: they satisfy the call sites and emit the same symbols the
// original exports, while doing nothing (the actual rendering happens via GLES2).
#include <GLES/gl.h>

extern "C" {

void glMatrixMode(GLenum) {}
void glLoadMatrixf(const GLfloat *) {}
void glLoadIdentity(void) {}
void glScalef(GLfloat, GLfloat, GLfloat) {}
void glColor4f(GLfloat, GLfloat, GLfloat, GLfloat) {}
void glMaterialf(GLenum, GLenum, GLfloat) {}
void glMaterialfv(GLenum, GLenum, const GLfloat *) {}
void glLightfv(GLenum, GLenum, const GLfloat *) {}
void glLightModelfv(GLenum, const GLfloat *) {}
void glTexEnvf(GLenum, GLenum, GLfloat) {}
void glVertexPointer(GLint, GLenum, GLsizei, const GLvoid *) {}
void glTexCoordPointer(GLint, GLenum, GLsizei, const GLvoid *) {}
void glNormalPointer(GLenum, GLsizei, const GLvoid *) {}
void glColorPointer(GLint, GLenum, GLsizei, const GLvoid *) {}
void glEnableClientState(GLenum) {}
void glDisableClientState(GLenum) {}
void glAlphaFunc(GLenum, GLclampf) {}
void glClientActiveTexture(GLenum) {}
void glFogf(GLenum, GLfloat) {}
void glFogfv(GLenum, const GLfloat *) {}
void glHint(GLenum, GLenum) {}
void glMultMatrixf(const GLfloat *) {}
void glShadeModel(GLenum) {}
void glTexEnvi(GLenum, GLenum, GLint) {}

}
