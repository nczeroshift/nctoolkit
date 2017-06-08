
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_GUI_H_
#define _NCK_DEMO_GUI_H_

#include "../nckDemo.h"

class WidgetEventsHandler;

class Demo_GUI : public Demo{
public:
    Demo_GUI(Core::Window * wnd, Graph::Device * dev);
	~Demo_GUI();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    WidgetEventsHandler * wgtEventsHandler;
    Gui::Manager * wgtManager;
    Graph::Texture * fontTexture;
    Gui::FontMap * fontMap;
    Gui::ShapeRenderer * shpRender;
    Gui::WidgetRenderer * wgtRender;
};

Demo * CreateDemo_GUI(Core::Window * wnd, Graph::Device * dev);

#endif
