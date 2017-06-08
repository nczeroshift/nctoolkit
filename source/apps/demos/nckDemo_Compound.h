
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_COMPOUND_H_
#define _NCK_DEMO_COMPOUND_H_

#include "../nckDemo.h"

class Demo_Compound : public Demo{
public:
    Demo_Compound(Core::Window * wnd, Graph::Device * dev);
	~Demo_Compound();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Scene::Compound_Base * model;
    Graph::Program * shader;
    float time;
};

Demo * CreateDemo_Compound(Core::Window * wnd, Graph::Device * dev);

#endif
