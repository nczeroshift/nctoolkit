
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_WINDOW_WIN32_H
#define NCK_WINDOW_WIN32_H

#include "../nckWindow.h"

#if defined(NCK_WINDOWS)

#include <windows.h>
#undef CreateMutex // Clear windows defines rubish

#include "../nckThread.h"
#include <map>
#include <vector>

_CORE_BEGIN

/// Implementation of interface class Window
class Window_Win32: public Window
{
public:
	Window_Win32();
	~Window_Win32();

	unsigned int GetWidth();
	unsigned int GetHeight();
	void SetWidth(unsigned int Width);
	void SetHeight(unsigned int Height);
	WindowStatus GetStatus();
	void SetTitle(const std::string & title );
	std::string GetTitle();
	bool Peek(bool peekAndWait);
	ButtonStatus GetKeyStatus(KeyboardButton code);
	Point GetMousePosition();
	void SetCursorVisiblity(bool visible);
	ButtonStatus GetKeyCodeStatus(int key);
	ButtonStatus GetMouseButtonStatus(MouseButton code);
	
	Core::Mutex		*m_Mutex;
	std::string		m_Title,m_TmpTitle;
	unsigned int	m_Width,m_TmpWidth;
	unsigned int	m_Height,m_TmpHeight;
	DWORD			m_Flags;
	WNDCLASSEX		m_Class;
	HINSTANCE		m_Instance;
	HWND			m_Handle;
	MSG				m_Message;
	WindowStatus	m_Status;
	std::map<int,ButtonStatus> m_KeyStatus;
	std::map<MouseButton,ButtonStatus> m_MouseBtnStatus;
};

/// Create a window.
Window_Win32 *CreateWindow_Win32(const std::string & Title, unsigned int Width,
							unsigned int Height, bool FullScreen);

/// Application entry point.
void Application_Main_Win32(const std::vector<std::string> & CmdLine);
	
_CORE_END

#endif // #if defined(NCK_WINDOWS)

#endif
