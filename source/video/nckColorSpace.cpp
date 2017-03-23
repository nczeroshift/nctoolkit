
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckColorSpace.h"
#include <math.h>

#define MAX(x, y ,z) (x>y ? (x>z?x:z) : (y>z?y:z))
#define MIN(x, y ,z) (x>y ? (y>z?z:y) : (x>z?z:x))

_VIDEO_BEGIN

// Found somewhere in the internet, kudus to the person who did this!
void ColorSpace::YUVToRGB(int y, int u, int v, char *r, char *g, char *b)
{
	int r1, g1, b1;
	int c = y-16, d = u - 128, e = v - 128;

	r1 = (298 * c           + 409 * e + 128) >> 8;
	g1 = (298 * c - 100 * d - 208 * e + 128) >> 8;
	b1 = (298 * c + 516 * d           + 128) >> 8;

	if (r1 > 255) r1 = 255;
	if (g1 > 255) g1 = 255;
	if (b1 > 255) b1 = 255;
	if (r1 < 0) r1 = 0;
	if (g1 < 0) g1 = 0;
	if (b1 < 0) b1 = 0;

	*r = r1 ;
	*g = g1 ;
	*b = b1 ;
}

// http://www.cs.rit.edu/~ncs/color/t_convert.html
void ColorSpace::RGBToHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min, max, delta;

	min = MIN( r, g, b );
	max = MAX( r, g, b );
	*v = max;

	delta = max - min;

	if( max != 0 )
		*s = delta / max;
	else {
		*s = 0;
		*h = -1;
		return;
	}

	if( r == max )
		*h = ( g - b ) / delta;
	else if( g == max )
		*h = 2 + ( b - r ) / delta;
	else
		*h = 4 + ( r - g ) / delta;

	*h *= 60;
	if( *h < 0 )
		*h += 360;
}

//http://www.cs.rit.edu/~ncs/color/t_convert.html
void ColorSpace::HSVToRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		*r = *g = *b = v;
		return;
	}

	h /= 60;
	i = floor( h );
	f = h - i;
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:
			*r = v;
			*g = p;
			*b = q;
			break;
	}

}

void ColorSpace::ConvertImageYUV422ToRGB24(const void * yuvData,
		size_t yuvSize, int width, int height, void* rgbData)
{
	unsigned int *yuyv = (unsigned int*)yuvData;
	unsigned char * rgb = (unsigned char*)rgbData;
	for (int i=0,px=0; i< height*width/2;i++)
	{
		char r, g, b;

		// read VYUY
		int y2 = ((*yuyv & 0x000000ff));
		int u = ((*yuyv & 0x0000ff00)>>8);
		int y = ((*yuyv & 0x00ff0000)>>16);
		int v = ((*yuyv & 0xff000000)>>24);

		YUVToRGB(y2, u, v, &r, &g, &b);

		rgb[px++] = r;
		rgb[px++] = g;
		rgb[px++] = b;

		YUVToRGB(y, u, v, &r, &g, &b);

		rgb[px++] = r;
		rgb[px++] = g;
		rgb[px++] = b;

		yuyv++;
	}
}

_VIDEO_END
