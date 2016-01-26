
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckWindow_linux.h"

#if defined(NCK_LINUX)

#include <list>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../nckUtils.h"

std::list<Core::Window_Linux*> *p_WindowManager = NULL;
std::map<Core::KeyboardButton,int> *p_KeyboardButtons = NULL;

void initAppDataFolder(void);
void releaseAppDataFolder(void);

_CORE_BEGIN

void p_CreateButtonMap()
{
	p_KeyboardButtons->insert(std::pair<KeyboardButton,int>(BUTTON_ESCAPE,9));
	p_KeyboardButtons->insert(std::pair<KeyboardButton,int>(BUTTON_ENTER,36));
	p_KeyboardButtons->insert(std::pair<KeyboardButton,int>(BUTTON_SPACE,65));
	p_KeyboardButtons->insert(std::pair<KeyboardButton,int>(BUTTON_UP,111));
	p_KeyboardButtons->insert(std::pair<KeyboardButton,int>(BUTTON_DOWN,116));
	p_KeyboardButtons->insert(std::pair<KeyboardButton,int>(BUTTON_LEFT,113));
	p_KeyboardButtons->insert(std::pair<KeyboardButton,int>(BUTTON_RIGHT,114));
	p_KeyboardButtons->insert(std::pair<KeyboardButton,int>(BUTTON_Z,'Z'));
	p_KeyboardButtons->insert(std::pair<KeyboardButton,int>(BUTTON_X,'X'));
	p_KeyboardButtons->insert(std::pair<KeyboardButton,int>(BUTTON_C,'C'));
	p_KeyboardButtons->insert(std::pair<KeyboardButton,int>(BUTTON_V,'V'));
}

Window_Linux::Window_Linux(){
	m_Width = 640;
	m_Height = 480;
	m_Buffered = false;
	m_Display = NULL;
	m_Window = 0;
	m_DeleteMessage = 0;
	m_Status = WINDOW_ACTIVE;
	m_Mutex = Core::CreateMutex();
}

Window_Linux::~Window_Linux()
{
	XDestroyWindow(m_Display,m_Window);
	XCloseDisplay(m_Display);
	(*p_WindowManager).remove(this);
	SafeDelete(m_Mutex);
}

unsigned int Window_Linux::GetWidth(){
	return m_Width;
}

unsigned int Window_Linux::GetHeight(){
	return m_Height;
}

std::string Window_Linux::GetTitle(){
    m_Mutex->Lock();
    std::string ret = m_Title;
    m_Mutex->Unlock();
	return ret;
}

void Window_Linux::SetWidth(unsigned int Width){
	m_Width = Width;
}

void Window_Linux::SetHeight(unsigned int Height){
	m_Height = Height;
}

void Window_Linux::SetTitle(const std::string & title ){
    m_Mutex->Lock();
	m_Title = title;
    m_Mutex->Unlock();
}

bool Window_Linux::processEvents(XEvent event, bool *running)
{
	switch(event.type)
	{
		case MotionNotify:
		{
			m_MousePosition = Point(event.xmotion.x,event.xmotion.y);
		}
		break;

		case ButtonPress:
		{
			int buttonID = event.xbutton.button;
			MouseButton btn = BUTTON_MOUSE_LEFT;
			if(buttonID == 2)
				btn = BUTTON_MOUSE_MIDDLE;
			else if(buttonID == 3)
				btn = BUTTON_MOUSE_RIGHT;	
			MapSet(m_MouseBtnStatus,MouseButton,ButtonStatus,btn,BUTTON_STATUS_DOWN);	
		}
		break;

		case ButtonRelease:
		{
			int buttonID = event.xbutton.button;
			MouseButton btn = BUTTON_MOUSE_LEFT;
			if(buttonID == 2)
				btn = BUTTON_MOUSE_MIDDLE;
			else if(buttonID == 3)
				btn = BUTTON_MOUSE_RIGHT;	
			MapSet(m_MouseBtnStatus,MouseButton,ButtonStatus,btn,BUTTON_STATUS_UP);
		}
		break;

		case KeyPress:
		{
			MapSet(m_KeyStatus,int,ButtonStatus,event.xkey.keycode,BUTTON_STATUS_DOWN);
		}
		break;

		case KeyRelease:
		{
			MapSet(m_KeyStatus,int,ButtonStatus,event.xkey.keycode,BUTTON_STATUS_UP);
		}
		break;

		case FocusOut:
		{
			m_Status = WINDOW_IDLE;
		}
		break;

		case FocusIn:
		{
			m_Status = WINDOW_ACTIVE;
		}
		break;

		case ConfigureNotify:
		{
			m_Width = event.xconfigure.width;
			m_Height = event.xconfigure.height;
		}
		break;

		case ClientMessage:
		{
			if((Atom)(event.xclient.data.l[0]) == m_DeleteMessage)
				*running = false;
		}
		break;

		default:
			return false;
	}

	return true;
}

