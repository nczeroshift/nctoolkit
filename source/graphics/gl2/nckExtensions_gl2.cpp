
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckExtensions_gl2.h"
#include "nckUtils.h"
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(NCK_WINDOWS) | defined(NCK_LINUX)

static bool areOpenGL2ExtensionsReady = false;

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
PFNGLDELETEBUFFERSPROC 	glDeleteBuffers;
PFNGLCREATEPROGRAMPROC	glCreateProgram;
PFNGLCREATESHADERPROC 	glCreateShader;
PFNGLSHADERSOURCEPROC 	glShaderSource;
PFNGLCOMPILESHADERPROC 	glCompileShader;
PFNGLLINKPROGRAMPROC 	glLinkProgram;
PFNGLDETACHSHADERPROC 	glDetachShader;
PFNGLDELETESHADERPROC 	glDeleteShader;
PFNGLGETPROGRAMIVPROC 	glGetProgramiv;
PFNGLDELETEPROGRAMPROC	glDeleteProgram;
PFNGLUSEPROGRAMPROC		glUseProgram;
PFNGLGETSHADERIVPROC 	glGetShaderiv;
PFNGLATTACHSHADERPROC	glAttachShader;

PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM2IPROC glUniform2i;
PFNGLUNIFORM3IPROC glUniform3i;
PFNGLUNIFORM4IPROC glUniform4i;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORM1FVPROC glUniform1fv;
PFNGLUNIFORM4IVPROC glUniform4iv;
PFNGLUNIFORM1IVPROC glUniform1iv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;

#if defined(NCK_WINDOWS)
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalExt;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture;
PFNGLTEXIMAGE3DPROC glTexImage3D;
PFNGLACTIVETEXTUREPROC glActiveTexture;
#endif

#if defined(NCK_LINUX)
PFNGLXSWAPINTERVALMESAPROC glXSwapInterval;
#endif

void nckInitExtensionsGL2()
{
	if(areOpenGL2ExtensionsReady)
		return;

	std::map<std::string,void*> extensions;

#define INSERT_EXTENSION(name,pointer) extensions.insert(std::pair<std::string,void*>(name,pointer))

#if defined(NCK_WINDOWS)
	INSERT_EXTENSION("glClientActiveTexture",&glClientActiveTexture);
	INSERT_EXTENSION("glTexImage3D",&glTexImage3D);
	INSERT_EXTENSION("glActiveTexture",&glActiveTexture);
#endif

	INSERT_EXTENSION("glGenBuffers",&glGenBuffers);
	INSERT_EXTENSION("glBindBuffer",&glBindBuffer);
	INSERT_EXTENSION("glBufferData",&glBufferData);
	INSERT_EXTENSION("glGetBufferParameteriv",&glGetBufferParameteriv);
	INSERT_EXTENSION("glDeleteBuffers",&glDeleteBuffers);
	INSERT_EXTENSION("glCreateProgram",&glCreateProgram);
	INSERT_EXTENSION("glCreateShader",&glCreateShader);
	INSERT_EXTENSION("glShaderSource",&glShaderSource);
	INSERT_EXTENSION("glCompileShader",&glCompileShader);
	INSERT_EXTENSION("glLinkProgram",&glLinkProgram);
	INSERT_EXTENSION("glDetachShader",&glDetachShader);
	INSERT_EXTENSION("glDeleteShader",&glDeleteShader);
	INSERT_EXTENSION("glGetProgramiv",&glGetProgramiv);
	INSERT_EXTENSION("glDeleteProgram",&glDeleteProgram);
	INSERT_EXTENSION("glUseProgram",&glUseProgram);
	INSERT_EXTENSION("glGetShaderiv",&glGetShaderiv);
	INSERT_EXTENSION("glAttachShader",&glAttachShader);

	INSERT_EXTENSION("glUniform1f",&glUniform1f);
	INSERT_EXTENSION("glUniform2f",&glUniform2f);
	INSERT_EXTENSION("glUniform3f",&glUniform3f);
	INSERT_EXTENSION("glUniform4f",&glUniform4f);
	INSERT_EXTENSION("glUniform1i",&glUniform1i);
	INSERT_EXTENSION("glUniform2i",&glUniform2i);
	INSERT_EXTENSION("glUniform3i",&glUniform3i);
	INSERT_EXTENSION("glUniform4i",&glUniform4i);
	INSERT_EXTENSION("glUniformMatrix4fv",&glUniformMatrix4fv);
	INSERT_EXTENSION("glGetUniformLocation",&glGetUniformLocation);
	INSERT_EXTENSION("glUniform4fv",&glUniform4fv);
	INSERT_EXTENSION("glUniform1fv",&glUniform1fv);
	INSERT_EXTENSION("glUniform4iv",&glUniform4iv);
	INSERT_EXTENSION("glUniform1iv",&glUniform1iv);
	INSERT_EXTENSION("glGetShaderInfoLog",&glGetShaderInfoLog);
	INSERT_EXTENSION("glGetProgramInfoLog",&glGetProgramInfoLog);

	INSERT_EXTENSION("glGenFramebuffers",&glGenFramebuffers);
	INSERT_EXTENSION("glGenRenderbuffers",&glGenRenderbuffers);
	INSERT_EXTENSION("glBindRenderbuffer",&glBindRenderbuffer);
	INSERT_EXTENSION("glRenderbufferStorage",&glRenderbufferStorage);
	INSERT_EXTENSION("glCheckFramebufferStatus",&glCheckFramebufferStatus);
	INSERT_EXTENSION("glBindFramebuffer",&glBindFramebuffer);
	INSERT_EXTENSION("glFramebufferRenderbuffer",&glFramebufferRenderbuffer);
	INSERT_EXTENSION("glFramebufferTexture2D",&glFramebufferTexture2D);
	INSERT_EXTENSION("glDeleteFramebuffers",&glDeleteFramebuffers);
	INSERT_EXTENSION("glDeleteRenderbuffers",&glDeleteRenderbuffers);

#if defined(NCK_LINUX)
	INSERT_EXTENSION("glXSwapIntervalSGI",&glXSwapInterval);
#endif
	
#if defined(NCK_WINDOWS)
	INSERT_EXTENSION("wglSwapIntervalEXT",&wglSwapIntervalExt);
#endif

	for(std::map<std::string,void*>::iterator i = extensions.begin();i!=extensions.end();i++)
	{
#if defined(NCK_LINUX)
		PFNGLXGETPROCADDRESSPROC value = (PFNGLXGETPROCADDRESSPROC)glXGetProcAddress((const GLubyte*)(*i).first.c_str());
		*((PFNGLXGETPROCADDRESSPROC*)(*i).second) = value;
#else
		PROC value = wglGetProcAddress((*i).first.c_str());
		*((PROC*)(*i).second) = value;
#endif
		if(!value)
			printf("Extension \"%s\" load failure\n",(*i).first.c_str());
	}

	areOpenGL2ExtensionsReady = true;
}

bool nckGetGLError(std::string * text){
	if(!text)
		return false;
	GLuint error = glGetError();
	bool hasError = false;
	while(error!=GL_NO_ERROR)
	{
		(*text)+= std::string((char*)gluErrorString(error));
		error = glGetError();
		hasError = true;
	}

	return hasError;
}

#endif // #if defined(NCK_WINDOWS) | defined(NCK_LINUX)

