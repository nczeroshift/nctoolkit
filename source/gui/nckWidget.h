
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_WIDGET_H
#define NCK_WIDGET_H

#include "nckWidgetRenderer.h"
#include "nckWindow.h"
#include "nckChronometer.h"
#include "nckVec2.h"
#include "nckSharePtr.h"

#include <list>
#include <vector>
#include <map>

#include <stdint.h>

_GUI_BEGIN


/**
 * Available widget types.
 */
enum WidgetType{
	WIDGET_UNDEFINED,
	WIDGET_MANAGER,
	WIDGET_PUSH_BUTTON,
	WIDGET_TEXT_LABEL,
	WIDGET_CHECKBOX_BUTTON,
	WIDGET_OPTION_BUTTON,
	WIDGET_SELECTION_BUTTON_AGGREGATOR,
	WIDGET_VALUE_RANGE_BUTTON,
	WIDGET_OPTION_RANGE_BUTTON,
	WIDGET_POPUP_MENU,
	WIDGET_MENU_OPTION_BUTTON,
};


/**
 * User input state class.
 */
class UserInputState
{
public:
	UserInputState();

	/// Apply offset to cursor position.
	UserInputState(const UserInputState & ref, float rX, float rY);

	// Mouse/Pointing device position.
	float m_CursorX;
	float m_CursorY;

	// Left/Right/Middle mouse button status.
	bool m_State_LMB;
	bool m_State_RMB;
	bool m_State_MMB;

	// Control button status.
	bool m_State_LControl;
	bool m_State_RControl;

	// Shift button status.
	bool m_State_LShift;
	bool m_State_RShift;

	// Text input characters.
	std::string m_TextInputCharacter;
};


class Widget;

/**
 * Interface class for event handling.
 */
class CallbackHandler{
public:
	virtual ~CallbackHandler(){};

	virtual void OnButtonFocus(Widget * widget){};
	virtual void OnButtonDown(Widget * widget){};
	virtual void OnButtonClick(Widget * widget){};
	virtual void OnButtonUp(Widget * widget){};
	virtual void OnValueChanged(Widget * widget){};
};

/**
 * Main abstract class for all the graphical user interface elements.
 */
class Widget{
public:
	Widget(float x, float y, float width, float height);
	virtual ~Widget()=0;
	virtual void Render(WidgetRenderer * wRender) = 0;
	virtual void Update(const UserInputState & input, Widget ** focusWidget = NULL) = 0;
	inline float GetX() const{return m_X;};
	inline float GetY() const{return m_Y;};
	inline float GetWidth() const{return m_Width;};
	inline float GetHeight() const{return m_Height;};
	inline Widget & SetX(float value) {m_X=value;return *this;};
	inline Widget & SetY(float value) {m_Y=value;return *this;};
	inline Widget & SetWidth(float value) {m_Width=value;return *this;};
	inline Widget & SetHeight(float value) {m_Height=value;return *this;};
	void SetCallback(CallbackHandler * callback);
	CallbackHandler * GetCallback();
	inline Widget * GetParent(){return m_Parent;}
	inline void SetParent(Widget * parent){m_Parent=parent;}
	inline void SetTooltip(const std::string & text){m_Tooltip = text;}
	inline std::string GetTooltip(){return m_Tooltip;}
	virtual WidgetType GetType()=0;
protected:
	bool CheckCursorOver(const UserInputState & input);
	std::string m_Tooltip;
	WidgetState m_State;
	WidgetState m_LastState;
	CallbackHandler * m_Callback;
	Widget * m_Parent;
	float m_X;
	float m_Y;
	float m_Width;
	float m_Height;
	bool m_Enabled;
};

/**
 * Simple push button class.
 */
class PushButton : public Widget
{
public:
	PushButton(float x, float y, float width, float height, const std::string & text);
	virtual ~PushButton();
	void SetText(const std::string & text);
	std::string GetText(const std::string & text);
	ShapeRenderer::RoundMode GetRoundMode();
	void SetRoundMode(ShapeRenderer::RoundMode roundMode);
	void Render(WidgetRenderer * wRender);
	void Update(const UserInputState & input, Widget ** focusWidget = NULL);
	WidgetType GetType(){return WIDGET_PUSH_BUTTON;}
private:
	ShapeRenderer::RoundMode m_RoundMode;
	std::string m_Text;
};

/**
 * Abstract class for the selectable buttons.
 */
class GenericSelectionButton : public Widget{
public:
	GenericSelectionButton(float x, float y, float width, float height);
	virtual ~GenericSelectionButton(){};
	inline void SetValue(bool value) { m_Value = value;}
	inline bool GetValue() const { return m_Value; }
	inline void SetText(const std::string & text){m_Text=text;}
	std::string GetText(const std::string & text) const {return m_Text;}
protected:
	bool m_Value;
	std::string m_Text;
};

/**
 * Single option selection button class.
 */
class OptionButton : public virtual GenericSelectionButton
{
public:
	OptionButton(float x, float y, float width, float height, const std::string & text);
	virtual ~OptionButton();

	inline void SetRoundMode(ShapeRenderer::RoundMode roundMode){m_RoundMode=roundMode;}
	inline ShapeRenderer::RoundMode GetRoundMode() const {return m_RoundMode;}

	void Render(WidgetRenderer * wRender);
	void Update(const UserInputState & input, Widget ** focusWidget = NULL);
	WidgetType GetType(){return WIDGET_OPTION_BUTTON;}

private:
	ShapeRenderer::RoundMode m_RoundMode;
};

/**
 * Multiple option selection button class.
 */
class CheckBoxButton : public virtual GenericSelectionButton{
public:
	CheckBoxButton(float x, float y, float size, const std::string & text);
	virtual ~CheckBoxButton();

