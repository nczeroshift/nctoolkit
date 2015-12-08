
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 6 - 3D Textures
*/

#ifndef _NCK_DEMO_6_H_
#define _NCK_DEMO_6_H_

#include "nckDemo.h"

/**
* Demo 6 class.
*/
class Demo6 : public Demo{
public:
	Demo6(Core::Window * wnd, Graph::Device * dev);
	~Demo6();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
	
	Scene::Compound_Basic * model;
	Graph::Texture3D * volume;
	Graph::Program * shader;
	Scene::Camera * camera;

	float zRotation;
};

Demo * CreateDemo_6(Core::Window * wnd, Graph::Device * dev);

#endif
