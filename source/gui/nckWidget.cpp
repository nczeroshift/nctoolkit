
/**
 * NCtoolKit © 2007-2015 Lu’s F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckWidget.h"
#include "nckUtils.h"
#include "nckException.h"
#include "nckMathUtils.h"

#include <stdio.h>

#define DESTRUCTOR_MSG(args) printf(args)

_GUI_BEGIN

UserInputState::UserInputState(){
	m_State_LMB = false;
	m_State_RMB = false;
	m_State_MMB = false;
}

UserInputState::UserInputState(const UserInputState & ref, float rX, float rY)
{
	m_CursorX = ref.m_CursorX-rX;
	m_CursorY = ref.m_CursorY-rY;
	m_State_LMB = ref.m_State_LMB;
	m_State_RMB = ref.m_State_RMB;
	m_State_MMB = ref.m_State_MMB;
}

Widget::Widget(float x, float y, float width, float height){
	m_Callback = NULL;
	m_X = x;m_Y = y;
	m_Width = width;m_Height = height;
	m_Enabled = true;
	m_Parent = NULL;
	m_State = WIDGET_NORMAL;
	m_LastState = WIDGET_NORMAL;
}

Widget::~Widget(){
}

void Widget::SetCallback(CallbackHandler * callback){
	m_Callback = callback;
}

CallbackHandler * Widget::GetCallback(){
	return m_Callback;
}

bool Widget::CheckCursorOver(const UserInputState & input)
{
	if((input.m_CursorX >= m_X) && (input.m_CursorX <= m_X + m_Width) &&
				(input.m_CursorY >= m_Y) && (input.m_CursorY <= m_Y + m_Height))
		return true;
	return false;
}

PushButton::PushButton(float x, float y, float width, float height, const std::string & name) : Widget(x,y,width,height){
	m_Text = name;
	m_RoundMode = ShapeRenderer::ROUND_ALL;
}

PushButton::~PushButton(){
	DESTRUCTOR_MSG("PushButton destructor\n");
}

ShapeRenderer::RoundMode PushButton::GetRoundMode(){
	return m_RoundMode;
}

void PushButton::SetRoundMode(ShapeRenderer::RoundMode roundMode){
	m_RoundMode = roundMode;
}
void PushButton::Render(WidgetRenderer * wRender){
	wRender->Button(m_X,m_Y,m_Width,m_Height,m_Text,m_State,m_RoundMode);
}

void PushButton::Update(const UserInputState & input, Widget ** focusWidget)
{
	m_LastState = m_State;

	if(CheckCursorOver(input))
	{
		if(m_LastState == WIDGET_NORMAL && input.m_State_LMB)
			return;

		if(focusWidget)
			*focusWidget = this;

		if(m_Callback && m_LastState == WIDGET_NORMAL)
			m_Callback->OnButtonFocus(this);

		if(!input.m_State_LMB && m_State == WIDGET_PRESSED)
		{
			if(m_Callback)m_Callback->OnButtonClick(this);
		}

		if(input.m_State_LMB)
		{
			if(m_Callback && m_LastState != WIDGET_PRESSED)
				if(m_Callback)m_Callback->OnButtonDown(this);

			m_State = WIDGET_PRESSED;
		}
		else
		{
			if(m_Callback && m_LastState == WIDGET_PRESSED)
				if(m_Callback)m_Callback->OnButtonUp(this);

			m_State = WIDGET_HIGHLIGHTED;
		}
	}
	else
	{
		m_State = WIDGET_NORMAL;
	}
}


RangeOptionButton::RangeOptionButton(float x, float y, float width, float height,
		const std::vector< std::pair<int,std::string> > & options) : Widget(x,y,width,height){
	m_Options = options;
	m_RoundMode = ShapeRenderer::ROUND_ALL;
	m_CurrentOption = 0;
}

RangeOptionButton::~RangeOptionButton(){
	DESTRUCTOR_MSG("RangeOptionButton destructor\n");
}

ShapeRenderer::RoundMode RangeOptionButton::GetRoundMode(){
	return m_RoundMode;
}

void RangeOptionButton::SetRoundMode(ShapeRenderer::RoundMode roundMode){
	m_RoundMode = roundMode;
}

void RangeOptionButton::Render(WidgetRenderer * wRender)
{
	std::pair<int,std::string> val = m_Options[m_CurrentOption];
	wRender->RangeOptionButton(m_X,m_Y,m_Width,m_Height,val.second,m_State,m_RoundMode);
}

void RangeOptionButton::Update(const UserInputState & input, Widget ** focusWidget)
{
	m_LastState = m_State;

	if(CheckCursorOver(input))
	{
		if(m_LastState == WIDGET_NORMAL && input.m_State_LMB)
			return;

		if(focusWidget)
			*focusWidget = this;

		if(m_Callback && m_LastState == WIDGET_NORMAL)
			m_Callback->OnButtonFocus(this);

		if(!input.m_State_LMB && m_State == WIDGET_PRESSED)
		{
			if(input.m_CursorX >= m_X+m_Width*0.5)
			{
				int nextValue = m_CurrentOption+1;
				if(nextValue >= m_Options.size())
					nextValue = 0;
				m_CurrentOption = nextValue;
			}
			else
			{
				int nextValue = m_CurrentOption-1;
				if(nextValue <0 && m_Options.size()>0)
					nextValue = (int)m_Options.size()-1;
				m_CurrentOption = nextValue;
			}

			if(m_Callback)m_Callback->OnButtonClick(this);
		}

		if(input.m_State_LMB)
		{
			if(m_Callback && m_LastState != WIDGET_PRESSED)
				if(m_Callback)m_Callback->OnButtonDown(this);

			m_State = WIDGET_PRESSED;
		}
		else
		{
			if(m_Callback && m_LastState == WIDGET_PRESSED)
				if(m_Callback)m_Callback->OnButtonUp(this);

			m_State = WIDGET_HIGHLIGHTED;
		}
	}
	else
	{
		m_State = WIDGET_NORMAL;
	}
}



RangeValueButton::RangeValueButton(float x, float y, float width, float height,
		float min, float max,float value) : Widget(x,y,width,height)
{
	m_RoundMode = ShapeRenderer::ROUND_ALL;
	m_Min = min;
	m_Max = max;
	m_Value = value;
	m_BigChange = (m_Max - m_Min)/20.0f;
	m_SmallChange = (m_Max - m_Min)/200.0f;
}

RangeValueButton::~RangeValueButton(){
	DESTRUCTOR_MSG("RangeValueButton destructor\n");
}

ShapeRenderer::RoundMode RangeValueButton::GetRoundMode(){
	return m_RoundMode;
}

void RangeValueButton::SetRoundMode(ShapeRenderer::RoundMode roundMode){
	m_RoundMode = roundMode;
}


void RangeValueButton::Render(WidgetRenderer * wRender)
{
	int decimalPlaces = 4;

	if(fabs(m_Value)>=10)
		decimalPlaces = 3;
	if(fabs(m_Value)>=100)
		decimalPlaces = 2;
	if(fabs(m_Value)>=1000)
		decimalPlaces = 1;

	float tempValue = m_Value;
	if(fabs(m_Value)<0.00001)
		tempValue = 0.0;

	std::string label="";
	if(m_Text.length()>0)
		label = m_Text+": ";

	wRender->RangeValueButton(m_X,m_Y,m_Width,m_Height,label+Math::FloatToString(tempValue,decimalPlaces),m_State,m_RoundMode);
}

void RangeValueButton::Update(const UserInputState & input, Widget ** focusWidget)
{
	m_LastState = m_State;

	if(CheckCursorOver(input))
	{
		if(m_LastState == WIDGET_NORMAL && input.m_State_LMB)
			return;

		if(focusWidget)
			*focusWidget = this;

		if(m_Callback && m_LastState == WIDGET_NORMAL)
			m_Callback->OnButtonFocus(this);

		if(!input.m_State_LMB && m_State == WIDGET_PRESSED)
		{
			if(input.m_CursorX >= m_X+m_Width*0.5)
			{
				float nextValue = m_Value + m_BigChange;
				if(nextValue >= m_Max)
					nextValue = m_Max;
				m_Value = nextValue;
			}
			else
			{
				float nextValue = m_Value - m_BigChange;
				if(nextValue <= m_Min)
					nextValue = m_Min;
				m_Value = nextValue;
			}

			if(m_Callback)m_Callback->OnButtonClick(this);
		}

		if(input.m_State_LMB)
		{
			if(m_Callback && m_LastState != WIDGET_PRESSED)
				if(m_Callback)m_Callback->OnButtonDown(this);

			m_State = WIDGET_PRESSED;
		}
		else
		{
			if(m_Callback && m_LastState == WIDGET_PRESSED)
				if(m_Callback)m_Callback->OnButtonUp(this);

			m_State = WIDGET_HIGHLIGHTED;
		}
	}
	else
	{
		m_State = WIDGET_NORMAL;
	}
}

GenericSelectionButton::GenericSelectionButton(float x, float y, float width, float height) : Widget(x,y,width,height){
	m_Value = false;
	m_Text = "Selection Button";
}

CheckBoxButton::CheckBoxButton(float x, float y, float size, const std::string & text) : GenericSelectionButton(x,y,size,size)
{
	m_Text = text;
}

CheckBoxButton::~CheckBoxButton(){
	DESTRUCTOR_MSG("CheckBoxButton destructor\n");
}

void CheckBoxButton::Render(WidgetRenderer * wRender)
{
	wRender->CheckBoxButton(m_X,m_Y,m_Height, m_Width,m_Text,m_Value,m_State,ShapeRenderer::ROUND_ALL);
}

void CheckBoxButton::Update(const UserInputState & input, Widget ** focusWidget ){
	m_LastState = m_State;

	if(CheckCursorOver(input))
	{
		if(m_LastState == WIDGET_NORMAL && input.m_State_LMB)
			return;

		if(focusWidget)
			*focusWidget = this;

		if(m_Callback && m_LastState == WIDGET_NORMAL)
			m_Callback->OnButtonFocus(this);

		if(!input.m_State_LMB && m_State == WIDGET_PRESSED)
		{
			if(m_Callback)m_Callback->OnButtonClick(this);
			m_Value = !m_Value;
		}

		if(input.m_State_LMB)
		{
			if(m_Callback && m_LastState != WIDGET_PRESSED)
				if(m_Callback)m_Callback->OnButtonDown(this);

			m_State = WIDGET_PRESSED;
		}
		else
		{
			if(m_Callback && m_LastState == WIDGET_PRESSED)
				if(m_Callback)m_Callback->OnButtonUp(this);

			m_State = WIDGET_HIGHLIGHTED;
		}
	}
	else
	{
		m_State = WIDGET_NORMAL;
	}
}


OptionButton::OptionButton(float x, float y, float width, float height, const std::string & text):GenericSelectionButton(x,y,width,height)
{
	m_RoundMode = ShapeRenderer::ROUND_ALL;
	m_Text = text;
}

OptionButton::~OptionButton(){
	DESTRUCTOR_MSG("OptionButton destructor\n");
}

void OptionButton::Render(WidgetRenderer * wRender)
{
	wRender->ToggleButton(m_X,m_Y,m_Width,m_Height,m_Text,m_Value,m_State,m_RoundMode);
}

void OptionButton::Update(const UserInputState & input, Widget ** focusWidget)
{
	m_LastState = m_State;

	if(CheckCursorOver(input))
	{
		if(m_LastState == WIDGET_NORMAL && input.m_State_LMB)
			return;

		if(focusWidget)
			*focusWidget = this;

		if(m_Callback && m_LastState == WIDGET_NORMAL)
			m_Callback->OnButtonFocus(this);

		if(!input.m_State_LMB && m_State == WIDGET_PRESSED)
		{
			if(m_Callback)m_Callback->OnButtonClick(this);
			m_Value = !m_Value;
		}

		if(input.m_State_LMB)
		{
			if(m_Callback && m_LastState != WIDGET_PRESSED)
				if(m_Callback)m_Callback->OnButtonDown(this);

			m_State = WIDGET_PRESSED;
		}
		else
		{
			if(m_Callback && m_LastState == WIDGET_PRESSED)
				if(m_Callback)m_Callback->OnButtonUp(this);

			m_State = WIDGET_HIGHLIGHTED;
		}
	}
	else
	{
		m_State = WIDGET_NORMAL;
	}
}

SelectionButtonGroup::SelectionButtonGroup(float x, float y, float width, float height) : Widget(x,y,width,height){
	m_GroupType = WIDGET_UNDEFINED;
	m_Value = 0;
}

SelectionButtonGroup::~SelectionButtonGroup(){
	m_Buttons.clear();
	DESTRUCTOR_MSG("SelectionButtonGroup destructor\n");
}

void SelectionButtonGroup::Render(WidgetRenderer * wRender)
{
	wRender->PushTranslation(m_X,m_Y);
	for(std::map< int64_t, Core::SharedPtr<GenericSelectionButton> >::iterator i = m_Buttons.begin();i!=m_Buttons.end();i++)
	{
		i->second->Render(wRender);
	}
	wRender->PopTranslation();
}

void SelectionButtonGroup::Update(const UserInputState & input, Widget ** focusWidget)
{
	UserInputState iState = UserInputState(input,m_X,m_Y);

	int64_t retValue = 0;

	for(std::map<int64_t, Core::SharedPtr<GenericSelectionButton> >::iterator i = m_Buttons.begin();i!=m_Buttons.end();i++)
	{
		i->second->Update(iState,focusWidget);
		if(i->second->GetValue())
			retValue |= i->first;
	}

	if(m_GroupType == WIDGET_OPTION_BUTTON)
	{
		if((retValue ^ m_Value) != 0)
		{
			m_Value ^= retValue;

			for(std::map<int64_t, Core::SharedPtr<GenericSelectionButton> >::iterator i = m_Buttons.begin();i!=m_Buttons.end();i++)
			{
				if((i->first & m_Value) != i->first)
					i->second->SetValue(false);
			}
		}
	}
	else if(m_GroupType == WIDGET_CHECKBOX_BUTTON)
		m_Value = retValue;
}

bool SelectionButtonGroup::AddButton(int64_t value,Core::SharedPtr<GenericSelectionButton> button)
{
	if(m_Buttons.size()==0)
	{
		m_GroupType = button->GetType();
	}
	else if(m_GroupType!=button->GetType())
		return false;

	button->SetParent(this);

	m_Buttons.insert(std::pair<int64_t, Core::SharedPtr<GenericSelectionButton> >(value,button));

	return true;
}

void SelectionButtonGroup::GetKeys(std::list<int64_t> * copyList)
{
	if(!copyList)
		return;

	for(std::map<int64_t, Core::SharedPtr<GenericSelectionButton> >::iterator i = m_Buttons.begin();i!=m_Buttons.end();i++)
	{
		copyList->push_back(i->first);
	}
}

Core::SharedPtr<GenericSelectionButton> SelectionButtonGroup::GetButton(int64_t keyId)
{
	std::map<int64_t, Core::SharedPtr<GenericSelectionButton> >::iterator i = m_Buttons.find(keyId);


	if(i!=m_Buttons.end())
	{
		return i->second;
	}

	return NULL;
}

Label::Label(float x, float y,const std::string & text):Widget(x,y,1,1)
{
	m_Text = text;
	m_TextBold = false;
	m_TextSize = 16;
	m_TextColor = Math::Color4ub(0,0,0,255);
	m_TextAlign = FONT_ALIGNMENT_LEFT;
	m_TextDropShadow = true;
}


void Label::Render(WidgetRenderer * wRender){
	if(m_TextDropShadow)
		wRender->Label(m_X+1,m_Y+1,m_Text,m_TextSize,m_TextBold,Math::Color4ub(135,135,135),m_TextAlign);

	wRender->Label(m_X,m_Y,m_Text,m_TextSize,m_TextBold,m_TextColor,m_TextAlign);
}

void Label::Update(const UserInputState & input, Widget ** focusWidget){

}

Manager::Manager(float x, float y, float width, float height,WidgetRenderer * wr): Widget(x,y,width,height){
	m_Renderer = wr;
	m_FocusTimer = Core::CreateChronometer();
	m_FocusWidget = NULL;
}

Manager::Manager(float x, float y, float width, float height,
		Manager * parent): Widget(x,y,width,height){
	m_Parent = parent;
	m_FocusWidget = NULL;
}

Manager::~Manager()
{
	m_Widgets.clear();
	SafeDelete(m_FocusTimer);
}

Core::SharedPtr<Widget> Manager::Add(Core::SharedPtr<Widget> widget)
{
	widget->SetParent(this);
	m_Widgets.push_back(widget);
	return widget;
}

void Manager::Render(){
	Render(m_Renderer);
}

void Manager::Render(WidgetRenderer * wRender)
{
	wRender->PushTranslation(m_X,m_Y);

	ListFor( Core::SharedPtr<Widget> ,m_Widgets,i)
		(*i)->Render(wRender);

	Widget * tfWidget = m_FocusWidget;

	if(tfWidget && m_FocusTimer->GetElapsedTime()>1000 && tfWidget->GetTooltip().length()>0)
	{
		float x = tfWidget->GetX();
		float y = tfWidget->GetY();
		float dx = tfWidget->GetWidth()*0.5;
		float dy = tfWidget->GetHeight()*1.5;

		if(x+dx>m_Width)
		{
			dx = -dx;
		}

		if(y+dy>m_Height)
		{
			dy = -dy;
		}

		wRender->Tooltip(x+dx,y+dy,tfWidget->GetTooltip());
	}

	wRender->PopTranslation();
}

void Manager::Update(const UserInputState & input,Widget ** focusWidget)
{
	UserInputState relative = UserInputState(input,m_X,m_Y);

	Widget * tempFocus = NULL;
	ListFor( Core::SharedPtr<Widget> ,m_Widgets,i)
		(*i)->Update(relative,&tempFocus);

	if(tempFocus != m_FocusWidget){
		m_FocusWidget = tempFocus;

		if(m_FocusWidget)
			m_FocusTimer->Start();
		else
			m_FocusTimer->Clear();
	}


}

WidgetRenderer * Manager::GetRenderer(){
	return m_Renderer;
}


_GUI_END
