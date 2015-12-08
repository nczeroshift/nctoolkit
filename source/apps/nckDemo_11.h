
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 11 - BXON Demo
*/

#ifndef _NCK_DEMO_11_H_
#define _NCK_DEMO_11_H_

#include "nckDemo.h"
#include <map>
#include "bxonDataIO.h"

/**
* Demo 11 class.
*/
class Demo11 : public Demo{
public:
	Demo11(Core::Window * wnd, Graph::Device * dev);
	~Demo11();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    
private:
    Scene::Compound_Basic * scene;
    Scene::Camera * camera;
    Graph::Program * basic;
    float time;
};

Demo * CreateDemo_11(Core::Window * wnd, Graph::Device * dev);

#endif
