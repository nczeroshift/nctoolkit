
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef _NCK_CONFIG_H_
#define _NCK_CONFIG_H_

#if defined(_WIN32) || defined(_WIN64)
    #define NCK_WINDOWS
#elif defined(__linux__)
    #define NCK_LINUX
#elif defined(__APPLE__)
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR | TARGET_OS_IPHONE
        #define NCK_IOS
    #elif TARGET_OS_MAC
        #define NCK_MACOSX
    #endif
    #define NCK_APPLE
#endif

#define NCK_OPENGL2
#define NCK_PNG
#define NCK_JPG
#define NCK_TGA
#define NCK_BMP

#if defined(NCK_LINUX) | defined(NCK_WINDOWS)
    #define NCK_OGG
#endif

#if defined(NCK_LINUX)
    #define NCK_ALSA
    #define NCK_WEBCAM_V2L
#endif

#if defined(NCK_LINUX) | defined(NCK_MACOSX)
    #define NCK_SERIAL_PORT
#endif

#if defined(NCK_WINDOWS)
    //#define NCK_DIRECTX9
    #define NCK_WAVEOUT
#endif

#endif // #ifndef _NCK_CONFIG_H_
