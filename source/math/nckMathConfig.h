
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_MATHCONFIG_H
#define NCK_MATHCONFIG_H

#include "../nckConfig.h"

#if defined(NCK_WINDOWS)
	#define _USE_MATH_DEFINES
#endif

#include <math.h>

#define _MATH_BEGIN namespace Math{
#define _MATH_END }

#endif // #ifndef NCK_MATHCONFIG_H
