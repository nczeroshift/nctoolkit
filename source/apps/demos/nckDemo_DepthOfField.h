
/**
* NCtoolKit © 2007-2018 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_DEPTH_OF_FIELD_H_
#define _NCK_DEMO_DEPTH_OF_FIELD_H_

#include "../nckDemo.h"

class Demo_DepthOfField : public Demo{
public:
    Demo_DepthOfField(Core::Window * wnd, Graph::Device * dev);
	~Demo_DepthOfField();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Gui::ShapeRenderer * shape;
    int width, height;
    Graph::Program * prog;
    Scene::Compound_Base * model;
    Graph::Texture2D * tex1;
    Graph::Texture2D * tex2;
    Graph::RTManager * mng;
	float time;
};

Demo * CreateDemo_DepthOfField(Core::Window * wnd, Graph::Device * dev);

#endif
