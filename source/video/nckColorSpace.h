
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_COLOR_SPACE_H
#define NCK_COLOR_SPACE_H

#include "nckVideoConfig.h"
#include <stdlib.h>

_VIDEO_BEGIN

/**
* Class for color conversion utility methods.
*/ 
class ColorSpace{
public:
	static void YUVToRGB(int y, int u, int v, char *r, char *g, char *b);
	static void RGBToHSV( float r, float g, float b, float *h, float *s, float *v);
	static void HSVToRGB( float *r, float *g, float *b, float h, float s, float v );

	static void ConvertImageYUV422ToRGB24(const void * yuvData,
			size_t yuvSize, int width, int height, void * rgbData);
};

_VIDEO_END

#endif // NCK_COLOR_SPACE_H
