
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_WINDOW_LINUX_H
#define NCK_WINDOW_LINUX_H

#include "../nckWindow.h"

#if defined(NCK_LINUX)

#include "../nckThread.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <vector>
#include <map>
_CORE_BEGIN

/// Implementation of interface class Window
class Window_Linux: public Window
{
public:
	Window_Linux();
	virtual ~Window_Linux();

	unsigned int GetWidth();
	unsigned int GetHeight();
	void SetWidth(unsigned int Width);
	void SetHeight(unsigned int Height);
	WindowStatus GetStatus();
	void SetTitle(const std::string & title );
	std::string GetTitle();
	bool Peek(bool peekAndWait);
	ButtonStatus GetMouseButtonStatus(MouseButton btn);
	ButtonStatus GetKeyStatus(KeyboardButton btn);
	ButtonStatus GetKeyCodeStatus(int code);
	Point GetMousePosition();
	void SetCursorVisiblity(bool visible);
	bool processEvents(XEvent event, bool * running);
	Core::Mutex * m_Mutex;
	std::string	m_Title,m_TmpTitle;
	unsigned int	m_Width,m_TmpWidth;
	unsigned int	m_Height,m_TmpHeight;
	WindowStatus	m_Status;
	std::map<int,ButtonStatus> m_KeyStatus;
	std::map<MouseButton,ButtonStatus> m_MouseBtnStatus;
	Atom m_DeleteMessage;
	Point m_MousePosition;
	bool m_Buffered;
	Display *m_Display;
	XID  m_Window;
};

/// Create a window.
Window_Linux *CreateWindow_Linux(const std::string & Title, unsigned int Width,
							unsigned int Height, bool FullScreen);

/// Application entry point.
void Application_Main_Linux(const std::vector<std::string> & CmdLine);
	
_CORE_END

#endif // #if defined(NCK_LINUX)

#endif // #ifndef NCK_WINDOW_LINUX_H
