
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_WINDOW_H
#define NCK_WINDOW_H

#include "nckPoint.h"
#include <string>
#include <vector>

_CORE_BEGIN

/// Window state.
enum WindowStatus
{	
	WINDOW_IDLE=0,
	WINDOW_ACTIVE,
};

/// Button status.
enum ButtonStatus
{
	BUTTON_STATUS_UP	= 0,
	BUTTON_STATUS_DOWN	= 1,
};

/// Buttons identifiers.
enum KeyboardButton
{
	BUTTON_ESCAPE	= 0,
	BUTTON_ENTER	= 1,
	BUTTON_SPACE	= 2,
	BUTTON_UP		= 3,
	BUTTON_DOWN		= 4,
	BUTTON_LEFT		= 5,
	BUTTON_RIGHT	= 6,
	BUTTON_Z		= 7,
	BUTTON_X		= 8,
	BUTTON_C		= 9,
	BUTTON_V		= 10,
};

/// Mouse buttons identifiers.
enum MouseButton{
	BUTTON_MOUSE_LEFT	= 0,
	BUTTON_MOUSE_MIDDLE	= 1,
	BUTTON_MOUSE_RIGHT	= 2,
};

/// Interface class for the graphic window.
class Window
{
public:
	/// Destructor.
	virtual ~Window(){};

	/// Get graphic window width.
	virtual unsigned int GetWidth()=0;

	/// Get graphic window height.
	virtual unsigned int GetHeight()=0;

	/// Set graphic window width.
	virtual void SetWidth(unsigned int Width)=0;

	/// Set graphic window height. 
	virtual void SetHeight(unsigned int Height)=0;
	
	/// Get window status.
	virtual WindowStatus GetStatus()=0;

	/// Set window title text.
	virtual void SetTitle(const std::string & title )=0;

	/// Get window title text.
	virtual std::string GetTitle() = 0;

	/// Update window messages.
	virtual bool Peek(bool peekAndWait = false)=0;

	/// Get mouse button status.
	virtual ButtonStatus GetMouseButtonStatus(MouseButton btn)=0;

	/// Get keyboard button status.
	virtual ButtonStatus GetKeyStatus(KeyboardButton btn)=0;

	/// Get keyboard keys status.
	virtual ButtonStatus GetKeyCodeStatus(int code) = 0;

	/// Get mouse cursor position in graphic window space.
	virtual Point GetMousePosition()=0;

	/// Set mouse cursor visibility.
	virtual void SetCursorVisiblity(bool visible)=0;
};

/// Create graphic window. 
Window *CreateWindow(const std::string & Title, unsigned int Width, unsigned int Height, bool fullscreen = false);

/// Application entry point.
void Application_Main(const std::vector<std::string> & CmdLine);

/// Popup message box.
void MsgBox(const std::string & text,const std::string & title);

_CORE_END

#endif // #ifndef NCK_WINDOW_H
