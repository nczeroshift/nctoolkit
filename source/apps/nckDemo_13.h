
/**
* NCtoolKit © 2007-2016 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 13 - Normal Map
*/

#ifndef _NCK_DEMO_13_H_
#define _NCK_DEMO_13_H_

#include "nckDemo.h"
#include <map>
#include "bxonDataIO.h"

/**
* Demo 13 class.
*/
class Demo13 : public Demo{
public:
	Demo13(Core::Window * wnd, Graph::Device * dev);
	~Demo13();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    
private:
    Scene::Compound_Base * scene;
    Scene::Camera * camera;
    Graph::Program * basic;

    float time;
};

Demo * CreateDemo_13(Core::Window * wnd, Graph::Device * dev);

#endif
