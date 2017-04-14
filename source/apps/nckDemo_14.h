
/**
* NCtoolKit © 2007-2016 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 14 - Shadow maps
*/

#ifndef _NCK_DEMO_14_H_
#define _NCK_DEMO_14_H_

#include "nckDemo.h"
#include <map>
#include "bxonDataIO.h"

/**
* Demo 14 class.
*/
class Demo14 : public Demo{
public:
	Demo14(Core::Window * wnd, Graph::Device * dev);
	~Demo14();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    
private:
    Scene::Compound_Base * scene;
    Scene::Camera * camera;
    Scene::Lamp * lamp;
    Graph::RTManager * rtManager;
    Graph::Texture2D * rtTexture;

    void RenderFromLight(float width, float height);
    void CaptureLightBuffer();

    Math::Mat44 lampProjViewMat;
    
    Graph::Program * program, * depth;
    float time;
};

Demo * CreateDemo_14(Core::Window * wnd, Graph::Device * dev);

#endif
