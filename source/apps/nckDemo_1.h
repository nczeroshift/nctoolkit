
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 1 - Caravela Portuguesa
*/

#ifndef _NCK_DEMO_1_H_
#define _NCK_DEMO_1_H_

#include "nckDemo.h"

/**
* Demo 1 class.
*/
class Demo1 : public Demo{
public:
	Demo1(Core::Window * wnd, Graph::Device * dev);
	~Demo1();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();

	Scene::Compound_Basic * model;
	Graph::Program * shader;
	Scene::Camera * camera;
	float zRotation;
};

Demo * CreateDemo_1(Core::Window * wnd, Graph::Device * dev);

#endif
