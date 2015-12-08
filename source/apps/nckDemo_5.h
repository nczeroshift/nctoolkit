
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 5 - Marching cubes
*/

#ifndef _NCK_DEMO_5_H_
#define _NCK_DEMO_5_H_

#include "nckDemo.h"
#include "nckMarchingCubes.h"

 /**
* Demo 5 class.
*/
class Demo5 : public Demo{
public:
	Demo5(Core::Window * wnd, Graph::Device * dev);
	~Demo5();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();

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
};

Demo * CreateDemo_5(Core::Window * wnd, Graph::Device * dev);

#endif
