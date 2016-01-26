
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 2 - Graphical User Interface
*/

#include "nckDemo_2.h"

class WidgetEventsHandler : public Gui::CallbackHandler
{
public:
	~WidgetEventsHandler(){}

	void OnButtonFocus(Gui::Widget * widget){
		printf("Widget[0x%p], got focus.\n",widget);
	}

	void OnButtonDown(Gui::Widget * widget){
		printf("Widget[0x%p], event down.\n",widget);
	}

	void OnButtonClick(Gui::Widget * widget){
		printf("Widget[0x%p], click.\n",widget);
	}

	void OnButtonUp(Gui::Widget * widget){
		printf("Widget[0x%p], event up.\n",widget);
	}

	void OnValueChanged(Gui::Widget * widget){
		printf("Widget[0x%p], value changed.\n",widget);
	}
};

Demo2::Demo2(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;

	fontTexture	= NULL;
	fontMap		= NULL;
	shpRender	= NULL;
	wgtRender	= NULL;
	wgtEventsHandler = new WidgetEventsHandler();
	wgtManager = NULL;
}

Demo2::~Demo2(){
	SafeDelete(wgtEventsHandler);
	SafeDelete(wgtManager);
	SafeDelete(wgtRender);
	SafeDelete(shpRender);
	SafeDelete(fontTexture);
	SafeDelete(fontMap);
}

void Demo2::Load(){
	dev->Enable(Graph::STATE_BLEND);
	dev->Enable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);
	
	fontTexture = dev->LoadTexture("texture://sans_serif.png");
	fontTexture->SetFilter(Graph::FILTER_MIPMAPPING,Graph::FILTER_NEAREST);
	fontTexture->SetFilter(Graph::FILTER_MAGNIFICATION,Graph::FILTER_NEAREST);
	fontTexture->SetFilter(Graph::FILTER_MINIFICATION,Graph::FILTER_NEAREST);

	fontMap = new Gui::FontMap(dev);
	fontMap->Load("script://sans_serif.txt");
	fontMap->SetPositionAccuracy(Gui::FONT_POSITION_ACCURACY_INTEGER);

	shpRender = new Gui::ShapeRenderer(dev);
	wgtRender = new Gui::BlenderWidgetRenderer(dev,shpRender,fontMap,fontTexture);
	wgtManager = new Gui::Manager(0,0,wnd->GetWidth(),wnd->GetHeight(),wgtRender);

	Gui::Label * tmpLabel = new Gui::Label(20,20,"Push Buttons");
	tmpLabel->SetTextBold(true);
	wgtManager->Add(tmpLabel);

	Gui::PushButton *tmpBtn = NULL;

	tmpBtn = new Gui::PushButton(20,40,100,20,"Push Button 1");
	tmpBtn->SetTooltip("This is the tooltip of the push button 1");
	tmpBtn->SetRoundMode(Gui::ShapeRenderer::ROUND_TOP);
	tmpBtn->SetCallback(wgtEventsHandler);

	wgtManager->Add(tmpBtn);

	tmpBtn = new Gui::PushButton(20,60,100,20,"Push Button 2");
	tmpBtn->SetTooltip("This is the tooltip of the push button 2");
	tmpBtn->SetRoundMode(Gui::ShapeRenderer::ROUND_NONE);
	tmpBtn->SetCallback(wgtEventsHandler);

	wgtManager->Add(tmpBtn);

	tmpBtn = new Gui::PushButton(20,80,100,20,"Push Button 3");
	tmpBtn->SetTooltip("This is the tooltip of the push button 3");
	tmpBtn->SetRoundMode(Gui::ShapeRenderer::ROUND_BOTTOM);
	tmpBtn->SetCallback(wgtEventsHandler);

	wgtManager->Add(tmpBtn);

	tmpBtn = new Gui::PushButton(20,110,100,20,"Push Button 4");
	tmpBtn->SetTooltip("Yet another push button tooltip");
	tmpBtn->SetRoundMode(Gui::ShapeRenderer::ROUND_ALL);
	tmpBtn->SetCallback(wgtEventsHandler);

	wgtManager->Add(tmpBtn);


	tmpLabel = new Gui::Label(20,160,"Checkbox Buttons");
	tmpLabel->SetTextBold(true);
	wgtManager->Add(tmpLabel);

	Gui::CheckBoxButton * tmpChkBox = NULL;

	tmpChkBox = new Gui::CheckBoxButton(20,180,14,"Checkbox A");
	tmpChkBox->SetValue(true);
	wgtManager->Add(tmpChkBox);
	tmpChkBox = new Gui::CheckBoxButton(20,200,14,"Checkbox B");
	wgtManager->Add(tmpChkBox);
	tmpChkBox = new Gui::CheckBoxButton(20,220,14,"Checkbox C");
	wgtManager->Add(tmpChkBox);

	tmpLabel = new Gui::Label(180,20,"Range Option Buttons");
	tmpLabel->SetTextBold(true);
	wgtManager->Add(tmpLabel);

	std::vector< std::pair<int,std::string> > roptValues;
	roptValues.reserve(4);
	roptValues.push_back( std::pair<int,std::string> (0,std::string("Option 1")));
	roptValues.push_back( std::pair<int,std::string> (1,std::string("Option 2")));
	roptValues.push_back( std::pair<int,std::string> (2,std::string("Option 3")));
	roptValues.push_back( std::pair<int,std::string> (3,std::string("Option 4")));

	Gui::RangeOptionButton * tmpRangeBtn = new Gui::RangeOptionButton(180,40,100,20,roptValues);
	tmpRangeBtn->SetTooltip("This is a option range value tooltip !");
	tmpRangeBtn->SetCallback(wgtEventsHandler);
	wgtManager->Add(tmpRangeBtn);

	tmpLabel = new Gui::Label(180,100,"Range Value Buttons");
	tmpLabel->SetTextBold(true);
	wgtManager->Add(tmpLabel);

	Gui::RangeValueButton * tmpRangeValue = new Gui::RangeValueButton(180,120,100,20,0,1,0.5);
	tmpRangeValue->SetTooltip("This is a value range button tooltip !");
	tmpRangeValue->SetCallback(wgtEventsHandler);
	wgtManager->Add(tmpRangeValue);


	tmpLabel = new Gui::Label(180,180,"Selection Button Group");
	tmpLabel->SetTextBold(true);
	wgtManager->Add(tmpLabel);

	Gui::OptionButton * op1 = new Gui::OptionButton(0,0,60,20,"Option 1");
	op1->SetRoundMode(Gui::ShapeRenderer::ROUND_LEFT);
	op1->SetValue(true);

	Gui::OptionButton * op2 = new Gui::OptionButton(60,0,60,20,"Option 2");
	op2->SetRoundMode(Gui::ShapeRenderer::ROUND_NONE);


	Gui::OptionButton * op3 = new Gui::OptionButton(120,0,60,20,"Option 3");
	op3->SetRoundMode(Gui::ShapeRenderer::ROUND_RIGHT);

	Gui::SelectionButtonGroup * sbGroup = new Gui::SelectionButtonGroup(180,200,180,20);

	wgtManager->Add(sbGroup);

	sbGroup->AddButton(1,op1);
	sbGroup->AddButton(2,op2);
	sbGroup->AddButton(4,op3);


	tmpLabel = new Gui::Label(400,20,"Generated checkbox bitmaps");
	tmpLabel->SetTextBold(true);
	wgtManager->Add(tmpLabel);

	tmpLabel = new Gui::Label(400,220,"Generated circle bitmaps");
	tmpLabel->SetTextBold(true);
	wgtManager->Add(tmpLabel);

	tmpLabel = new Gui::Label(20,280,"Bitmap + Vectorial round shapes");
	tmpLabel->SetTextBold(true);
	wgtManager->Add(tmpLabel);
}

