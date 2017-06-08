
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_CURVES_H_
#define _NCK_DEMO_CURVES_H_

#include "../nckDemo.h"

class Demo_Curves : public Demo{
public:
    Demo_Curves(Core::Window * wnd, Graph::Device * dev);
	~Demo_Curves();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Scene::Compound_Base * curves;
};

Demo * CreateDemo_Curves(Core::Window * wnd, Graph::Device * dev);

#endif
