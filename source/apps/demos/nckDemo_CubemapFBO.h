
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_CUBEMAP_FBO_H_
#define _NCK_DEMO_CUBEMAP_FBO_H_

#include "../nckDemo.h"

class Demo_CubemapFBO : public Demo{
public:
    Demo_CubemapFBO(Core::Window * wnd, Graph::Device * dev);
	~Demo_CubemapFBO();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    void RenderCubeMap(const Math::Vec3 & position, int face);

    int faceSize;

    Math::Vec3 lamp_pos_wm;

    Graph::RTManager * cbRT;
    Graph::TextureCubeMap * cbTex;

    Graph::Program * material;
    Graph::Program * material_cb;

    Scene::Compound_Base * box;

	float time;
};

Demo * CreateDemo_CubemapFBO(Core::Window * wnd, Graph::Device * dev);

#endif