	void Render(WidgetRenderer * wRender);
	void Update(const UserInputState & input, Widget ** focusWidget = NULL);
	WidgetType GetType(){return WIDGET_CHECKBOX_BUTTON;}


private:
	ShapeRenderer::RoundMode m_RoundMode;
};

/**
 * Selection button aggregator class.
 */
class SelectionButtonGroup : public Widget
{
public:
	SelectionButtonGroup(float x, float y, float width, float height);
	virtual ~SelectionButtonGroup();

	void Render(WidgetRenderer * wRender);
	void Update(const UserInputState & input, Widget ** focusWidget = NULL);
	WidgetType GetType(){return WIDGET_SELECTION_BUTTON_AGGREGATOR;}

	bool AddButton(int64_t value,Core::SharedPtr<GenericSelectionButton> button);

	inline int64_t GetValue() const{return m_Value;}
	inline void SetValue(int64_t value) { m_Value = value; }

	void GetKeys(std::list<int64_t> * copyList);
	Core::SharedPtr<GenericSelectionButton> GetButton(int64_t keyId);

	inline WidgetType GetGroupType(){return m_GroupType;}
private:
	WidgetType m_GroupType;
	int64_t m_Value;
	std::map<int64_t, Core::SharedPtr<GenericSelectionButton> > m_Buttons;
};

/**
 * Single option selection range button class.
 */
class RangeOptionButton : public Widget
{
public:
	RangeOptionButton(float x, float y, float width, float height,
			const std::vector< std::pair<int,std::string> > & options);
	virtual ~RangeOptionButton();

	ShapeRenderer::RoundMode GetRoundMode();
	void SetRoundMode(ShapeRenderer::RoundMode roundMode);

	void Render(WidgetRenderer * wRender);
	void Update(const UserInputState & input, Widget ** focusWidget = NULL);
	WidgetType GetType(){return WIDGET_OPTION_RANGE_BUTTON;}

	inline int GetSelectedOption(){return m_CurrentOption;};
	inline void SetSelectedOption(int option){m_CurrentOption=option;};
	std::vector< std::pair<int,std::string> > GetOptions(){return m_Options;}
private:
	ShapeRenderer::RoundMode m_RoundMode;
	std::vector< std::pair<int,std::string> > m_Options;
	int m_CurrentOption;
};

/**
 * Value tunning range button class.
 */
class RangeValueButton : public Widget
{
public:
	RangeValueButton(float x, float y, float width, float height,float min, float max,float value);
	virtual ~RangeValueButton();

	ShapeRenderer::RoundMode GetRoundMode();
	void SetRoundMode(ShapeRenderer::RoundMode roundMode);

	void Render(WidgetRenderer * wRender);
	void Update(const UserInputState & input, Widget ** focusWidget = NULL);
	WidgetType GetType(){return WIDGET_VALUE_RANGE_BUTTON;}

	inline float GetMax(){return m_Max;}
	inline float GetMin(){return m_Min;}
	inline float GetBigChange(){return m_BigChange;}
	inline float GetSmallhange(){return m_SmallChange;}
	inline float GetValue(){return m_Value;}

	inline void SetMax(float max){m_Max=max;}
	inline void SetMin(float min){m_Min=min;}
	inline void SetBigChange(float bigChange){m_BigChange=bigChange;}
	inline void SetSmallhange(float smallChange){m_SmallChange=smallChange;}
	inline void SetValue(float value){m_Value=value;}

	inline std::string GetText(){return m_Text;}
	inline void SetText(const std::string & label){m_Text=label;}
private:
	ShapeRenderer::RoundMode m_RoundMode;
	float m_Max;
	float m_Min;
	float m_Value;
	float m_BigChange;
	float m_SmallChange;
	std::string m_Text;
};


/**
 * Text label class.
 */
class Label: public Widget
{
public:
	Label(float x, float y,const std::string & text);
	virtual ~Label(){};

	inline std::string GetText(){return m_Text;}
	inline void SetText(const std::string & label){m_Text=label;}
	WidgetType GetType(){return WIDGET_TEXT_LABEL;}


	void Render(WidgetRenderer * wRender);
	void Update(const UserInputState & input, Widget ** focusWidget = NULL);

	void SetTextBold(bool isBold){m_TextBold = isBold;}
	void SetTextSize(float size){m_TextSize = size;}
	void SetTextShadow(bool enabled){m_TextDropShadow = enabled;};
	bool GetTextBold(){return m_TextBold;}
	float GetTextSize(){return m_TextSize;}
	bool GetTextShadow(){return m_TextDropShadow;};

private:
	bool m_TextDropShadow;
	FontAlignment m_TextAlign;
	bool m_TextBold;
	float m_TextSize;
	Math::Color4ub m_TextColor;
	std::string m_Text;
};

/**
 * UI Widgets core manager class.
 */
class Manager : public Widget
{
public:
	Manager(float x, float y, float width, float height,WidgetRenderer * wr);
	Manager(float x, float y, float width, float height,Manager * parent);

	~Manager();

	WidgetRenderer * GetRenderer();

	Core::SharedPtr<Widget> Add(Core::SharedPtr<Widget> widget);

	void Render();

	void Render(WidgetRenderer * wRender);
	void Update(const UserInputState & input, Widget ** focusWidget = NULL);

	WidgetType GetType(){return WIDGET_MANAGER;}

private:
	Widget * m_FocusWidget;
	Core::Chronometer * m_FocusTimer;
	WidgetRenderer * m_Renderer;
	std::list< Core::SharedPtr<Widget> > m_Widgets;
};

_GUI_END

#endif // #ifndef NCK_WIDGET_H
