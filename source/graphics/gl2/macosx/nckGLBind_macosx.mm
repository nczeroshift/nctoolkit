
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#import "nckGLBind_macosx.h"
#import "nckGLView_macosx.h"

#if defined(NCK_MACOSX)

#import "macosx/nckWindow_macosx.h"

void GLCreate(Core::Window * wnd, int depthBits){
    Core::Window_MacOSX * macWnd = (Core::Window_MacOSX*)wnd;
    NSWindow * window = (NSWindow*)macWnd->m_Private;
    CGRect rect = ((NSView*)window.contentView).bounds;
    NCKOpenGLView * gl = [[[NCKOpenGLView alloc] initWithDepthBits:depthBits] autorelease];
    [window setContentView:gl];
    gl.frame = rect;
    if(![NSThread isMainThread]){
        // Some methods for OGL creation will be called in the main thread loop.
        while(!gl.isReady){
            usleep(1000);
        }
    }
    
    [gl bindToContext];
}

void GLLock(Core::Window * wnd){
    Core::Window_MacOSX * macWnd = (Core::Window_MacOSX*)wnd;
    NSWindow * window = (NSWindow*)macWnd->m_Private;
    NCKOpenGLView * gl = window.contentView;
    
    if(!gl.isReady)
        return;
    
    [gl lock];
}

void GLUnlock(Core::Window * wnd){
    Core::Window_MacOSX * macWnd = (Core::Window_MacOSX*)wnd;
    NSWindow * window = (NSWindow*)macWnd->m_Private;
    NCKOpenGLView * gl = window.contentView;
    
    if(!gl.isReady)
        return;
    
    [gl unlock];
}

void GLDestroy(Core::Window * wnd){
    Core::Window_MacOSX * macWnd = (Core::Window_MacOSX*)wnd;
    NSWindow * window = (NSWindow*)macWnd->m_Private;
    NCKOpenGLView * gl = window.contentView;
    
    if(!gl.isReady)
        return;
    
    [window setContentView:nil];
}

#endif // #if defined(NCK_MACOSX)