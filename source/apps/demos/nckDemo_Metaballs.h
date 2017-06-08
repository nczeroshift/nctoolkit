
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_METABALLS_H_
#define _NCK_DEMO_METABALLS_H_

#include "../nckDemo.h"

class Demo_Metaballs : public Demo{
public:
    Demo_Metaballs(Core::Window * wnd, Graph::Device * dev);
	~Demo_Metaballs();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Core::Chronometer * chron;

    Gui::FontMap * fontMap;
    Graph::Texture * fontTexture;

    Graph::Program * shader;
    Scene::MCRenderer * processor;

    std::vector<Scene::MCSphereShape> spheres;

    float dimensions;
    int currentLod;
    int maxLod;
    float zRotation;

    float time;
};

Demo * CreateDemo_Metaballs(Core::Window * wnd, Graph::Device * dev);

#endif
