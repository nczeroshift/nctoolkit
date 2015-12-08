
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

// https://developer.apple.com/library/mac/qa/qa1385/_index.html

#import "nckGLView_macosx.h"

#if defined(NCK_MACOSX)

@interface NCKOpenGLView (PrivateMethods)
- (void) initGL;
@end

@implementation NCKOpenGLView
@synthesize isReady = _isReady;
@synthesize isLocked = _isLocked;

-(id)initWithDepthBits:(int)depthBits{
    self = [super init];
    if(self){
        _isReady = FALSE;
        NSOpenGLPixelFormatAttribute attrs[] =
        {
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFADepthSize, depthBits,
            0
        };
        
        NSOpenGLPixelFormat *pf = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attrs] autorelease];
        
        if (!pf)
        {
            NSLog(@"No OpenGL pixel format");
        }
        
        NSOpenGLContext* context = [[[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil] autorelease];
        
        [self setPixelFormat:pf];
        [self setOpenGLContext:context];
        
        [[self openGLContext] makeCurrentContext];
    }
    return self;
}

- (void) prepareOpenGL
{
	[super prepareOpenGL];
	[self initGL];
    _isReady = TRUE;
}

- (void) initGL
{
	[[self openGLContext] makeCurrentContext];
	GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
}

- (void)renewGState
{	
	[[self window] disableScreenUpdatesUntilFlush];
	[super renewGState];
}

-(void)bindToContext{
    [[self openGLContext] makeCurrentContext];
}

-(void)lock{
    if(self.isLocked)
        return;
    
    [[self openGLContext] makeCurrentContext];
	CGLLockContext([[self openGLContext] CGLContextObj]);
    _isLocked = TRUE;
}

-(void)unlock{
    if(!_isLocked)
        return;
    
    CGLFlushDrawable([[self openGLContext] CGLContextObj]);
	CGLUnlockContext([[self openGLContext] CGLContextObj]);
    
    _isLocked = FALSE;
}

- (void) dealloc
{
	[super dealloc];
}

@end

#endif // #if defined(NCK_MACOSX)

