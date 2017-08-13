
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_AUTO_MATERIAL_H_
#define _NCK_DEMO_AUTO_MATERIAL_H_

#include "../nckDemo.h"

class Demo_AutoMaterial : public Demo{
public:
    Demo_AutoMaterial(Core::Window * wnd, Graph::Device * dev);
	~Demo_AutoMaterial();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Scene::Compound_Base * scene;
    Graph::Program * program;
	float time;
};

Demo * CreateDemo_AutoMaterial(Core::Window * wnd, Graph::Device * dev);

#endif
