
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

/*
 * Display link feature from MacOSX/iOS will be supported in the future, currently NCK
 * uses swapinterval to keep everything synced.
 */

#ifndef NCK_OPENGL_VIEW_H
#define NCK_OPENGL_VIEW_H

#include "../../nckGraphicsConfig.h"

#if defined(NCK_MACOSX)

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>

#import <OpenGL/gl.h>
#import <OpenGL/OpenGL.h>

@interface NCKOpenGLView : NSOpenGLView {
}
@property(nonatomic,readonly) BOOL isReady;
@property(nonatomic,readonly) BOOL isLocked;
-(id)initWithDepthBits:(int)depthBits;
-(void)lock;
-(void)unlock;
-(void)bindToContext;
@end

#endif // #if defined(NCK_MACOSX)

#endif // #ifndef NCK_OPENGL_VIEW_H
