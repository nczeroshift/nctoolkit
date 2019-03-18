
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_FRUSTUM2_H_
#define _NCK_DEMO_FRUSTUM2_H_

#include "../nckDemo.h"

class Demo_Frustum2 : public Demo{
public:
	Demo_Frustum2(Core::Window * wnd, Graph::Device * dev);
	~Demo_Frustum2();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Scene::Compound_Base * models;
    Graph::Texture * fontTexture;
    Gui::FontMap * fontMap;
    float time;
};

Demo * CreateDemo_Frustum2(Core::Window * wnd, Graph::Device * dev);

#endif
