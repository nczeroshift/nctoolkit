
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_WINDOW_MACOSX_H
#define NCK_WINDOW_MACOSX_H

#include "../nckWindow.h"

#if defined(NCK_MACOSX)

#include "../nckThread.h"
#include <vector>
#include <map>

_CORE_BEGIN

/// Window interface implementation for MacOSX
class Window_MacOSX: public Window
{
public:
	Window_MacOSX();
	virtual ~Window_MacOSX();
    
	unsigned int GetWidth();
	unsigned int GetHeight();
	void SetWidth(unsigned int Width);
	void SetHeight(unsigned int Height);
	WindowStatus GetStatus();
	void SetTitle(const std::string & title);
    std::string GetTitle();
	bool Peek(bool peekAndWait);
	ButtonStatus GetMouseButtonStatus(MouseButton btn);
	ButtonStatus GetKeyStatus(KeyboardButton btn);
    ButtonStatus GetKeyCodeStatus(int code);
	Point GetMousePosition();
	void SetCursorVisiblity(bool visible);
    bool m_IsClosed;
	std::map<int,ButtonStatus> m_KeyStatus;
	std::map<MouseButton,ButtonStatus> m_MouseBtnStatus;
	Point m_MousePosition;
    void * m_Private;
};

Window_MacOSX *CreateWindow_MacOSX(const std::string & Title, unsigned int Width,
                                 unsigned int Height, bool FullScreen);

void Application_Main_MacOSX(const std::vector<std::string> & CmdLine);

_CORE_END

std::string Application_Filename_MacOSX();
int64_t FileModified_MacOSX(const std::string & filename);

void * AutoReleasePool_Create_MacOSX();
void AutoReleasePool_Destroy_MacOSX(void * pool);
void MessageBox_MacOSX(const std::string & msg, const std::string & title);

void * Mutex_Create_MacOSX();
void Mutex_Destroy_MacOSX(void * lock);
void Mutex_Lock_MacOSX(void * lock);
void Mutex_Unlock_MacOSX(void * lock);

#endif  // #if defined(NCK_MACOSX)

#endif // #ifndef NCK_WINDOW_MACOSX_H
