
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef _NCK_OPENGL_BIND_MACOSX_H_
#define _NCK_OPENGL_BIND_MACOSX_H_

#include "nckWindow.h"

#if defined(NCK_MACOSX)

void GLCreate(Core::Window * wnd, int depthBits);
void GLDestroy(Core::Window * wnd);

void GLLock(Core::Window * wnd);
void GLUnlock(Core::Window * wnd);

#endif // #if defined(NCK_MACOSX)

#endif // #ifndef _NCK_OPENGL_BIND_MACOSX_H_