bool Window_Linux::Peek(bool peekAndWait){
	XEvent event;
	bool running = true;

	m_Mutex->Lock();
	if(m_TmpWidth!=m_Width || m_TmpHeight != m_Height){
		m_TmpWidth = m_Width;
		m_TmpHeight = m_Height;

		XLockDisplay(m_Display);
		XResizeWindow(m_Display,m_Window,m_Width,m_Height);
		XUnlockDisplay(m_Display);
	}

	if(m_Title != m_TmpTitle){
		m_TmpTitle = m_Title;
		XLockDisplay(m_Display);
		XSetStandardProperties( m_Display,
			m_Window,
			m_Title.c_str(),
			NULL,
			None,
			NULL,
			0,
			NULL);
		XUnlockDisplay(m_Display);
	}
	m_Mutex->Unlock();

	if(peekAndWait)
	{
		while(true)
		{
			XLockDisplay(m_Display);
			if(XPending(m_Display))
				XNextEvent(m_Display,&event);

			XUnlockDisplay(m_Display);

			if(processEvents(event,&running))
				return running;
			else
				usleep(1000);
		}
	}
	else
	{
		XLockDisplay(m_Display);
		if(XPending(m_Display))
			XNextEvent(m_Display,&event);

		XUnlockDisplay(m_Display);

		processEvents(event,&running);

		return running;
	}

	return false;
}

Point Window_Linux::GetMousePosition(){
	return m_MousePosition;
}

WindowStatus Window_Linux::GetStatus(){
	return m_Status;
}


ButtonStatus Window_Linux::GetMouseButtonStatus(MouseButton code){
	std::map<MouseButton,ButtonStatus>::iterator entry = m_MouseBtnStatus.find(code);
	if(entry == m_MouseBtnStatus.end())
		return BUTTON_STATUS_UP;
	return entry->second;
}

ButtonStatus Window_Linux::GetKeyStatus(KeyboardButton keycode){
	std::map<KeyboardButton,int>::iterator entry = (*p_KeyboardButtons).find(keycode);
	
	if(entry == (*p_KeyboardButtons).end())
		return BUTTON_STATUS_UP;

	std::map<int,Core::ButtonStatus>::iterator entry2 = m_KeyStatus.find(entry->second);

	if(entry2 == m_KeyStatus.end())
		return BUTTON_STATUS_UP;

	return entry2->second;
}

ButtonStatus Window_Linux::GetKeyCodeStatus(int code){
	if(m_KeyStatus.find(code)==m_KeyStatus.end())
		return BUTTON_STATUS_UP;
	return m_KeyStatus.find(code)->second;
}

void Window_Linux::SetCursorVisiblity(bool visiblity){
	
}

Window_Linux *CreateWindow_Linux(const std::string & Title, unsigned int Width, unsigned int Height, bool FullScreen)
{
	Window_Linux * wnd = new Window_Linux();

	wnd->m_TmpWidth = wnd->m_Width = Width;
	wnd->m_TmpHeight = wnd->m_Height = Height;
	wnd->m_Display = XOpenDisplay( NULL );

	XLockDisplay(wnd->m_Display);
	wnd->m_Window = XCreateSimpleWindow( wnd->m_Display,
                              DefaultRootWindow(wnd->m_Display),
                              0, 0,
                              Width, Height,
                              0,0,
                             0xFF000000);

 	XWindowAttributes currAttr;
	XGetWindowAttributes(wnd->m_Display,wnd->m_Window,&currAttr);


	XMapWindow(wnd->m_Display, wnd->m_Window);
	XMoveWindow(wnd->m_Display, wnd->m_Window,currAttr.screen->width/2-Width/2,currAttr.screen->height/2-Height/2);

	long flags = ExposureMask      |
			VisibilityChangeMask   |
            KeyPressMask           |
            KeyReleaseMask         |
            ButtonPressMask			|
            ButtonReleaseMask		|
            PointerMotionMask		|
            StructureNotifyMask		|
            SubstructureNotifyMask	|
            FocusChangeMask;

    XSelectInput(wnd->m_Display,  wnd->m_Window,flags);

    XSetStandardProperties( wnd->m_Display,
                            wnd->m_Window,
                            Title.c_str(),
                            NULL,
                            None,
                            NULL,
                            0,
                            NULL);

    wnd->m_DeleteMessage = XInternAtom(wnd->m_Display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(wnd->m_Display, wnd->m_Window, &wnd->m_DeleteMessage, 1);
    XUnlockDisplay(wnd->m_Display);

	return wnd;
}

float Window::GetDisplayDensity() {
	return 1.0;
}


_CORE_END

int main(int argc, char *argv[])
{
	initAppDataFolder();

	XInitThreads();

	p_WindowManager = new std::list<Core::Window_Linux*>;
	p_KeyboardButtons = new std::map<Core::KeyboardButton,int>();

	Core::p_CreateButtonMap();

	std::string command = "";

	std::vector<std::string> params(argc-1);
	for(int i = 1;i<argc;i++)
		params.push_back(argv[i]);

	Core::Application_Main_Linux(params);

	p_KeyboardButtons->clear();
	p_WindowManager->clear();

	delete p_KeyboardButtons;
	delete p_WindowManager;

	releaseAppDataFolder();

	return 1;
}

#endif // #if defined(NCK_LINUX)
