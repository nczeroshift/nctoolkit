
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics.h"

#if defined NCK_DIRECTX9
    #include "dx9/nckGraphics_dx9.h"
#endif

#if defined NCK_OPENGL2
    #include "gl2/nckGraphics_gl2.h"
#endif

_GRAPHICS_BEGIN

Device* CreateDevice(Core::Window *wnd,DeviceType type,
								unsigned int width, unsigned int height,
								unsigned int antialiasing,
								bool fullscreen)
{

#if defined NCK_DIRECTX9
	if(type == DEVICE_AUTO || type == DEVICE_DIRECTX)
		return (Device*)CreateDevice_DX9(wnd,width,height,antialiasing,fullscreen);
#endif

#if defined NCK_OPENGL2
	if(type == DEVICE_AUTO || type == DEVICE_OPENGL)
		return (Device*)CreateDevice_GL2(wnd,width,height,antialiasing,fullscreen);
#endif

	return NULL;
}

_GRAPHICS_END
