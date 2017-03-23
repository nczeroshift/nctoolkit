
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckWindow_macosx.h"
#include "../nckUtils.h"

#if defined(NCK_MACOSX)

#import "nckApplication_macosx.h"

void initAppDataFolder(void);
void releaseAppDataFolder(void);

std::map<Core::KeyboardButton,int> *p_KeyboardButtons = NULL;

std::string Application_Filename_MacOSX(){
    NSFileManager * fm = [[[NSFileManager alloc] init] autorelease];
    NSString * currentPath = [fm currentDirectoryPath];
    std::string ret = std::string([currentPath cStringUsingEncoding:NSUTF8StringEncoding]);
    return ret;
}

int64_t FileModified_MacOSX(const std::string & filename){
    NSDictionary * dic = [[NSFileManager defaultManager] attributesOfItemAtPath:[NSString stringWithUTF8String:filename.c_str()] error:nil];
    NSDate * d = [dic objectForKey:NSFileModificationDate];
    return [d timeIntervalSince1970] * 1e3;
}

@interface CustomNSWindow : NSWindow<NSWindowDelegate>
@property (nonatomic, readwrite) Core::Window_MacOSX * m_Wrapper;
@end

@implementation CustomNSWindow
@synthesize m_Wrapper;

- (void)keyDown:(NSEvent *)theEvent {
    unichar unicodeKey = [ [ theEvent characters ] characterAtIndex:0 ];
    MapSet(m_Wrapper->m_KeyStatus,int,Core::ButtonStatus,unicodeKey,Core::BUTTON_STATUS_DOWN);
}

- (void)keyUp:(NSEvent *)theEvent {
    unichar unicodeKey = [ [ theEvent characters ] characterAtIndex:0 ];
    MapSet(m_Wrapper->m_KeyStatus,int,Core::ButtonStatus,unicodeKey,Core::BUTTON_STATUS_UP);
}

-(void)mouseDown:(NSEvent *)event {
    Core::MouseButton btn = Core::BUTTON_MOUSE_LEFT;
    if(event.buttonNumber == 1)
        btn = Core::BUTTON_MOUSE_MIDDLE;
    if(event.buttonNumber == 2)
        btn = Core::BUTTON_MOUSE_RIGHT;
    MapSet(m_Wrapper->m_MouseBtnStatus,Core::MouseButton,Core::ButtonStatus,btn,Core::BUTTON_STATUS_DOWN);
}

-(void)mouseUp:(NSEvent *)event {
    Core::MouseButton btn = Core::BUTTON_MOUSE_LEFT;
    if(event.buttonNumber == 1)
        btn = Core::BUTTON_MOUSE_MIDDLE;
    if(event.buttonNumber == 2)
        btn = Core::BUTTON_MOUSE_RIGHT;
    MapSet(m_Wrapper->m_MouseBtnStatus,Core::MouseButton,Core::ButtonStatus,btn,Core::BUTTON_STATUS_UP);
}

-(void)windowWillClose:(NSNotification *)notification{
    m_Wrapper->m_IsClosed = true;
}

@end

void * AutoReleasePool_Create_MacOSX(){
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    return pool;
}

void AutoReleasePool_Destroy_MacOSX(void * pool){
    NSAutoreleasePool * tmp = (NSAutoreleasePool*)pool;
    [tmp release];
}


void * Mutex_Create_MacOSX(){
    NSLock * l = [[NSLock alloc] init];
    return l;
}
void Mutex_Destroy_MacOSX(void * lock){
    NSLock * l = (NSLock*)lock;
    [l release];
}

void Mutex_Lock_MacOSX(void * lock){
    NSLock * l = (NSLock*)lock;
    [l lock];
}
void Mutex_Unlock_MacOSX(void * lock){
    NSLock * l = (NSLock*)lock;
    [l unlock];
}

void MessageBox_MacOSX(const std::string & msg,const std::string & title){
    NSAlert* alert = [NSAlert alertWithMessageText:[NSString stringWithCString:title.c_str() encoding:NSUTF8StringEncoding]
                                     defaultButton:nil
                                   alternateButton:nil
                                       otherButton:nil
                         informativeTextWithFormat:@"%@",[NSString stringWithCString:msg.c_str() encoding:NSUTF8StringEncoding]];
    [alert runModal];  
    
}

int main(int argc, const char * argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	initAppDataFolder();
    
    p_KeyboardButtons = new std::map<Core::KeyboardButton,int>();
    p_KeyboardButtons->insert(std::pair<Core::KeyboardButton,int>(Core::BUTTON_ESCAPE,0x1b));
    p_KeyboardButtons->insert(std::pair<Core::KeyboardButton,int>(Core::BUTTON_LEFT,0xf702));
    p_KeyboardButtons->insert(std::pair<Core::KeyboardButton,int>(Core::BUTTON_RIGHT,0xf703));
    p_KeyboardButtons->insert(std::pair<Core::KeyboardButton,int>(Core::BUTTON_UP,0xf700));
    p_KeyboardButtons->insert(std::pair<Core::KeyboardButton,int>(Core::BUTTON_DOWN,0xf701));
    p_KeyboardButtons->insert(std::pair<Core::KeyboardButton,int>(Core::BUTTON_Z,0x7a));
    p_KeyboardButtons->insert(std::pair<Core::KeyboardButton,int>(Core::BUTTON_X,0x78));
    p_KeyboardButtons->insert(std::pair<Core::KeyboardButton,int>(Core::BUTTON_C,0x63));
    p_KeyboardButtons->insert(std::pair<Core::KeyboardButton,int>(Core::BUTTON_V,0x76));
    p_KeyboardButtons->insert(std::pair<Core::KeyboardButton,int>(Core::BUTTON_SPACE,0x20));
    p_KeyboardButtons->insert(std::pair<Core::KeyboardButton,int>(Core::BUTTON_ENTER,0xd));
    
    
    NSMutableArray * args = [[NSMutableArray alloc] initWithCapacity:argc];
    for(int i = 0;i<argc;i++)
        [args addObject:[NSString stringWithUTF8String:argv[i]]];
    
    [NCKApplication appMain:args];
    
    [args release];
    
    releaseAppDataFolder();
    SafeDelete(p_KeyboardButtons);
    [pool release];
	return 0;
}

