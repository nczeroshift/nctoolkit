
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_BUMPMAP_H_
#define _NCK_DEMO_BUMPMAP_H_

#include "../nckDemo.h"

class Demo_Bumpmap : public Demo{
public:
    Demo_Bumpmap(Core::Window * wnd, Graph::Device * dev);
	~Demo_Bumpmap();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
	float time;
    Scene::Compound_Base * scene;
    Graph::Program * material;
};

Demo * CreateDemo_Bumpmap(Core::Window * wnd, Graph::Device * dev);

#endif