void Demo2::Render(float dt){
	dev->Clear();
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	dev->Disable(Graph::STATE_DEPTH_TEST);

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	dev->Ortho2D(wnd->GetWidth(),wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	wgtManager->Render();

	shpRender->CheckSymbol(400,40,128,Math::Color4ub(255,255,255,255));
	shpRender->CheckSymbol(400,160,64,Math::Color4ub(255,100,0,255));
	shpRender->CheckSymbol(500,140,32,Math::Color4ub(60,60,60,255));

	shpRender->CircleShape(400,240,128,Math::Color4ub(255,255,255,255));
	shpRender->CircleShape(480,360,64,Math::Color4ub(255,155,0,255));
	shpRender->CircleShape(420,380,32,Math::Color4ub(60,60,60,255));

	shpRender->SetRoundMode(Gui::ShapeRenderer::ROUND_ALL);
	shpRender->SetRoundRadius(20);
	shpRender->Square(40,300,200,60,Math::Color4ub(255,255,255,255));

	dev->PushMatrix();
	dev->Rotate(30,0,0,1);
	shpRender->SetRoundRadius(30);
	shpRender->Square(300,200,150,60,Math::Color4ub(255,70,0,255));
	dev->PopMatrix();

	dev->PresentAll();
}

void Demo2::UpdateWndEvents(){
	Gui::UserInputState state;
	state.m_CursorX = wnd->GetMousePosition().GetX();
	state.m_CursorY = wnd->GetMousePosition().GetY();
	state.m_State_LMB = (wnd->GetMouseButtonStatus(Core::BUTTON_MOUSE_LEFT) != Core::BUTTON_STATUS_UP);
	wgtManager->Update(state);
}

Demo * CreateDemo_2(Core::Window * wnd, Graph::Device * dev){
	return new Demo2(wnd,dev);
}
