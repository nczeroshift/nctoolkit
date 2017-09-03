
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_FRUSTUM_H_
#define _NCK_DEMO_FRUSTUM_H_

#include "../nckDemo.h"

class Demo_Frustum : public Demo{
public:
    Demo_Frustum(Core::Window * wnd, Graph::Device * dev);
	~Demo_Frustum();

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

Demo * CreateDemo_Frustum(Core::Window * wnd, Graph::Device * dev);

#endif
