
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 2 - Graphical User Interface
*/

#ifndef _NCK_DEMO_2_H_
#define _NCK_DEMO_2_H_

#include "nckDemo.h"

class WidgetEventsHandler;

/**
* Demo 2 class.
*/
class Demo2 : public Demo{
public:
	Demo2(Core::Window * wnd, Graph::Device * dev);
	~Demo2();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();

	WidgetEventsHandler * wgtEventsHandler;
	Gui::Manager * wgtManager;
	Graph::Texture * fontTexture;
	Gui::FontMap * fontMap;
	Gui::ShapeRenderer * shpRender;
	Gui::WidgetRenderer * wgtRender;
};

Demo * CreateDemo_2(Core::Window * wnd, Graph::Device * dev);

#endif
