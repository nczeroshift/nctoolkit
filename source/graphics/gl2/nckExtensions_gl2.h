
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_EXTENSIONS_GL2_H_
#define NCK_EXTENSIONS_GL2_H_

#include "../nckGraphicsConfig.h"

#if defined(NCK_MACOSX)
    #import <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <OpenGL/glext.h>
#else
    #if defined(NCK_WINDOWS)
        #include <Windows.h>
    #endif

    #include <GL/gl.h>
    #include <GL/glext.h>
    #include <GL/glu.h>

    #if defined(NCK_WINDOWS)
        #include <GL/wglext.h>
    #elif defined(NCK_LINUX)
        #include <GL/glx.h>
    #endif
#endif

#include <string>

#if defined(NCK_LINUX) | defined(NCK_WINDOWS)

void nckInitExtensionsGL2();
bool nckGetGLError(std::string * text);

extern PFNGLGENBUFFERSPROC      glGenBuffers;
extern PFNGLBINDBUFFERPROC      glBindBuffer;
extern PFNGLBUFFERDATAPROC      glBufferData;
extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
extern PFNGLDELETEBUFFERSPROC	glDeleteBuffers;
extern PFNGLCREATEPROGRAMPROC	glCreateProgram;
extern PFNGLCREATESHADERPROC	glCreateShader;
extern PFNGLATTACHSHADERPROC	glAttachShader;
extern PFNGLDETACHSHADERPROC	glDetachShader;
extern PFNGLLINKPROGRAMPROC		glLinkProgram;
extern PFNGLDELETESHADERPROC	glDeleteShader;
extern PFNGLDELETEPROGRAMPROC	glDeleteProgram;
extern PFNGLSHADERSOURCEPROC	glShaderSource;
extern PFNGLGETSHADERIVPROC 	glGetShaderiv;
extern PFNGLGETPROGRAMIVPROC 	glGetProgramiv;
extern PFNGLUSEPROGRAMPROC		glUseProgram;
extern PFNGLCOMPILESHADERPROC 	glCompileShader;

extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLUNIFORM3FPROC glUniform3f;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM2IPROC glUniform2i;
extern PFNGLUNIFORM3IPROC glUniform3i;
extern PFNGLUNIFORM4IPROC glUniform4i;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLUNIFORM1FVPROC glUniform1fv;
extern PFNGLUNIFORM4IVPROC glUniform4iv;
extern PFNGLUNIFORM1IVPROC glUniform1iv;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
extern PFNGLDRAWBUFFERSPROC glDrawBuffers;

#if defined (NCK_LINUX)
extern PFNGLXSWAPINTERVALMESAPROC glXSwapInterval;
#endif

#if defined(NCK_WINDOWS)
extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalExt;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture;
extern PFNGLTEXIMAGE3DPROC glTexImage3D;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
#endif

#endif // #if defined(NCK_LINUX) | defined(NCK_WINDOWS)

#endif // NCK_EXTENSIONS_GL2_H_
