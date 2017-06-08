
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_SHADOWS_H_
#define _NCK_DEMO_SHADOWS_H_

#include "../nckDemo.h"

class Demo_Shadows : public Demo{
public:
    Demo_Shadows(Core::Window * wnd, Graph::Device * dev);
	~Demo_Shadows();

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
    Graph::RTManager * rtManager;
    Graph::Texture2D * rtTexture;

    void RenderFromLight(float width, float height);
    void CaptureLightBuffer();

    Math::Mat44 lampProjViewMat;

    Graph::Program * program, *depth;

	float time;
};

Demo * CreateDemo_Shadows(Core::Window * wnd, Graph::Device * dev);

#endif
