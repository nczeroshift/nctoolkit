
/**
* NCtoolKit © 2007-2018 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_SSAO_H_
#define _NCK_DEMO_SSAO_H_

#include "../nckDemo.h"

class Demo_SSAO : public Demo{
public:
    Demo_SSAO(Core::Window * wnd, Graph::Device * dev);
	~Demo_SSAO();

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

    Graph::RTManager * mng;
    Graph::Texture2D * tex_position;
    Graph::Texture2D * tex_normal;
    Graph::Texture2D * tex_direct;

	float time;
};

Demo * CreateDemo_SSAO(Core::Window * wnd, Graph::Device * dev);

#endif