_CORE_BEGIN

Window_MacOSX::Window_MacOSX(){
    m_Private = NULL;
    m_IsClosed = FALSE;
}

Window_MacOSX::~Window_MacOSX()
{
    if(m_Private)
        [(NSWindow*)m_Private release];
}

unsigned int Window_MacOSX::GetWidth(){
    CustomNSWindow * wnd =((CustomNSWindow*)m_Private);
    return ((NSView*)wnd.contentView).bounds.size.width;
}

unsigned int Window_MacOSX::GetHeight(){
    CustomNSWindow * wnd =((CustomNSWindow*)m_Private);
    return ((NSView*)wnd.contentView).bounds.size.height;
}

void Window_MacOSX::SetWidth(unsigned int Width){
    NSWindow * wnd = ((NSWindow*)m_Private);
    CGRect tmp = wnd.frame;
    tmp.size.width = Width;
    [wnd setFrame:tmp display:TRUE];
}

void Window_MacOSX::SetHeight(unsigned int Height){
    NSWindow * wnd = ((NSWindow*)m_Private);
    CGRect tmp = wnd.frame;
    tmp.size.width = Height;
    [wnd setFrame:tmp display:TRUE];
}

void Window_MacOSX::SetTitle(const std::string & title){
    NSWindow * wnd = ((NSWindow*)m_Private);
	wnd.title = [NSString stringWithUTF8String:title.c_str()];
}

std::string Window_MacOSX::GetTitle(){
    NSWindow * wnd = ((NSWindow*)m_Private);
    return std::string([wnd.title UTF8String]);
}

bool Window_MacOSX::Peek(bool peekAndWait){
    [[NCKApplication singleton] processEvents];
	return !m_IsClosed;
}

Point Window_MacOSX::GetMousePosition(){
    CustomNSWindow * wnd = (CustomNSWindow*)m_Private;
    NSPoint pt = [wnd convertScreenToBase:[NSEvent mouseLocation]];
    m_MousePosition = Core::Point(pt.x,((NSView*)wnd.contentView).bounds.size.height-pt.y);
    return m_MousePosition;
}

WindowStatus Window_MacOSX::GetStatus(){
    NSWindow * wnd = ((NSWindow*)m_Private);
    if([wnd isMainWindow])
        return WINDOW_ACTIVE;
    return WINDOW_IDLE;
}

ButtonStatus Window_MacOSX::GetMouseButtonStatus(MouseButton btn){
    std::map<MouseButton,ButtonStatus>::iterator entry = m_MouseBtnStatus.find(btn);
	if(entry == m_MouseBtnStatus.end())
		return BUTTON_STATUS_UP;
	return entry->second;
}

ButtonStatus Window_MacOSX::GetKeyStatus(KeyboardButton btn){
    std::map<KeyboardButton,int>::iterator entry = (*p_KeyboardButtons).find(btn);
	
	if(entry == (*p_KeyboardButtons).end())
		return BUTTON_STATUS_UP;
    
	std::map<int,Core::ButtonStatus>::iterator entry2 = m_KeyStatus.find(entry->second);
    
	if(entry2 == m_KeyStatus.end())
		return BUTTON_STATUS_UP;
    
	return entry2->second;
}

ButtonStatus Window_MacOSX::GetKeyCodeStatus(int code){
    if(m_KeyStatus.find(code)==m_KeyStatus.end())
		return BUTTON_STATUS_UP;
	return m_KeyStatus.find(code)->second;
}

void Window_MacOSX::SetCursorVisiblity(bool visiblity){
	if(!visiblity)
        [NSCursor hide];
    else
        [NSCursor unhide];
}

float Window::GetDisplayDensity() {
	return 1.0;
}

Window_MacOSX *CreateWindow_MacOSX(const std::string & Title, unsigned int Width, unsigned int Height, bool FullScreen)
{
    Window_MacOSX * ret = new Window_MacOSX();
    
    CGRect rect = CGRectMake(0,0,Width,Height);
    CustomNSWindow * wnd = [[CustomNSWindow alloc] initWithContentRect:rect
                                                    styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
                                                      backing:NSBackingStoreBuffered
                                                        defer:NO];
    wnd.m_Wrapper = ret;
    [wnd setReleasedWhenClosed:FALSE];
    [wnd setAcceptsMouseMovedEvents:TRUE];
    wnd.delegate = wnd;
    wnd.title = [NSString stringWithUTF8String:Title.c_str()];
    [wnd setBackgroundColor:[NSColor blackColor]];

    ((NSView*)wnd.contentView).frame = rect;
    [wnd center];
    [wnd makeKeyAndOrderFront:NSApp];
    
    ret->m_Private = wnd;
	return ret;
}

_CORE_END

#endif // #if defined(NCK_MACOSX)