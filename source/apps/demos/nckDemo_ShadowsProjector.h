
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_SHADOWSPROJECTOR_H_
#define _NCK_DEMO_SHADOWSPROJECTOR_H_

#include "../nckDemo.h"

class Demo_ShadowsProjector : public Demo{
public:
	Demo_ShadowsProjector(Core::Window * wnd, Graph::Device * dev);
	~Demo_ShadowsProjector();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    int bufferSize;
    Scene::Compound_Base * scene;
    Scene::Camera * camera;
    Scene::Lamp * lamp;

    Graph::RTManager * rtShadow, *rtDirect;
    Graph::Texture2D * texShadow, * texColor, * texPosition, * texNormal;

    Math::Mat44 lampProjViewMat;
    Math::Mat44 lampViewMat;

    Graph::Program * program, *depth, *display, *sampler;

	float time;
};

Demo * CreateDemo_ShadowsProjector(Core::Window * wnd, Graph::Device * dev);

#endif
