
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_MODEL_H_
#define _NCK_DEMO_MODEL_H_

#include "../nckDemo.h"

class Demo_Model : public Demo{
public:
    Demo_Model(Core::Window * wnd, Graph::Device * dev);
	~Demo_Model();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
	float time;
};

Demo * CreateDemo_Model(Core::Window * wnd, Graph::Device * dev);

#endif
